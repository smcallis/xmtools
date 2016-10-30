#include "xmtools.h"

namespace xm {

    //{{{ linear algebra (work in progress)
    namespace internal {

        //
        // "The Complex Householder Transform", Kuo-Liang Chung and Wen-Ming Yan
        // IEEE Transactions on Sig Proc, Vol. 45, No. 9, September 1997
        // 
        // There are two pairs of Householder functions here.  One pair works
        // on columns from the left of the matrix, and the other works on rows
        // form the right.  The first function of each pair builds the
        // Householder vector and modifies the original matrix.  The second
        // function can be used to apply that Householder vector to other
        // matrices.  The second function also has an optional last argument
        // which is used to determine whether you're trying to apply the actual
        // Householder transform, or if you're trying to recover the matrix for
        // a decomposition.  Note that for decompositions, the transforms must
        // be applied in reverse order to recover the combined Householder
        // matrix (Q below).
        //
        // For a least squares problems:
        // 
        //     Assume we start with:              A X = B
        //     householder_col(A, ...) implements:     (H A)
        //     householder_col(A, ,,, B, ...) gives:   (H B)
        //     allowing us to make progress on:  H A X = H B
        //
        // For matrix decompositions:
        //
        //     Assume we start with:     A and I
        //     householder_col(A, ...) implements:     R = (H A)
        //     householder_col(A, ..., Q, ..., true):   Q = (H I)
        //
        // See gensolve(), qrdecomp(), and lqdecomp() for examples.
        //

        // Apply a Householder col from A to cols of B starting at low
        template<class atype, class btype>
        void householder_col(
            const atype* A, int64 arows, int64 acols,
                  btype* B, int64 brows, int64 bcols,
            int64 row, int64 col, int64 low=0, bool recover=false
        ) {
            check(arows == brows, "matching rows");
            atype pivot = A[row*acols + col];
            if (pivot == 0) return;
            if (recover) pivot = conj(pivot);
            atype scale = 1.0/pivot;
            for (int64 jj = low; jj<bcols; jj++) {
                typename promotion<atype, btype>::type sum = 0;
                for (int64 ii = row; ii<arows; ii++) {
                    sum += B[ii*bcols + jj] * conj(A[ii*acols + col]);
                }
                sum *= scale;
                for (int64 ii = row; ii<arows; ii++) {
                    B[ii*bcols + jj] += sum * A[ii*acols + col];
                }
            }
        }

        template<class type>
        double householder_col(
            type* H, int64 rows, int64 cols,
            int64 row, int64 col
        ) {
            // calculate the vector magnitude
            double magr = 0;
            for (int64 ii = row; ii<rows; ii++) {
                magr = hypot(magr, H[ii*cols + col]);
            }
            if (magr == 0) return 0; // it's a zero col
            if (real(H[row*cols + col]) > 0) magr = -magr;
            double invr = 1/magr;
            for (int64 ii = row; ii<rows; ii++) {
                H[ii*cols + col] *= invr;
            }
            H[row*cols + col] -= 1;
            householder_col(H, rows, cols, H, rows, cols, row, col, col + 1);
            return magr;
        }

        // Apply a Household row from H to rows of A starting at low
        template<class atype, class btype>
        void householder_row(
            const atype* A, int64 arows, int64 acols,
                  btype* B, int64 brows, int64 bcols,
            int64 row, int64 col, int64 low=0, bool recover=false
        ) {
            (void)arows; // not needed, but it looks nicer
            check(acols == bcols, "matching cols");
            atype pivot = A[row*acols + col];
            if (pivot == 0) return;
            if (recover) pivot = conj(pivot);
            atype scale = 1.0/pivot;
            for (int64 ii = low; ii<brows; ii++) {
                typename promotion<atype, btype>::type sum = 0;
                for (int64 jj = col; jj<acols; jj++) {
                    sum += B[ii*bcols + jj] * conj(A[row*acols + jj]);
                }
                sum *= scale;
                for (int64 jj = col; jj<acols; jj++) {
                    B[ii*bcols + jj] += sum * A[row*acols + jj];
                }
            }
        }

