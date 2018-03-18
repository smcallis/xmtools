#ifndef XM_CHOLESKY_H_
#define XM_CHOLESKY_H_ 1

namespace xm {
    
    //
    // Decompose A = L * L^t with the Cholesky algorithm
    //
    static inline void cholesky(double* L, const double* A, int64 dim) {
        memset(L, 0, dim*dim*sizeof(double));
        for (int64 ii = 0; ii<dim; ii++) {
            for (int64 jj = 0; jj<=ii; jj++) {
                double sum = 0;
                for (int64 kk = 0; kk<jj; kk++) {
                    sum += L[ii*dim + kk]*L[jj*dim + kk];
                }
                double inv = 1/L[jj*dim +jj];
                L[ii*dim + jj] = (
                    (ii == jj) ? sqrt(A[ii*dim + ii] - sum) :
                    (A[ii*dim + jj] - sum) * inv
                );
            }
        }
    }

    // Modify a cholesky composition.  It's either "update" or "downdate",
    // depending on the sign.  (+1 = update, -1 = downdate).  Both arguments
    // are modified.
    static inline void cholupdate(
        double* L, int sign, double* x, int64 len
    ) {
        check(sign == +1 || sign == -1, "sanity");
        for (int64 jj = 0; jj<len; jj++) {
            double rr = sqrt(
                sqr(L[jj*len + jj]) + sign*sqr(x[jj])
            );
            double cc = rr/L[jj*len + jj];
            double ss = x[jj]/L[jj*len + jj];
            L[jj*len + jj] = rr;
            for (int64 ii = jj+1; ii<len; ii++) {
                L[ii*len + jj] = (L[ii*len + jj] + sign*ss*x[ii])/cc;
            }
            for (int64 ii = jj+1; ii<len; ii++) {
                x[ii] = cc*x[ii] - ss*L[ii*len + jj];
            }
        }
    }

    // Scale L so the largest diagonal is 1.0
    static inline void cholunitize(double* L, int64 dim) {
        double biggest = 0;
        for (int64 ii = 0; ii<dim; ii++) {
            double norm = 0;
            for (int64 jj = 0; jj<=ii; jj++) {
                norm += sqr(L[ii*dim + jj]);
            }
            biggest = max(norm, biggest);
        }
        double scale = 1/sqrt(biggest);
        for (int64 ii = 0; ii<dim; ii++) {
            for (int64 jj = 0; jj<=ii; jj++) {
                L[ii*dim + jj] *= scale;
            }
        }
    }

    // XXX: templatize this to work with fixed sized matrices
    static inline matrix<double> cholesky(const matrix<double>& A) {
        check(A.rows() == A.cols(), "must be a square matrix");
        matrix<double> L(A.rows(), A.cols());
        cholesky(L.data(), A.data(), A.rows());
        return L;
    }

    // XXX: templatize this to work with fixed sized matrices
    static inline matrix<double> cholupdate(
        matrix<double> L, int sign, vector<double> x
    ) {
        check(L.rows() == L.cols(), "must be a square matrix");
        check(L.rows() == x.size(), "must be a matching size");
        cholupdate(L.data(), sign, x.data(), x.size());
        return L;
    }

}

#endif // XM_CHOLESKY_H_

