#ifndef XM_SVDECOMP_H_
#define XM_SVDECOMP_H_ 1

#include "householder.h"

namespace xm {
    //{{{ internal
    namespace internal {

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
            double* dd, double* ff, int64 lo, int64 hi
        ) {
            double aa = dd[hi - 2]*dd[hi - 2];
            double bb = dd[hi - 2]*ff[hi - 2];
            double cc = ff[hi - 2]*ff[hi - 2] + dd[hi - 1]*dd[hi - 1];
            double ee = 0.5*(aa - cc);
            double uu = cc + ee - copysign(1, ee)*hypot(ee, bb);
            check(dd[lo] != 0, "no divides by zero");
            return givens_rotation(dd[lo] - uu/dd[lo], ff[lo]);
            //return givens_rotation(dd[lo]*dd[lo] - uu, dd[lo]*ff[lo]);
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
            double* ll, double* rr, int64 size
        ) {
            if (3*fabs(dd[lo]) < fabs(dd[hi - 1])) {
                reverse_submatrix(dd, ff, lo, hi, ll, rr, size);
            }

            // sweep top
            double bb = 0;
            givens_rotation gt = wilkinson_shift(dd, ff, lo, hi);
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
            double* ll, double* rr, int64 size
        ) {
            while (hi - lo > 2) {
                /*
                static int64 iteration;
                fprintf(stderr, "iter: %lld\n", ++iteration);
                fprintf(stderr, "     ff: ");
                for (int64 ii = 0; ii<size - 1; ii++) fprintf(stderr, "\t%lf", ff[ii]);
                fprintf(stderr, "\ndd: ");
                for (int64 ii = 0; ii<size; ii++) fprintf(stderr, "\t%lf", dd[ii]);
                fprintf(stderr, "\n");
                getchar();
                */

                const double eps = 1e-20;

                // look for opportunities to recurse from the off diagonal
                for (int64 ii = lo; ii<hi - 1; ii++) {
                    if (fabs(ff[ii]) < eps*(fabs(dd[ii]) + fabs(dd[ii + 1]))) {
                        if ((ii - lo) < (hi - lo)/2) {
                            block_diag_svd(dd, ff, lo, ii + 1, ll, rr, size);
                            lo = ii + 1;
                        } else {
                            block_diag_svd(dd, ff, ii + 1, hi, ll, rr, size);
                            hi = ii + 1;
                            break;
                        }
                    }
                }

                // look for opportunities to recurse from the main diagonal
                for (int64 ii = lo; ii<hi; ii++) {
                    double f0 = (ii == lo) ? 0 : fabs(ff[ii - 1]);
                    double f1 = (ii == hi - 1) ? 0 : fabs(ff[ii]);
                    if (fabs(dd[ii]) < eps*(f0 + f1)) {
                        split_diagonal(dd, ff, lo, hi, ll, rr, size);
                        if ((ii - lo) < (hi - lo)/2) {
                            block_diag_svd(dd, ff, lo, ii, ll, rr, size);
                            lo = ii; hi = hi - 1;
                        } else {
                            block_diag_svd(dd, ff, ii, hi - 1, ll, rr, size);
                            hi = ii;
                            break;
                        }
                    }
                }

                if (hi - lo > 2) golub_kahan(dd, ff, lo, hi, ll, rr, size);
            }
            
            if (hi - lo == 2) upper_2x2_svd(dd, ff, lo, ll, rr, size);
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
            double* ll, double* rr, int64 size
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

            block_diag_svd(dd, ff, 0, size, ll, rr, size);
            
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

    template<class type>
    void svdecomp(
        type* A, int64 rows, int64 cols,
        type* L, double* D, type* R,
        double* U=0, double* V=0,
        double* F=0, int64* T=0
    ) {
        using namespace internal;
        matrix<double> utmp, vtmp;
        vector<double> dtmp, ftmp;
        vector<int64> ttmp;

        const int64 size = min(rows, cols);
        double* uu = (L && !U) ? (utmp.resize(size, size), utmp.data()) : U;
        double* vv = (R && !V) ? (vtmp.resize(size, size), vtmp.data()) : V;
        double* dd = D ? D : (dtmp.resize(size), dtmp.data());
        double* ff = F ? F : (ftmp.resize(size), ftmp.data());
        int64*  tt = T ? T : (ttmp.resize(size), ttmp.data());

        if (L) {
            for (int64 ii = 0; ii<size; ii++) {
                for (int64 jj = 0; jj<size; jj++) {
                    uu[ii*size + jj] = (ii == jj);
                }
            }
        }
        if (R) {
            for (int64 ii = 0; ii<size; ii++) {
                for (int64 jj = 0; jj<size; jj++) {
                    vv[ii*size + jj] = (ii == jj);
                }
            }
        }

        if (rows >= cols) {
            // build it upper diagonal
            for (int64 ij = 0; ij<size; ij++) {
                dd[ij] = householder_col(A, rows, cols, ij, ij);
                ff[ij] = householder_row(A, rows, cols, ij, ij + 1);
            }
        } else {
            // build it lower diagonal
            for (int64 ij = 0; ij<size; ij++) {
                dd[ij] = householder_row(A, rows, cols, ij, ij);
                ff[ij] = householder_col(A, rows, cols, ij + 1, ij);
            }
            // givens rotate to upper diagonal
            for (int64 ij = 0; ij<size - 1; ij++) {
                givens_rotation gg(dd[ij], ff[ij]);
                dd[ij] = gg.rr; ff[ij] = 0;
                gg.rotate(ff[ij], dd[ij+1]);
                gg.rotate(L ? uu : 0, ij, size);
            }
        }

        bidiagonal_svd(tt, dd, ff, L ? uu : 0, R ? vv : 0, size);

        if (L) {
            for (int64 ii = 0; ii<size; ii++) {
                for (int64 jj = 0; jj<size; jj++) {
                    // this is intentionally transposed
                    L[ii*size + jj] = uu[jj*size + ii];
                }
            }
            for (int64 ii = size; ii<rows; ii++) {
                for (int64 jj = 0; jj<size; jj++) {
                    L[ii*size + jj] = 0;
                }
            }
            if (rows >= cols) {
                for (int64 ij = size-1; ij>=0; ij--) {
                    householder_col(
                        A, rows, cols, 
                        L, rows, size,
                        ij, ij, 0, true
                    );
                }
            } else {
                for (int64 ij = size-1; ij>=0; ij--) {
                    householder_col(
                        A, rows, cols,
                        L, rows, size,
                        ij + 1, ij, 0, true
                    );
                }
            }
        }

        if (R) {
            for (int64 ii = 0; ii<size; ii++) {
                for (int64 jj = 0; jj<size; jj++) {
                    R[ii*cols + jj] = vv[ii*size + jj];
                }
                for (int64 jj = size; jj<cols; jj++) {
                    R[ii*cols + jj] = 0;
                }
            }
            if (rows >= cols) {
                for (int64 ij = size-1; ij>=0; ij--) {
                    householder_row(
                        A, rows, cols,
                        R, size, cols,
                        ij, ij + 1, 0, true
                    );
                }
            } else {
                for (int64 ij = size-1; ij>=0; ij--) {
                    householder_row(
                        A, rows, cols,
                        R, size, cols,
                        ij, ij, 0, true
                    );
                }
            }
        }
    }
        
    template<class type>
    void svdecomp(
        const matrix<type>& A, matrix<type>* L,
        vector<double>* D, matrix<type>* R
    ) {
        const int64 size = min(A.rows(), A.cols());
        if (L) L->resize(A.rows(), size);
        if (D) D->resize(size);
        if (R) R->resize(size, A.cols());

        matrix<type> copy = A;
        svdecomp(
            copy.data(), A.rows(), A.cols(),
            L ? L->data() : 0,
            D ? D->data() : 0,
            R ? R->data() : 0
        );
    }

}

#endif // XM_SVDECOMP_H_