        template<class type>
        double householder_row(
            type* H, int64 rows, int64 cols,
            int64 row, int64 col
        ) {
            // calculate the vector magnitude
            double magr = 0;
            for (int64 jj = col; jj<cols; jj++) {
                magr = hypot(magr, H[row*cols + jj]);
            }
            if (magr == 0) return 0; // it's a zero row
            if (real(H[row*cols + col]) > 0) magr = -magr;
            double invr = 1/magr;
            for (int64 jj = col; jj<cols; jj++) {
                H[row*cols + jj] *= invr;
            }
            H[row*cols + col] -= 1;
            householder_row(H, rows, cols, H, rows, cols, row, col, row + 1);
            return magr;
        }

        // Apply the inverse of Householder row from A to R
        template<class atype, class rtype>
        void inverse_row_col(
            const atype* A, int64 arows, int64 acols,
                  rtype* R, int64 rrows, int64 rcols,
            int64 row, int64 col, int64 low=0
        ) {
            (void)arows; // not needed, but it looks nicer
            check(acols == rrows, "matching sizes");
            atype pivot = A[row*acols + col];
            if (pivot == 0) return;
            atype scale = 1.0/pivot;
            for (int64 jj = low; jj<rcols; jj++) {
                typename promotion<atype, rtype>::type sum = 0;
                for (int64 kk = col; kk<acols; kk++) {
                    sum += R[kk*rcols + jj] * A[row*acols + kk];
                }
                sum *= scale;
                for (int64 kk = col; kk<acols; kk++) {
                    R[kk*rcols + jj] += sum * conj(A[row*acols + kk]);
                }
            }
        }

        /*
        // Apply the inverse of Householder col from A to L
        template<class atype, class ltype>
        void inverse_col_row(
            const atype* A, int64 arows, int64 acols,
                  ltype* L, int64 lrows, int64 lcols,
            int64 row, int64 col, int64 low=0
        ) {
            check(arows == lcols, "matching sizes");
            atype pivot = A[row*acols + col];
            if (pivot == 0) return;
            atype scale = 1.0/conj(pivot);
            for (int64 ii = low; ii<lrows; ii++) {
                typename promotion<atype, atype>::type sum = 0;
                for (int64 kk = row; kk<arows; kk++) {
                    sum += L[ii*lcols + kk] * A[kk*acols + col];
                }
                sum *= scale;
                for (int64 kk = row; kk<arows; kk++) {
                    L[ii*lcols + kk] += sum * conj(A[kk*acols + col]);
                }
            }
        }
        */


        struct givens_rotation {
            double rr, cc, ss;
            givens_rotation(double r0, double c0, double s0) :
                rr(r0), cc(c0), ss(s0) {}
            givens_rotation(double xx, double yy) {
                rr = hypot(xx, yy);
                if (rr <= 0) {
                    cc = 1;
                    ss = 0;
                } else {
                    cc = xx/rr;
                    ss = yy/rr;
                }
            }
            void rotate(double& xx, double& yy) {
                double uu =  xx*cc + yy*ss;
                double vv = -xx*ss + yy*cc;
                xx = uu;
                yy = vv;
            }
            void rotate(double* aa, int64 ii, int64 size) {
                if (!aa) return;
                double* lo = aa + (ii + 0)*size;
                double* hi = aa + (ii + 1)*size;
                for (int64 jj = 0; jj<size; jj++) {
                    rotate(lo[jj], hi[jj]);
                }
            }
        };

        static void reverse_submatrix(
            double* dd, double* ff, int64 lo, int64 hi,
            double* ll, double* rr, int64 size
        ) {
            for (int64 ii = 0; ii<(hi - lo)/2; ii++) {
                xm::swap(dd[lo + ii], dd[hi - 1 - ii]);
                if (ll) for (int64 jj = 0; jj<size; jj++) {
                    xm::swap(ll[(lo + ii)*size + jj], ll[(hi - 1 - ii)*size + jj]);
                }
                if (rr) for (int64 jj = 0; jj<size; jj++) {
                    xm::swap(rr[(lo + ii)*size + jj], rr[(hi - 1 - ii)*size + jj]);
                }
            }
            for (int64 ii = 0; ii<(hi - 1 - lo)/2; ii++) {
                xm::swap(ff[lo + ii], ff[hi - 2 - ii]);
            }
            for (int64 ii = lo; ii<hi - 1; ii++) {
                givens_rotation gg(dd[ii], ff[ii]);
                dd[ii] = gg.rr; ff[ii] = 0;
                gg.rotate(ff[ii], dd[ii+1]);
                gg.rotate(ll, ii, size);
            }
        }

