#ifndef XM_SOLVERS_H_
#define XM_SOLVERS_H_ 1

#include "householder.h"

namespace xm {

    //
    // Solve: A X = B, Given: A[rows*cols], B[max(rows, cols)*size]
    // 
    // This function finds the least-squares or least-norm solution to the
    // system of linear equations.  Both A and B will be modified with the
    // final result in the top of B.
    //
    // If provided, P is used as scratch and must be P[min(rows, cols)] large.
    // If not provided, the routine will allocate and free temporary space.
    //
    template<class type>
    int64 gensolve(
        type* A, int64 rows, int64 cols, 
        type* B, int64 size, double tol=1e-12,
        int64* P=0
    ) {
        using namespace internal;
        int64 rank = min(rows, cols);
        vector<int64> temp;
        int64* perm = P ? P : (temp.resize(rank), temp.data());

        // zero out the extra part of B, only if rows < cols
        for (int64 ii = rows; ii<cols; ii++) {
            for (int64 jj = 0; jj<size; jj++) {
                B[ii*size + jj] = 0;
            }
        }

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
            double diag = householder_row(A, rank, cols, ij, ij);
            double offd = householder_col(A, rank, cols, ij+1, ij);
            householder_col(A, rank, cols, B, rank, size, ij+1, ij);
            // forward ellimation with the diagonal and sub-diagonal
            double scale = 1.0/diag;
            for (int64 jj = 0; jj<size; jj++) {
                B[ij*size + jj] *= scale;
                B[(ij + 1)*size + jj] -= offd*B[ij*size + jj];
            }
        }
        // do the final division for the last diagonal
        double diag = householder_row(A, rows, cols, rank - 1, rank - 1);
        double scale = 1.0/diag;
        for (int64 jj = 0; jj<size; jj++) {
            B[(rank - 1)*size + jj] *= scale;
        }

        // apply our householder rows to X
        for (int64 ij = rank - 1; ij>=0; ij--) {
            gensolve_row_col(A, rank, cols, B, cols, size, ij, ij);
        }

        // apply our permutations to X
        for (int64 ii = rank - 1; ii>=0; ii--) {
            if (perm[ii] != ii) {
                for (int64 jj = 0; jj<size; jj++) {
                    swap(B[ii*size + jj], B[perm[ii]*size + jj]);
                }
            }
        }

        return rank;
    }


}

#endif
