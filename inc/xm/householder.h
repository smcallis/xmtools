#ifndef XM_HOUSEHOLDER_H_
#define XM_HOUSEHOLDER_H_ 1

namespace xm {

    //{{{ internal
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
        template<class type>
        void householder_col(
            const type* A, int64 arows, int64 acols,
                  type* B, int64 brows, int64 bcols,
            int64 row, int64 col, int64 low=0, bool recover=false
        ) {
            check(arows == brows, "matching rows");
            type pivot = A[row*acols + col];
            if (pivot == 0) return;
            if (recover) pivot = conj(pivot);
            type scale = 1.0/pivot;
            for (int64 jj = low; jj<bcols; jj++) {
                type sum = 0;
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
        template<class type>
        void householder_row(
            const type* A, int64 arows, int64 acols,
                  type* B, int64 brows, int64 bcols,
            int64 row, int64 col, int64 low=0, bool recover=false
        ) {
            (void)arows; // not needed, but it looks nicer
            check(acols == bcols, "matching cols");
            type pivot = A[row*acols + col];
            if (pivot == 0) return;
            if (recover) pivot = conj(pivot);
            type scale = 1.0/pivot;
            for (int64 ii = low; ii<brows; ii++) {
                type sum = 0;
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

        // Special Householder for the gensolve() function
        template<class type>
        void gensolve_row_col(
            const type* A, int64 arows, int64 acols,
                  type* R, int64 rrows, int64 rcols,
            int64 row, int64 col, int64 low=0
        ) {
            (void)arows; // not needed, but it looks nicer
            check(acols == rrows, "matching sizes");
            type pivot = A[row*acols + col];
            if (pivot == 0) return;
            type scale = 1.0/pivot;
            for (int64 jj = low; jj<rcols; jj++) {
                type sum = 0;
                for (int64 kk = col; kk<acols; kk++) {
                    sum += R[kk*rcols + jj] * A[row*acols + kk];
                }
                sum *= scale;
                for (int64 kk = col; kk<acols; kk++) {
                    R[kk*rcols + jj] += sum * conj(A[row*acols + kk]);
                }
            }
        }

        // Special Householder for upper_hessenber
        template<class type>
        void hessenberg_col_row(
            const type* A, int64 arows, int64 acols,
                  type* L, int64 lrows, int64 lcols,
            int64 row, int64 col, int64 low=0
        ) {
            check(arows == lcols, "matching sizes");
            type pivot = A[row*acols + col];
            if (pivot == 0) return;
            pivot = conj(pivot);
            type scale = 1.0/pivot;
            for (int64 ii = low; ii<lrows; ii++) {
                type sum = 0;
                for (int64 kk = row; kk<arows; kk++) {
                    sum += L[ii*lcols + kk] * A[kk*acols + col];
                }
                sum *= scale;
                for (int64 kk = row; kk<arows; kk++) {
                    L[ii*lcols + kk] += sum * conj(A[kk*acols + col]);
                }
            }
        }

    }
    //}}}

}

#endif // XM_HOUSEHOLDER_H_