        // Golub and Van Loan, "Matrix Computations", 4th edition, Section 8.3.4
        static givens_rotation wilkinson_shift(
            double* dd, double* ff, int64 lo, int64 hi, double* pp
        ) {
            if (pp) {
                double uu = pp[hi - 1]*pp[hi - 1];
                return givens_rotation(dd[lo]*dd[lo] - uu, dd[lo]*ff[lo]);
            }
            double aa = dd[hi - 2]*dd[hi - 2];
            double bb = dd[hi - 2]*ff[hi - 2];
            double cc = ff[hi - 2]*ff[hi - 2] + dd[hi - 1]*dd[hi - 1];
            double ee = 0.5*(aa - cc);
            double uu = cc + ee - copysign(1, ee)*hypot(ee, bb);
            return givens_rotation(dd[lo]*dd[lo] - uu, dd[lo]*ff[lo]);
            //return givens_rotation(dd[lo] - uu/dd[lo], ff[lo]);
        }

        //
        // Golub and Van Loan, "Matrix Computations", 4th edition, Section 8.6.1
        //
        //  +----------------------------------+  +----------------------------------+
        //  |                                  |  |                                  |
        //  | bulge chasing:                   |  |  sweep top:   sweep row:         |
        //  |                                  |  |                                  |
        //  |  d0 <-- f0 <-- b1                |  |   d0 <- f0     di   fi   bj+1    |
        //  |   ^      ^      ^                |  |                 ^    ^    ^      |
        //  |   |      |      |                |  |   b0 <- d1      |    |    |      |
        //  |  b0 <-- d1 <-- f1 <-- b3         |  |                bj   di+1 fi+1    |
        //  |          ^      ^      ^         |  |                                  |
        //  |          |      |      |         |  |                                  |
        //  |         b2 <-- d2 <-- f2 <-- b5  |  |  sweep col:                      |
        //  |                 ^      ^      ^  |  |                     sweep end:   |
        //  |                 |      |      |  |  |     fi <-- bj                    |
        //  |                b4 <-- d3 <-- f3  |  |                      d3   f3     |
        //  |                        ^      ^  |  |   di+1 <-- fi+1       ^    ^     |
        //  |                        |      |  |  |                       |    |     |
        //  |                       b6 <-- d4  |  |   bj+1 <-- di+2      b6   d4     |
        //  |                                  |  |                                  |
        //  +----------------------------------+  +----------------------------------+
        //
        static inline void golub_kahan(
            double* dd, double* ff, int64 lo, int64 hi,
            double* ll, double* rr, int64 size, double* pp
        ) {
            if (3*fabs(dd[lo]) < fabs(dd[hi - 1])) {
                reverse_submatrix(dd, ff, lo, hi, ll, rr, size);
            }

            // sweep top
            double bb = 0;
            givens_rotation gt = wilkinson_shift(dd, ff, lo, hi, pp);
            gt.rotate(dd[lo], ff[lo]);
            gt.rotate(bb, dd[lo + 1]);
            gt.rotate(rr, lo, size);

            for (int64 ii = lo; ii<hi-2; ii++) {
                // sweep row
                givens_rotation gr(dd[ii], bb);
                dd[ii] = gr.rr; bb = 0;
                gr.rotate(ff[ii], dd[ii + 1]);
                gr.rotate(bb, ff[ii + 1]);
                gr.rotate(ll, ii, size);

                // sweep col
                givens_rotation gc(ff[ii], bb);
                ff[ii] = gc.rr; bb = 0;
                gc.rotate(dd[ii + 1], ff[ii + 1]);
                gc.rotate(bb, dd[ii + 2]);
                gc.rotate(rr, ii + 1, size);
            }

            // sweep end
            givens_rotation ge(dd[hi - 2], bb);
            dd[hi - 2] = ge.rr;
            ge.rotate(ff[hi - 2], dd[hi - 1]);
            ge.rotate(ll, hi - 2, size);
        }

