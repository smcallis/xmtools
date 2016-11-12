#include "xmtools.h"

namespace xm {


    namespace internal {

        template<class type>
        void upper_hessenberg(type* A, type* Q, int64 size) {
            if (Q) {
                for (int64 ii = 0; ii<size; ii++) {
                    for (int64 jj = 0; jj<size; jj++) {
                        Q[ii*size + jj] = (ii == jj);
                    }
                }
            }

            for (int64 ij = 0; ij<size - 1; ij++) {
                double pp = householder_col(A, size, size, ij + 1, ij);
                hessenberg_col_row(
                    A, size, size,
                    A, size, size,
                    ij + 1, ij
                );
                if (Q) householder_col(
                    A, size, size,
                    Q, size, size,
                    ij + 1, ij
                );
                A[(ij + 1)*size + ij] = pp;
                for (int64 ii = ij + 2; ii<size; ii++) {
                    A[ii*size + ij] = 0;
                }
            }
        }

        void jacobi_rotate(double& xx, double& yy, double cc, double ss) {
            double tt = xx;
            xx =  tt*cc + yy*ss;
            yy = -tt*ss + yy*cc;
        }

        void jacobi_rotation(
            double* A, double* Q, int64 size,
            int64 row, int64 col
        ) {
            double pp = A[(row + 1)*size + col];
            if (fabs(pp) < 1e-20) return;
            double xx = A[(row + 1)*size + col + 1];
            double yy = A[row*size + col];
            double bb = (xx - yy)/(2*pp);
            double tt = copysign(1, bb)/(fabs(bb) - hypot(bb, 1));
            double cc = 1/hypot(tt, 1);
            double ss = cc*tt;

            for (int64 jj = 0; jj<size; jj++) {
                jacobi_rotate(A[row*size + jj], A[(row + 1)*size + jj], cc, -ss);
            }
            for (int64 ii = 0; ii<size; ii++) {
                jacobi_rotate(A[ii*size + row], A[ii*size + row + 1], cc, -ss);
            }
            if (Q) {
                for (int64 jj = 0; jj<size; jj++) {
                    jacobi_rotate(Q[row*size + jj], Q[(row + 1)*size + jj], cc, -ss);
                }
            }
        }

        void jacobi_givens(
            double* A, double* Q, int64 size,
            int64 row, int64 col
        ) {
            double xx = A[row*size + col];
            double yy = A[(row + 1)*size + col];
            double rr = hypot(xx, yy);
            double cc = xx/rr;
            double ss = yy/rr;
            printf("xx: %lf, yy: %lf, %lf\n", xx, yy, rr);

            for (int64 jj = 0; jj<size; jj++) {
                jacobi_rotate(A[row*size + jj], A[(row + 1)*size + jj], cc, ss);
            }
            for (int64 ii = 0; ii<size; ii++) {
                jacobi_rotate(A[ii*size + row], A[ii*size + row + 1], cc, ss);
            }
            if (Q) {
                for (int64 jj = 0; jj<size; jj++) {
                    jacobi_rotate(Q[row*size + jj], Q[(row + 1)*size + jj], cc, ss);
                }
            }
        }
    }



    static inline void dump(const char* str, const matrix<cdouble>& A) {
        printf("%s\n", str);
        for (int64 ii = 0; ii<A.rows(); ii++) {
            for (int64 jj = 0; jj<A.cols(); jj++) {
                printf("\t%+.3lf%+.3lf", A(ii, jj).re, A(ii, jj).im);
            }
            printf("\n");
        }
    }

    static inline void dump(const char* str, const matrix<double>& A) {
        printf("%s\n", str);
        for (int64 ii = 0; ii<A.rows(); ii++) {
            for (int64 jj = 0; jj<A.cols(); jj++) {
                printf("\t%+.3lf", A(ii, jj));
            }
            printf("\n");
        }
    }
    //{{{ matmul
    template<class t0, int64 r0, int64 c0, class t1, int64 r1, int64 c1>
    typename internal::matmultype<t0, r0, c0, t1, r1, c1>::type matmul(
        const matrix<t0, r0, c0>& aa, const matrix<t1, r1, c1>& bb
    ) {
        const int64 rows = aa.rows();
        const int64 cols = bb.cols();
        const int64 size = aa.cols();
        check(bb.rows() == size, "compatible size %lld == %lld", size, bb.rows());
        typename internal::matmultype<t0, r0, c0, t1, r1, c1>::type cc(rows, cols);
        for (int64 ii = 0; ii<rows; ii++) {
            for (int64 jj = 0; jj<cols; jj++) {
                typename arithmetic<t0, t1>::type sum = 0;
                for (int64 kk = 0; kk<size; kk++) {
                    sum += aa(ii, kk)*bb(kk, jj);
                }
                cc(ii, jj) = sum;
            }
        }
        return cc;
    }
    //}}}
}


int main() {
    using namespace xm;

    prng random(1);

    const int64 size = 9;
    matrix<double> A(size, size);
    matrix<double> Q(size, size);

    for (int64 ii = 0; ii<size; ii++) {
        //for (int64 jj = 0; jj<size; jj++) {
        for (int64 jj = 0; jj<ii; jj++) {
            //A(ii, jj).re = ((int64)(random.uint64()%19)) - 9;
            //A(ii, jj).im = ((int64)(random.uint64()%19)) - 9;
            A(ii, jj) = ((int64)(random.uint64()%19)) - 9;
            A(jj, ii) = conj(A(ii, jj));
        }
        A(ii, ii) = ((int64)(random.uint64()%19)) - 9;
    }

    dump("A before:", A);

    internal::upper_hessenberg(A.data(), Q.data(), size);
    dump("A:", A);
    //dump("Hess: Q'*A*Q:", matmul(herm(Q), matmul(A, Q)));
    getchar();
    for (;;) {
        internal::jacobi_rotation(A.data(), Q.data(), size, 0, 0);
        //internal::jacobi_givens(A.data(), Q.data(), size, 0, 0);
        dump("A:", A);
        //dump("Jacobi: Q'*A*Q:", matmul(herm(Q), matmul(A, Q)));
        getchar();
        for (int64 ij = 0; ij < size - 2; ij++) {
            internal::jacobi_givens(A.data(), Q.data(), size, ij + 1, ij);
            dump("A:", A);
            //dump("Givens: Q'*A*Q:", matmul(herm(Q), matmul(A, Q)));
            getchar();
        }
        dump("Givens: Q'*A*Q:", matmul(herm(Q), matmul(A, Q)));
        getchar();
    }

    dump("A after:", A);
    dump("Q after:", Q);
    //dump("Q*A*Q':", matmul(Q, matmul(A, herm(Q))));
    dump("Q'*A*Q:", matmul(herm(Q), matmul(A, Q)));
    //dump("Q*A:", matmul(Q, A));


    return 0;
}
