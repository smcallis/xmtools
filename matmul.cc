#include "xmtools.h"

namespace xm {

    namespace internal {

        template<class atype, class btype, class ctype>
        void standard_matrix_mul(
            atype* aa, const btype* bb, const ctype* cc,
            int64 rows, int64 cols, int64 size
        ) {
            for (int64 ii = 0; ii<rows; ii++) {
                for (int64 jj = 0; jj<cols; jj++) {
                    atype sum = 0;
                    for (int64 kk = 0; kk<size; kk++) {
                        sum += bb[ii*size + kk] * cc[kk*cols + jj];
                    }
                    aa[ii*cols + jj] = sum;
                }
            }
        }
    }


    static inline void dump(const char* str, const matrix<int64>& A) {
        printf("%s\n", str);
        for (int64 ii = 0; ii<A.rows(); ii++) {
            for (int64 jj = 0; jj<A.cols(); jj++) {
                printf("\t%lld", A(ii, jj));
            }
            printf("\n");
        }
    }

}

int main() {
    using namespace xm;

    const int64 rows = 9000;
    const int64 cols = 800;
    const int64 size = 300;
    matrix<int64> BB(rows, size);
    matrix<int64> CC(size, cols);

    prng random(1);

    for (int64 ii = 0; ii<rows; ii++) {
        for (int64 jj = 0; jj<size; jj++) {
            BB(ii, jj) = (int64)(random.uint64() % 10);
        }
    }

    for (int64 ii = 0; ii<size; ii++) {
        for (int64 jj = 0; jj<cols; jj++) {
            CC(ii, jj) = (int64)(random.uint64() % 10);
        }
    }

    int64 hack = BB(0, 0);

    const int64 its = 10;
    matrix<int64> A0(rows, cols);
    if (0) {
        double before0 = stopwatch();
        int64 sum0 = 0;
        for (int64 tt = 0; tt<its; tt++) {
            internal::standard_matrix_mul(
                A0.data(), BB.data(), CC.data(),
                rows, cols, size
            );
            sum0 += A0(0, 0);
            BB(0, 0) += 1;
        }
        double after0 = stopwatch();
        fprintf(stderr, "standard: %lf, %lld\n", after0 - before0, sum0);
    }

    //dump("A0", A0);

    BB(0, 0) = hack;

    matrix<int64> A1(rows, cols);
    double before1 = stopwatch();
    int64 sum1 = 0;
    for (int64 tt = 0; tt<its; tt++) {
        internal::recursive_matrix_mul(
            A1.data(), BB.data(), CC.data(),
            rows, cols, size, true,
            0, rows, 0, cols, 0, size
        );
        sum1 += A1(0, 0);
        BB(0, 0) += 1;
    }
    double after1 = stopwatch();
    fprintf(stderr, "recursive: %lf, %lld\n", after1 - before1, sum1);

    //dump("A1", A1);

    //dump("A1 - A0", A1 - A0);




    return 0;

}