        static inline void split_diagonal(
            double* dd, double* ff, int64 lo, int64 hi,
            double* ll, double* rr, int64 size
        ) {
            for (int64 ii = lo; ii<hi - 1; ii++) {
                givens_rotation gg(dd[ii], ff[ii]);
                dd[ii] = gg.rr; ff[ii] = 0;
                gg.rotate(ff[ii], dd[ii+1]);
                gg.rotate(rr, ii, size);
            }
            for (int64 ii = lo; ii<hi - 1; ii++) {
                givens_rotation gg(dd[ii], ff[ii]);
                dd[ii] = gg.rr; ff[ii] = 0;
                gg.rotate(ff[ii], dd[ii+1]);
                gg.rotate(ll, ii, size);
            }
        }

        //
        // Loosely from "Accuracy of two SVD algorithms for 2x2 triangular
        // matrices", by V. Harii and J. Matejas, in Applied Mathematics and
        // Computation 2009.
        //
        static void upper_2x2_svd(
            double* dd, double* ff, int64 lo,
            double* ll, double* rr, int64 size
        ) {
            double aa = dd[lo + 0];
            double bb = ff[lo + 0];
            double cc = dd[lo + 1];

            // Check if it's already diagonal
            if (bb == 0) return;

            if (aa == 0) { // shortcut
                double hh = hypot(bb, cc);
                dd[lo] = ff[lo] = 0;
                dd[lo + 1] = hh;
                if (ll) {
                    givens_rotation gg(0, cc/hh, -bb/hh);
                    gg.rotate(ll, lo, size);
                }
                return;
            }

            if (cc == 0) { // shortcut
                double hh = hypot(aa, bb);
                dd[lo] = hh;
                ff[lo] = dd[lo + 1] = 0;
                if (rr) {
                    givens_rotation gg(0, aa/hh, bb/hh);
                    gg.rotate(rr, lo, size);
                }
                return;
            }

            double a2 = aa*aa;
            double b2 = bb*bb;
            double c2 = cc*cc;

            double Rc, Rs, Lc, Ls;
            if (a2 > c2) {
                double L = .5*atan2(-2*bb*cc, a2 + b2 - c2);
                Lc = cos(L);
                Ls = sin(L);
                double xx, yy;
                if (fabs(Ls*bb + Lc*cc) > fabs(Lc*bb - Ls*cc)) {
                    xx = Ls*bb + Lc*cc;
                    yy = -Ls*aa;
                } else {
                    xx = Lc*aa;
                    yy = Lc*bb - Ls*cc;
                }
                double hh = hypot(xx, yy);
                Rc = xx/hh;
                Rs = yy/hh;
            } else {
                double R = .5*atan2(-2*aa*bb, c2 + b2 - a2);
                Rc = cos(R);
                Rs = sin(R);
                double xx, yy;
                if (fabs(Rc*aa + Rs*bb) > fabs(Rc*bb - Rs*aa)) {
                    xx = Rc*aa + Rs*bb;
                    yy = -Rs*cc;
                } else {
                    xx = Rc*cc;
                    yy = Rc*bb - Rs*aa;
                }
                double hh = hypot(xx, yy);
                Lc = xx/hh;
                Ls = yy/hh;
            }

            dd[lo + 0] =  Rc*Lc*aa + Rs*Lc*bb - Rs*Ls*cc;
            dd[lo + 1] = -Rs*Ls*aa + Rc*Ls*bb + Rc*Lc*cc;
            ff[lo] = 0;

            if (ll) {
                givens_rotation gg(0, Lc, -Ls);
                gg.rotate(ll, lo, size);
            }

            if (rr) {
                givens_rotation gg(0, Rc, Rs);
                gg.rotate(rr, lo, size);
            }
        }

