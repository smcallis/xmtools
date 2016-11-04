#include "xmtools.h"

namespace xm {


}

static inline void dump(const char* name, const xm::mat<xm::cdouble>& matrix) {
    fprintf(stderr, "%s:\n", name);
    for (int64 ii = 0; ii<matrix.rows(); ii++) {
        printf("\t[");
        for (int64 jj = 0; jj<matrix.cols(); jj++) {
            printf("\t%+.3lf%+.3lfj,", matrix(ii, jj).re, matrix(ii, jj).im);
        }
        printf("],\n");
    }
}

static inline void dump(const char* name, const xm::mat<double>& matrix) {
    fprintf(stderr, "%s:\n", name);
    for (int64 ii = 0; ii<matrix.rows(); ii++) {
        printf("\t[");
        for (int64 jj = 0; jj<matrix.cols(); jj++) {
            printf("\t%+.3lf,", matrix(ii, jj));
        }
        printf("],\n");
    }
}

template<class type>
static inline xm::mat<type> matmul(
    xm::mat<type> aa, xm::mat<type> bb
) {
    xm::check(aa.cols() == bb.rows(), "compatible sizes");
    int64 size = aa.cols();
    xm::mat<type> cc(aa.rows(), bb.cols());
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

int main() {
    using namespace xm;
    using namespace xm::internal;

    prng random(0);
    const int64 rows = 8;
    const int64 cols = 8;
    const int64 size = min(rows, cols);

    mat<cdouble> A(rows, cols), L, R;
    vec<double> S(size);

    for (int64 ii = 0; ii<rows; ii++) {
        for (int64 jj = 0; jj<cols; jj++) {
            A(ii, jj).re = (int64)(random.uint64()%18) - 9;
            A(ii, jj).im = (int64)(random.uint64()%18) - 9;
        }
    }

    svdecomp(A, &L, &S, &R);
    mat<cdouble> C(size, size);
    for (int64 ii = 0; ii<size; ii++) {
        for (int64 jj = 0;jj<size; jj++) {
            C(ii, jj) = (ii == jj) ? S[ii] : 0;
        }
    }

    dump("A", A);
    dump("L", L);
    dump("L'*L", matmul(herm(L), L));
    dump("C", C);
    dump("R", R);
    dump("R*R'", matmul(R, herm(R)));
    dump("L*C*R", matmul(L, matmul(C, R)));
    dump("L*S*R - A", matmul(L, matmul(C, R)) - A);

    return 0;
}

    /*
    const int64 size = 9;

    mat<double> L = ident<double>(size, size);
    mat<double> C(size, size);
    mat<double> S(size, size);
    mat<double> R = ident<double>(size, size);

    vec<double> d(size);
    vec<double> f(size - 1);
    vec<int64> t(size);
    for (int64 ii = 0; ii<size; ii++) {
        C(ii, ii) = d[ii] = (int64)(random.uint64()%1800) - 900;
    }
    for (int64 ii = 0; ii<size - 1; ii++) {
        C(ii, ii+1) = f[ii] = (int64)(random.uint64()%1800) - 900;
    }

    dump("C", C);

    bidiagonal_svd(t.data(), d.data(), f.data(), L.data(), R.data(), size);

    for (int64 ii = 0; ii<size; ii++) {
        S(ii, ii) = d[ii];
    }

    dump("L", L);
    dump("R", R);
    dump("L*S*R", matmul(herm(L), matmul(S, R)));
    dump("L*S*R - C", matmul(herm(L), matmul(S, R)) - C);
    */

    //{{{
namespace xm {
    template<class type>
    void svdecomp_old(
        const mat<type>& A, mat<type>* L,
        vec<double>* S, mat<type>* R
    ) {
        using namespace internal;
        int64 size = min(A.rows(), A.cols());
        check(size != 0, "can't be zero size"); // XXX: should allow this...

        mat<type> C = A;
        mat<double> U, V;
        double* u = L ? (U = ident<double>(size, size), U.data()) : 0;
        double* v = R ? (V = ident<double>(size, size), V.data()) : 0;
        vec<double> d(size), f(size);
        vec<int64> t(size);

        if (C.rows() >= C.cols()) {
            // build it upper diagonal
            for (int64 ij = 0; ij<size; ij++) {
                d[ij] = householder_col(C.data(), C.rows(), C.cols(), ij, ij);
                f[ij] = householder_row(C.data(), C.rows(), C.cols(), ij, ij + 1);
            }
        } else {
            // build it lower diagonal
            for (int64 ij = 0; ij<size; ij++) {
                d[ij] = householder_row(C.data(), C.rows(), C.cols(), ij, ij);
                f[ij] = householder_col(C.data(), C.rows(), C.cols(), ij + 1, ij);
            }
            // givens rotate to upper diagonal
            for (int64 ij = 0; ij<size - 1; ij++) {
                givens_rotation gg(d[ij], f[ij]);
                d[ij] = gg.rr; f[ij] = 0;
                gg.rotate(f[ij], d[ij+1]);
                gg.rotate(u, ij, size);
            }
        }
        bidiagonal_svd(t.data(), d.data(), f.data(), u, v, size);

        if (S) {
            vec<double>& T = *S;
            T.resize(size);
            for (int64 ii = 0; ii<size; ii++) T[ii] = d[ii];
        }

        if (L) {
            mat<type>& T = *L;
            T.resize(C.rows(), size, 0);
            for (int64 ii = 0; ii<size; ii++) {
                for (int64 jj = 0; jj<size; jj++) {
                    T(ii, jj) = U(jj, ii);
                }
            }
            if (C.rows() >= C.cols()) {
                for (int64 ij = size-1; ij>=0; ij--) {
                    householder_col(
                        C.data(), C.rows(), C.cols(),
                        T.data(), T.rows(), T.cols(),
                        ij, ij, 0, true
                    );
                }
            } else {
                for (int64 ij = size-1; ij>=0; ij--) {
                    householder_col(
                        C.data(), C.rows(), C.cols(),
                        T.data(), T.rows(), T.cols(),
                        ij + 1, ij, 0, true
                    );
                }
            }

        }

        if (R) {
            mat<type>& T = *R;
            T.resize(size, C.cols(), 0);
            for (int64 ii = 0; ii<size; ii++) {
                for (int64 jj = 0; jj<size; jj++) {
                    T(ii, jj) = V(ii, jj);
                }
            }
            if (C.rows() >= C.cols()) {
                for (int64 ij = size-1; ij>=0; ij--) {
                    householder_row(
                        C.data(), C.rows(), C.cols(),
                        T.data(), T.rows(), T.cols(),
                        ij, ij + 1, 0, true
                    );
                }
            } else {
                for (int64 ij = size-1; ij>=0; ij--) {
                    householder_row(
                        C.data(), C.rows(), C.cols(),
                        T.data(), T.rows(), T.cols(),
                        ij, ij, 0, true
                    );
                }
            }
        }
    }
}
    //}}}