        static inline void block_diag_svd(
            double* dd, double* ff, int64 lo, int64 hi,
            double* ll, double* rr, int64 size, double* pp
        ) {
            int its = 0;
            while (hi - lo > 2) {
                const double eps = 1e-20;

                // look for opportunities to recurse from the off diagonal
                for (int64 ii = lo; ii<hi - 1; ii++) {
                    if (fabs(ff[ii]) < eps*(fabs(dd[ii]) + fabs(dd[ii + 1]))) {
                        if ((ii - lo) < (hi - lo)/2) {
                            block_diag_svd(dd, ff, lo, ii + 1, ll, rr, size, pp);
                            lo = ii + 1;
                        } else {
                            block_diag_svd(dd, ff, ii + 1, hi, ll, rr, size, pp);
                            hi = ii + 1;
                            break;
                        }
                        its = 0;
                    }
                }

                // look for opportunities to recurse from the main diagonal
                for (int64 ii = lo; ii<hi; ii++) {
                    double f0 = (ii == lo) ? 0 : fabs(ff[ii - 1]);
                    double f1 = (ii == hi - 1) ? 0 : fabs(ff[ii]);
                    if (fabs(dd[ii]) < eps*(f0 + f1)) {
                        split_diagonal(dd, ff, lo, hi, ll, rr, size);
                        if ((ii - lo) < (hi - lo)/2) {
                            block_diag_svd(dd, ff, lo, ii, ll, rr, size, pp);
                            lo = ii; hi = hi - 1;
                        } else {
                            block_diag_svd(dd, ff, ii, hi - 1, ll, rr, size, pp);
                            hi = ii;
                            break;
                        }
                        its = 0;
                    }
                }

                if (hi - lo > 2) {
                    golub_kahan(dd, ff, lo, hi, ll, rr, size, ++its > 3 ? 0 : pp);
                }
            }
            
            if (hi - lo == 2) {
                upper_2x2_svd(dd, ff, lo, ll, rr, size);
            }
        }

        struct compare_singular_vals {
            const double* pp;
            compare_singular_vals(const double* dd) : pp(dd) {}
            bool operator ()(int64 ii, int64 jj) {
                return pp[ii] > pp[jj];
            }
        };

        static inline void bidiagonal_svd(
            int64* tt, double* dd, double* ff,
            double* ll, double* rr, int64 size, double* pp
        ) {
            // For numerical precision, we scale the array to near unity
            double frob = 0;
            for (int64 ii = 0; ii<size; ii++)   frob = hypot(frob, dd[ii]);
            for (int64 ii = 0; ii<size-1; ii++) frob = hypot(frob, ff[ii]);
            frob /= size;
            if (frob == 0) return;
            double inv = 1/frob;
            for (int64 ii = 0; ii<size; ii++)   dd[ii] *= inv;
            for (int64 ii = 0; ii<size-1; ii++) ff[ii] *= inv;

            block_diag_svd(dd, ff, 0, size, ll, rr, size, pp);
            
            // negate any negative elements
            for (int64 ii = 0; ii<size; ii++) if (dd[ii] < 0) {
                dd[ii] *= -1;
                if (ll) for (int64 jj = 0; jj<size; jj++) {
                    ll[ii*size + jj] *= -1;
                }
            }

            // Swaps are expensive with large arrays, so we sort the indices first
            // and then do at most N swaps instead of O(N log N) in the sort.
            for (int64 ii = 0; ii<size; ii++) tt[ii] = ii;
            compare_singular_vals decreasing(dd);
            introsort(tt, size, decreasing);

            for (int64 ii = 0; ii<size; ii++) {
                int64 spot = ii;
                int64 want = tt[spot];
                while (spot != want) {
                    xm::swap(dd[spot], dd[want]);
                    if (ll) for (int64 jj = 0; jj<size; jj++) {
                        xm::swap(ll[spot*size + jj], ll[want*size + jj]);
                    }
                    if (rr) for (int64 jj = 0; jj<size; jj++) {
                        xm::swap(rr[spot*size + jj], rr[want*size + jj]);
                    }
                    tt[spot] = spot;
                    spot = want;
                    want = tt[spot];
                    if (want == ii) {
                        tt[spot] = spot;
                        break;
                    }
                }
            }

            // Correct for the scaling we did above
            for (int64 ii = 0; ii<size; ii++)   dd[ii] *= frob;
            for (int64 ii = 0; ii<size-1; ii++) ff[ii] *= frob;
        }
    }

    //}}}

    //
    // Solve: A X = B, Given: A[rows, cols], X[cols, size], B[rows, size]
    // 
    // This function finds the least-squares or least-norm solution to the
    // system of linear equations.  Each of A, X, and B will be modified with
    // the final result in X.
    //
    // If provided, P is used as scratch space and should be P[rows] large.
    // If not provided, the routine will allocate and free temporary space.
    //
    template<class type>
    int64 gensolve(
        type* A, int64 rows, int64 cols, 
        type* X, type* B, int64 size=1,
        double tol=1e-12, int64* P=0
    ) {
        using namespace internal;
        int64 rank = min(rows, cols);
        int64* perm = P ? P : alloc<int64>(rank*sizeof(int64));

        // do a rank-revealing QR decomposition
        for (int64 ij = 0; ij<rank; ij++) {
            // find largest col for pivoting
            perm[ij] = -1;
            double maxsum = -1;
            for (int64 jj = ij; jj<cols; jj++) {
                double sum = 0;
                for (int64 ii = ij; ii<rows; ii++) {
                    sum = hypot(sum, A[ii*cols + jj]);
                }
                if (sum > maxsum) {
                    perm[ij] = jj;
                    maxsum = sum;
                }
            }

            if (ij != perm[ij]) {
                // do the column pivot
                for (int64 ii = 0; ii<rows; ii++) {
                    swap(A[ii*cols + ij], A[ii*cols + perm[ij]]);
                }
            }

            // do the Householder transform to A and B
            double diag = householder_col(A, rows, cols, ij, ij);
            householder_col(A, rows, cols, B, rows, size, ij, ij);

            // remove the Householder vector we dumped into A
            A[ij*cols + ij] = diag;
            for (int64 jj = 0; jj<ij; jj++) {
                A[ij*cols + jj] = 0;
            }

            // Compare to our tolerance to find the rank.  Note: On the first
            // pass, his can only be true if the diagonal is actually zero, in
            // which case our rank really is zero.
            if (::fabs(diag) <= tol*::fabs(real(A[0]))) {
                rank = ij;
                break;
            }
        }

        // do a bi-diagonal decomposition (diagonal and sub-diagonal)
        for (int64 ij = 0; ij<rank - 1; ij++) {
            double diag = householder_row(A, rows, cols, ij, ij);
            double offd = householder_col(A, rows, cols, ij+1, ij);
            householder_col(A, rank, cols, B, rank, size, ij+1, ij);
            // forward ellimation with the diagonal and sub-diagonal
            for (int64 jj = 0; jj<size; jj++) {
                B[ij*size + jj] /= diag;
                B[(ij + 1)*size + jj] -= offd*B[ij*size + jj];
            }
        }
        // do the final division for the last diagonal
        double diag = householder_row(A, rows, cols, rank - 1, rank - 1);
        for (int64 jj = 0; jj<size; jj++) {
            B[(rank - 1)*size + jj] /= diag;
        }

        // copy from B into X, and zero extend if needed
        for (int64 ii = 0; ii<rank; ii++) {
            for (int64 jj = 0; jj<size; jj++) {
                X[ii*size + jj] = B[ii*size + jj];
            }
        }
        for (int64 ii = rank; ii<cols; ii++) {
            for (int64 jj = 0; jj<size; jj++) {
                X[ii*size + jj] = 0;
            }
        }

        // apply our householder rows to X
        for (int64 ij = rank - 1; ij>=0; ij--) {
            inverse_row_col(A, rows, cols, X, cols, size, ij, ij);
        }

        // apply our permutations to X
        for (int64 ii = rank - 1; ii>=0; ii--) {
            if (perm[ii] != ii) {
                for (int64 jj = 0; jj<size; jj++) {
                    swap(X[ii*size + jj], X[perm[ii]*size + jj]);
                }
            }
        }

        if (!P) ::free(perm);

        return rank;
    }

}

static void dump(const char* name, const xm::mat<xm::cdouble>& matrix) {
    fprintf(stderr, "%s:\n", name);
    for (int64 ii = 0; ii<matrix.rows(); ii++) {
        for (int64 jj = 0; jj<matrix.cols(); jj++) {
            printf("\t%+.3lf%+.3lf", matrix(ii, jj).re, matrix(ii, jj).im);
        }
        printf("\n");
    }
}

static inline xm::mat<xm::cdouble> matmul(
    xm::mat<xm::cdouble> aa, xm::mat<xm::cdouble> bb
) {
    xm::check(aa.cols() == bb.rows(), "compatible sizes");
    int64 size = aa.cols();
    xm::mat<xm::cdouble> cc(aa.rows(), bb.cols());
    for (int64 ii = 0; ii<cc.rows(); ii++) {
        for (int64 jj = 0; jj<cc.cols(); jj++) {
            cc(ii, jj) = 0;
            for (int64 kk = 0; kk<size; kk++) {
                cc(ii, jj) += aa(ii, kk)*bb(kk, jj);
            }
        }
    }
    return cc;
}

namespace xm {
}

int main() {
    using namespace xm;
    using namespace xm::internal;

    prng random(0);

    const int64 rows = 16;
    const int64 cols = 16;
    const int64 size = 16;
    mat<cdouble> A(rows, cols);
    mat<cdouble> X(cols, size);
    mat<cdouble> B(rows, size);

    for (int64 ii = 0; ii<rows; ii++) {
        for (int64 jj = 0; jj<cols; jj++) {
            A(ii, jj).re = int64(random.uint64()%18) - 9;
            A(ii, jj).im = int64(random.uint64()%18) - 9;
        }
    }
    for (int64 ii = 0; ii<rows; ii++) {
        for (int64 jj = 0; jj<size; jj++) {
            B(ii, jj).re = int64(random.uint64()%18) - 9;
            B(ii, jj).im = int64(random.uint64()%18) - 9;
        }
    }

    mat<cdouble> acopy = A;
    mat<cdouble> bcopy = B;

    //int64 P[rows];
    gensolve(
        acopy.data(), rows, cols,
        X.data(), bcopy.data(), size
    );

    dump("A", A);
    dump("X", X);
    dump("B", B);
    dump("A*X", matmul(A, X));
    dump("B - A*X", B - matmul(A, X));

    return 0;
}

    /*
    const int64 rows = 9;
    const int64 cols = 5;
    mat<cdouble> A(rows, cols);
    mat<cdouble> lilQ = ident<cdouble>(rows, cols);
    mat<cdouble> bigQ = ident<cdouble>(rows, rows);
    for (int64 ii = 0; ii<rows; ii++) {
        for (int64 jj = 0; jj<cols; jj++) {
            A(ii, jj).re = int64(random.uint64()%18) - 9;
            A(ii, jj).im = int64(random.uint64()%18) - 9;
        }
    }
    dump("A", A);
    //getchar();

    mat<cdouble> lilR(cols, cols, 0);
    mat<cdouble> bigR(rows, cols, 0);
    const int64 size = min(rows, cols);
    for (int64 ij = 0; ij<size; ij++) {
        lilR(ij, ij) = bigR(ij, ij) = householder_col(
            A.data(), A.rows(), A.cols(),
            ij, ij
        );
    }
    for (int64 ij = size - 1; ij>=0; ij--) {
        householder_col(
            A.data(), A.rows(), A.cols(),
            lilQ.data(), lilQ.rows(), lilQ.cols(),
            ij, ij, 0, true
        );
        householder_col(
            A.data(), A.rows(), A.cols(),
            bigQ.data(), bigQ.rows(), bigQ.cols(),
            ij, ij, 0, true
        );
    }

    for (int64 ii = 0; ii<cols; ii++) {
        for (int64 jj = ii + 1; jj<cols; jj++) {
            lilR(ii, jj) = bigR(ii, jj) = A(ii, jj);
        }
    }
    dump("big R", bigR);
    dump("big Q*R", matmul(bigQ, bigR));
    dump("lil R", lilR);
    dump("lil Q*R", matmul(lilQ, lilR));
    */
        /*
        householder_row(A.data(), A.rows(), A.cols(), ij, ij + 1);
        reflect_row_row(
            A.data(), A.rows(), A.cols(),
            C.data(), C.rows(), C.cols(),
            ij, ij + 1, ij
        );
        inverse_row_col(
            A.data(), A.rows(), A.cols(),
            R.data(), R.rows(), R.cols(),
            ij, ij + 1
        );

        //dump("A", A);
        dump("C", C);
        dump("L*L'", matmul(L, herm(L)));
        dump("R*R'", matmul(R, herm(R)));
        dump("L*C*R", matmul(matmul(L, C), R));
        getchar();
        */

        /*
        householder_row(A.data(), A.rows(), A.cols(), ij, ij + 1);
        reflect_row_row(
            A.data(), A.rows(), A.cols(),
            C.data(), C.rows(), C.cols(),
            ij, ij + 1, ij
        );
        dump("A", A);
        dump("C", C);
        getchar();
        */


