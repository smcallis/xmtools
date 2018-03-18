
    /*{{{ check() old version, exits instead of throws
    __attribute__ ((format (printf, 2, 3))) 
    static inline void check(bool cond, const char* format, ...) {
        if (!cond) {
            // Print the check(...) message
            fprintf(stderr, "Check failed: ");
            va_list args;
            va_start(args, format);
            vfprintf(stderr, format, args);
            va_end(args);
            fprintf(stderr, "\nStack Trace:\n");

            char command[64];
            sprintf(command, "/usr/bin/pstack %d", (int)getpid());
            if (system(command) != 0) fprintf(stderr, "failed to run pstack\n");

            exit(EXIT_FAILURE);
        }
    }
    }}}*/
    //{{{ sum funcs 
    
    template<class type>
    type naivesum(const type* ptr, int64 len) {
        type sum = 0;
        for (int64 ii = 0; ii<len; ii++) {
            sum += ptr[ii];
        }
        return sum;
    }

    template<class type>
    type kahansum(const type* ptr, int64 len) {
        if (len == 0) return 0;
        long double total = ptr[0];
        long double carry = 0;
        for (int64 ii = 1; ii<len; ii++) {
            long double delta = ptr[ii] - carry;
            long double accum = total + delta;
            carry = (accum - total) - delta;
            total = accum;
        }
        return total;
    }

    /*
    template<class type>
    type kahansum(const cx<type>* ptr, int64 len) {
        if (len == 0) return 0;
        cx<long double> total = ptr[0];
        cx<long double> carry = 0;
        for (int64 ii = 1; ii<len; ii++) {
            cx<long double> delta = ptr[ii] - carry;
            cx<long double> accum = total + delta;
            carry = (accum - total) - delta;
            total = accum;
        }
        return total;
    }
    */

    template<class type, int64 size>
    type kahansum(const vec<type, size>& vv) {
        return kahansum(vv.data(), vv.size());
    }

    static inline void twosum(
        long double& ss, long double& rr,
        long double aa, long double bb
    ) {
        ss = aa + bb;
        long double zz = ss - aa;
        rr = (aa - (ss - zz)) + (bb - zz);
    }

    struct summation {
        summation() : capacity(0), storage(0) {}
        ~summation() { if (storage) free(storage); }

        void operator +=(long double xx) {
            int64 ii = 0;
            while (xx != 0) {
                if (ii >= capacity) resize();
                twosum(storage[ii], xx, storage[ii], xx);
                ++ii;
            }
            while (ii + 1 < capacity && storage[ii] == 0) {
                swap(storage[ii], storage[ii + 1]);
                ++ii;
            }
        }

        long double total() {
            long double sum = 0;
            for (int64 ii = capacity - 1; ii>=0; --ii) {
                sum += storage[ii];
            }
            return sum;
        }

        private:
            summation(const summation&) { check(false, "no copy"); }
            void operator =(const summation&) { check(false, "no assign"); }

            void resize() {
                int64 newsize = capacity ? 3*capacity/2 : 4;
                long double* bigger = alloc<long double>(
                    (newsize - 1)*sizeof(long double)
                );
                for (int64 ii = 0; ii<capacity; ii++) {
                    bigger[ii] = storage[ii];
                }
                for (int64 ii = capacity; ii<newsize; ii++) {
                    bigger[ii] = 0;
                }
                free(storage);
                storage = bigger;
                capacity = newsize;
            }

            int64 capacity;
            long double* storage;
    };


    template<class type>
    type exactsum(const type* ptr, int64 len) {
        summation sum;
        for (int64 ii = 0; ii<len; ii++) {
            sum += ptr[ii];
        }
        return sum.total();
    }

    template<class type>
    type exactsum(const cx<type>* ptr, int64 len) {
        summation re;
        summation im;
        for (int64 ii = 0; ii<len; ii++) {
            re += ptr[ii].re;
            im += ptr[ii].im;
        }
        return cx<type>(re.total(), im.total());
    }

    template<class type, int64 size>
    type exactsum(const vec<type, size>& vv) {
        return exactsum(vv.data(), vv.size());
    }
    //}}}
    //{{{ inner product

    /* XXX: is this how we want to do these?
    template<class atype, class btype>
    typename promotion<atype, btype>::type inner(
        const atype* aa, const btype* bb, int64 len
    ) {
        if (len == 0) return 0;
        // using Kahan sum because it doesn't cost much
        typename promotion<atype, btype>::type total = 0;
        typename promotion<atype, btype>::type carry = 0;
        for (int64 ii = 0; ii<len; ii++) {
            typename promotion<atype, btype>::type prod = aa[ii]*conj(bb[ii]);
            typename promotion<atype, btype>::type delta = prod - carry;
            typename promotion<atype, btype>::type accum = total + delta;
            carry = (accum - total) - delta;
            total = accum;
        }
        return total;
    }

    template<class atype, int64 asize, class btype, int64 bsize>
    typename promotion<atype, btype>::type inner(
        const array<atype, asize>& aa, const array<btype, bsize>& bb
    ) {
        check(aa.size() == bb.size(), "matching sizes");
        return innerprod(aa.data(), bb.data(), aa.size());
    }
    */

    // XXX:
    // template<class atype, class btype, class ctype>
    // mat<atype> outer(const vec<btype>& bb, const vec<ctype>& cc)


    //}}}
    //{{{ old mul operators
#if 0
    template<class atype, class btype> // matrix multiply A*b
    auto mul(const matrix<atype>& aa, const vector<btype>& bb)
        -> vector<decltype(atype() * btype())>
    {
        check(
            aa.cols() == bb.size(), "A[%lld, %lld] * b[%lld] must be compatible",
            aa.rows(), aa.cols(), bb.size()
        );
        const int64 rows = aa.rows();
        const int64 cols = aa.cols();
        vector<decltype(atype()*btype())> result(rows);
        zero(result);

        for (int64 ii = 0; ii<rows; ii++) {
            for (int64 jj = 0; jj<cols; jj++) {
                result[ii] += aa(ii, jj)*bb[jj];
            }
        }

        return result;
    }

    template<class atype, class btype> // matrix multiply A*B
    auto mul(const matrix<atype>& aa, const matrix<btype>& bb)
        -> matrix<decltype(atype() * btype())>
    {
        check(
            aa.cols() == bb.rows(), "A[%lld, %lld] * B[%lld, %lld] must be compatible",
            aa.rows(), aa.cols(), bb.rows(), bb.cols()
        );
        const int64 rows = aa.rows();
        const int64 cols = bb.cols();
        const int64 size = aa.cols();
        matrix<decltype(atype()*btype())> result(rows, cols);
        zero(result);

        for (int64 ii = 0; ii<rows; ii++) {
            for (int64 jj = 0; jj<cols; jj++) {
                for (int64 kk = 0; kk<size; kk++) {
                    result(ii, jj) += aa(ii, kk)*bb(kk, jj);
                }
            }
        }

        return result;
    }

    template<class atype, class btype> // matrix multiply herm(A)*B
    auto hmul(const matrix<atype>& aa, const matrix<btype>& bb)
        -> matrix<decltype(atype() * btype())>
    {
        check(
            aa.rows() == bb.rows(), "herm(A[%lld, %lld]) * B[%lld, %lld] must be compatible",
            aa.rows(), aa.cols(), bb.rows(), bb.cols()
        );
        const int64 rows = aa.cols();
        const int64 cols = bb.cols();
        const int64 size = bb.rows();
        matrix<decltype(atype()*btype())> result(rows, cols);
        zero(result);

        for (int64 ii = 0; ii<rows; ii++) {
            for (int64 jj = 0; jj<cols; jj++) {
                for (int64 kk = 0; kk<size; kk++) {
                    result(ii, jj) += conj(aa(kk, ii))*bb(kk, jj);
                }
            }
        }

        return result;
    }

    template<class atype, class btype> // matrix multiply A*herm(B)
    auto mulh(const matrix<atype>& aa, const matrix<btype>& bb)
        -> matrix<decltype(atype() * btype())>
    {
        check(
            aa.cols() == bb.cols(), "A[%lld, %lld] * herm(B[%lld, %lld]) must be compatible",
            aa.rows(), aa.cols(), bb.rows(), bb.cols()
        );
        const int64 rows = aa.rows();
        const int64 cols = bb.rows();
        const int64 size = aa.cols();
        matrix<decltype(atype()*btype())> result(rows, cols);
        zero(result);

        for (int64 ii = 0; ii<rows; ii++) {
            for (int64 jj = 0; jj<cols; jj++) {
                for (int64 kk = 0; kk<size; kk++) {
                    result(ii, jj) += aa(ii, kk)*conj(bb(jj, kk));
                }
            }
        }

        return result;
    }

    template<class atype, class btype> // matrix multiply herm(A)*herm(B)
    auto hmulh(const matrix<atype>& aa, const matrix<btype>& bb)
        -> matrix<decltype(atype() * btype())>
    {
        check(
            aa.rows() == bb.cols(), "herm(A[%lld, %lld]) * herm(B[%lld, %lld]) must be compatible",
            aa.rows(), aa.cols(), bb.rows(), bb.cols()
        );
        const int64 rows = aa.cols();
        const int64 cols = bb.rows();
        const int64 size = aa.rows();
        matrix<decltype(atype()*btype())> result(rows, cols);
        zero(result);

        for (int64 ii = 0; ii<rows; ii++) {
            for (int64 jj = 0; jj<cols; jj++) {
                for (int64 kk = 0; kk<size; kk++) {
                    result(ii, jj) += conj(aa(kk, ii))*conj(bb(jj, kk));
                }
            }
        }

        return result;
    }
#endif
    //}}}
//{{{ Miscellaneous Stuff
    /* These are more elegant Householder functions, but they don't work for complex...
        template<class htype, class atype>
        static void reflect_row_row(
            const htype* H, int64 Hrows, int64 Hcols,
                  atype* A, int64 Arows, int64 Acols,
            int64 row, int64 col, int64 low
        ) {
            check(Hcols == Acols, "matching cols");
            for (int64 ii = low; ii<Arows; ii++) {
                typename promotion<htype, atype>::type sum = 0;
                for (int64 jj = col; jj<Hcols; jj++) {
                    sum += A[ii*Acols + jj]*conj(H[row*Hcols + jj]);
                }
                sum *= 2;
                for (int64 jj = col; jj<Acols; jj++) {
                    A[ii*Acols + jj] -= sum*H[row*Hcols + jj];
                }
            }
        }

        template<class htype, class atype>
        static void reflect_col_col(
            const htype* H, int64 Hrows, int64 Hcols,
                  atype* A, int64 Arows, int64 Acols,
            int64 row, int64 col, int64 low
        ) {
            check(Hrows == Arows, "matching rows");
            for (int64 jj = low; jj<Acols; jj++) {
                typename promotion<htype, atype>::type sum = 0;
                for (int64 ii = row; ii<Hrows; ii++) {
                    sum += A[ii*Acols + jj]*conj(H[ii*Hcols + col]);
                }
                sum *= 2;
                for (int64 ii = row; ii<Arows; ii++) {
                    A[ii*Acols + jj] -= sum*H[ii*Hcols + col];
                }
            }
        }

        template<class type>
        static double householder_row(
            type* H, int64 rows, int64 cols,
            int64 row, int64 col
        ) {
            double rest = 0;
            for (int64 jj = col + 1; jj<cols; jj++) {
                rest = radius(rest, H[row*cols + jj]);
            }
            double magr = radius(rest, H[row*cols + col]);
            if (magr == 0) return 0;
            if (real(H[row*cols + col]) > 0) magr = -magr;
            H[row*cols + col] -= magr;
            double magh = radius(rest, H[row*cols + col]);
            double invh = 1/magh;
            for (int64 jj = col; jj<cols; jj++) {
                H[row*cols + jj] *= invh;
            }
            reflect_row_row(H, rows, cols, H, rows, cols, row, col, row + 1);
            return magr;
        }

        template<class type>
        static double householder_col(
            type* H, int64 rows, int64 cols,
            int64 row, int64 col
        ) {
            double rest = 0;
            for (int64 ii = row + 1; ii<rows; ii++) {
                rest = radius(rest, H[ii*cols + col]);
            }
            double magr = radius(rest, H[row*cols + col]);
            if (magr == 0) return 0;
            if (real(H[row*cols + col]) > 0) magr = -magr;
            H[row*cols + col] -= magr;
            double magh = radius(rest, H[row*cols + col]);
            double invh = 1/magh;
            for (int64 ii = row; ii<rows; ii++) {
                H[ii*cols + col] *= invh;
            }
            reflect_col_col(H, rows, cols, H, rows, cols, row, col, col + 1);
            return magr;
        }
    */
//}}}
    //{{{ LD Factorization
#if 0
    template<class type>
    bool ldfactor(type* A, int64 size) {
        for (int64 jj = 0; jj < size; jj++) {
            // Solve for Diagonal
            type ajj = A[jj*size + jj];
            for (int64 kk = 0; kk < jj; kk++) {
                type dkk = A[kk*size + kk];
                type ljk = A[jj*size + kk];
                ajj -= ljk*conj(ljk)*dkk;
            }
            A[jj*size + jj] = ajj;
            if (ajj == 0) return false;

            // Solve for Lowers
            type inv = 1.0/ajj;
            for (int64 ii = jj + 1; ii < size; ii++) {
                type aij = A[ii*size + jj];
                for (int64 kk = 0; kk < jj; kk++) {
                    type lik = A[ii*size + kk];
                    type ljk = A[jj*size + kk];
                    type dkk = A[kk*size + kk];
                    aij -= lik*conj(ljk)*dkk;
                }
                A[ii*size + jj] = aij*inv;
            }
        }

        return true;
    }

    template<class atype, class btype>
    void ldapply(const atype* A, int64 size, btype* B, int64 count) {
        for (int64 jj = 0; jj < size - 1; jj++) {
            for (int64 ii = jj + 1; ii<size; ii++) {
                for (int64 kk = 0; kk<count; kk++) {
                    B[ii*count + kk] -= A[ii*size + jj]*B[jj*count + kk];
                }
            }
        }

        for (int64 ij = 0; ij < size; ij++) {
            for (int64 kk = 0; kk<count; kk++) {
                B[ij*count + kk] /= A[ij*size + ij];
            }
        }

        for (int64 jj = size - 1; jj > 0; jj--) {
            for (int64 ii = 0; ii < jj; ii++) {
                for (int64 kk = 0; kk<count; kk++) {
                    B[ii*count + kk] -= conj(A[jj*size + ii])*B[jj*count + kk];
                }
            }
        }
    }

    template<class atype, class btype>
    bool ldsolve(atype* A, int64 size, btype* B, int64 count) {
        if (!ldfactor(A, size)) return false;
        ldapply(A, size, B, count);
        return true;
    }

    template<class at, int64 as, class bt, int64 br, int64 bc>
    opt<mat<typename promotion<at, bt>::type, br, bc> > ldsolve(
        const mat<at, as, as>& A, const mat<bt, br, bc>& B
    ) {
        check(A.rows() == A.cols(), "square matrix");
        check(A.rows() == B.rows(), "matching rows");
        mat<typename promotion<at, bt>::type, as, as> acopy(A);
        mat<typename promotion<at, bt>::type, br, bc> bcopy(B);

        if (!ldsolve(acopy.data(), A.rows(), bcopy.data(), B.rows())) {
            return "ldsolve requires a non-singular matrix";
        }

        return bcopy;
    }

    template<class type, int64 size>
    opt<mat<type, size, size> > ldinverse(const mat<type, size, size>& A) {
        mat<type, size, size> acopy(A);
        mat<type, size, size> result(
            mat<type, size, size>::ident(A.rows(), A.cols())
        );

        if (!ldsolve(acopy.data(), A.rows(), result.data(), A.cols())) {
            return "ldinverse required a non-singular matrix";
        }

        return result;
    }
#endif
    //}}}
    //{{{ BD Factorization
    // BD Decomposition
        //
        //  +--+--------------+      +-----------------------+
        //  |  |              |      |                       |
        //  |  +--+-----------+      +--+--------------------+
        //  |  |  |           |      |  |                    |
        //  |  |  +--+--------+      |  +--+-----------------+
        //  |  |  |  |        |      |  |  |                 |
        //  |  |  |  +--+-----+      |  |  +--+--------------+
        //  |  |  |  |  |     +      |  |  |  |              +
        //  |  |  |  |  +--+--+      |  |  |  +--+-----------+
        //  |  |  |  |  |  |  |      |  |  |  |  |           |
        //  |  |  |  |  |  +--+      +--+--+--+--+-----------+
        //  |  |  |  |  |  |  |
        //  |  |  |  |  |  |  |
        //  |  |  |  |  |  |  |
        //  |  |  |  |  |  |  |
        //  |  |  |  |  |  |  |
        //  +--+--+--+--+--+--+
        //
    //}}}
    //{{{ broken matrix operators


// XXX: Something is really wrong with all of the operator* functions....

    /*
    // matrix multiplication, specialized for complex*complex
    template<class t0, int64 r0, int64 c0, class t1, int64 r1, int64 c1>
    typename matmultype<complex<t0>, r0, c0, complex<t1>, r1, c1>::type operator *(
        const matrix<complex<t0>, r0, c0>& aa, const matrix<complex<t1>, r1, c1>& bb
    ) {
        const int64 rows = aa.rows();
        const int64 cols = bb.cols();
        const int64 size = aa.cols();
        check(bb.rows() == size, "compatible size %lld == %lld", size, bb.rows());
        typename matmultype<complex<t0>, r0, c0, complex<t1>, r1, c1>::type cc(rows, cols);
        for (int64 ii = 0; ii<rows; ii++) {
            for (int64 jj = 0; jj<cols; jj++) {
                typename arithmetic<complex<t0>, complex<t1> >::type sum = 0;
                for (int64 kk = 0; kk<size; kk++) {
                    sum += aa(ii, kk)*bb(kk, jj);
                }
                cc(ii, jj) = sum;
            }
        }
        return cc;
    }
    */

    /*
    // matrix multiplication, specialized for complex*real
    template<class t0, int64 r0, int64 c0, class t1, int64 r1, int64 c1>
    typename matmultype<complex<t0>, r0, c0, t1, r1, c1>::type operator *(
        const matrix<complex<t0>, r0, c0>& aa, const matrix<t1, r1, c1>& bb
    ) {
        const int64 rows = aa.rows();
        const int64 cols = bb.cols();
        const int64 size = aa.cols();
        check(bb.rows() == size, "compatible size %lld == %lld", size, bb.rows());
        typename matmultype<complex<t0>, r0, c0, t1, r1, c1>::type cc(rows, cols);
        for (int64 ii = 0; ii<rows; ii++) {
            for (int64 jj = 0; jj<cols; jj++) {
                typename arithmetic<complex<t0>, t1>::type sum = 0;
                for (int64 kk = 0; kk<size; kk++) {
                    sum += aa(ii, kk)*bb(kk, jj);
                }
                cc(ii, jj) = sum;
            }
        }
        return cc;
    }
    */

    /*
    // matrix multiplication, specialized for real*complex
    template<class t0, int64 r0, int64 c0, class t1, int64 r1, int64 c1>
    typename matmultype<t0, r0, c0, complex<t1>, r1, c1>::type operator *(
        const matrix<t0, r0, c0>& aa, const matrix<complex<t1>, r1, c1>& bb
    ) {
        const int64 rows = aa.rows();
        const int64 cols = bb.cols();
        const int64 size = aa.cols();
        check(bb.rows() == size, "compatible size %lld == %lld", size, bb.rows());
        typename matmultype<t0, r0, c0, complex<t1>, r1, c1>::type cc(rows, cols);
        for (int64 ii = 0; ii<rows; ii++) {
            for (int64 jj = 0; jj<cols; jj++) {
                typename arithmetic<t0, complex<t1> >::type sum = 0;
                for (int64 kk = 0; kk<size; kk++) {
                    sum += aa(ii, kk)*bb(kk, jj);
                }
                cc(ii, jj) = sum;
            }
        }
        return cc;
    }
    */

    /*
    // specialized for complex*complex
    template<class t0, class t1, int64 r1, int64 c1>
    matrix<typename arithmetic<complex<t0>, complex<t1> >::type, r1, c1> operator *(
        const complex<t0>& scalar, const matrix<complex<t1>, r1, c1>& bb
    ) {
        const int64 rows = bb.rows();
        const int64 cols = bb.cols();
        matrix<typename arithmetic<complex<t0>, complex<t1> >::type, r1, c1> cc(rows, cols);
        for (int64 ii = 0; ii<rows; ii++) {
            for (int64 jj = 0; jj<cols; jj++) {
                cc(ii, jj) = scalar*bb(ii, jj);
            }
        }
        return cc;
    }
    */

    /*
    // specialized for complex*real
    template<class t0, class t1, int64 r1, int64 c1>
    matrix<typename arithmetic<complex<t0>, t1>::type, r1, c1> operator *(
        const complex<t0>& scalar, const matrix<t1, r1, c1>& bb
    ) {
        const int64 rows = bb.rows();
        const int64 cols = bb.cols();
        matrix<typename arithmetic<complex<t0>, t1>::type, r1, c1> cc(rows, cols);
        for (int64 ii = 0; ii<rows; ii++) {
            for (int64 jj = 0; jj<cols; jj++) {
                cc(ii, jj) = scalar*bb(ii, jj);
            }
        }
        return cc;
    }
    */

    /*
    // specialized for real*complex
    template<class t0, class t1, int64 r1, int64 c1>
    matrix<typename arithmetic<t0, complex<t1> >::type, r1, c1> operator *(
        const t0& scalar, const matrix<complex<t1>, r1, c1>& bb
    ) {
        const int64 rows = bb.rows();
        const int64 cols = bb.cols();
        matrix<typename arithmetic<t0, complex<t1> >::type, r1, c1> cc(rows, cols);
        for (int64 ii = 0; ii<rows; ii++) {
            for (int64 jj = 0; jj<cols; jj++) {
                cc(ii, jj) = scalar*bb(ii, jj);
            }
        }
        return cc;
    }
    */

    /*
    // generalized without complex
    template<class t0, class t1, int64 r1, int64 c1>
    matrix<typename arithmetic<t0, t1>::type, r1, c1> operator *(
        const t0& scalar, const matrix<t1, r1, c1>& bb
    ) {
        const int64 rows = bb.rows();
        const int64 cols = bb.cols();
        matrix<typename arithmetic<t0, t1>::type, r1, c1> cc(rows, cols);
        for (int64 ii = 0; ii<rows; ii++) {
            for (int64 jj = 0; jj<cols; jj++) {
                cc(ii, jj) = scalar*bb(ii, jj);
            }
        }
        return cc;
    }
    */

    /*
    // specialized for complex*complex
    template<class t0, int64 r0, int64 c0, class t1>
    matrix<typename arithmetic<complex<t0>, complex<t1> >::type, r0, c0> operator *(
        const matrix<complex<t0>, r0, c0>& bb, const complex<t1>& scalar
    ) {
        const int64 rows = bb.rows();
        const int64 cols = bb.cols();
        matrix<typename arithmetic<complex<t0>, complex<t1> >::type, r0, c0> cc(rows, cols);
        for (int64 ii = 0; ii<rows; ii++) {
            for (int64 jj = 0; jj<cols; jj++) {
                cc(ii, jj) = bb(ii, jj)*scalar;
            }
        }
        return cc;
    }
    */

    /*
    // specialized for complex*real
    template<class t0, int64 r0, int64 c0, class t1>
    matrix<typename arithmetic<complex<t0>, t1>::type, r0, c0> operator *(
        const matrix<complex<t0>, r0, c0>& bb, const t1& scalar
    ) {
        const int64 rows = bb.rows();
        const int64 cols = bb.cols();
        matrix<typename arithmetic<complex<t0>, t1>::type, r0, c0> cc(rows, cols);
        for (int64 ii = 0; ii<rows; ii++) {
            for (int64 jj = 0; jj<cols; jj++) {
                cc(ii, jj) = bb(ii, jj)*scalar;
            }
        }
        return cc;
    }
    */

    /*
    // specialized for real*complex
    template<class t0, int64 r0, int64 c0, class t1>
    matrix<typename arithmetic<t0, complex<t1> >::type, r0, c0> operator *(
        const matrix<t0, r0, c0>& bb, const complex<t1>& scalar
    ) {
        const int64 rows = bb.rows();
        const int64 cols = bb.cols();
        matrix<typename arithmetic<t0, complex<t1> >::type, r0, c0> cc(rows, cols);
        for (int64 ii = 0; ii<rows; ii++) {
            for (int64 jj = 0; jj<cols; jj++) {
                cc(ii, jj) = bb(ii, jj)*scalar;
            }
        }
        return cc;
    }
    */

    /*
    // generalized without complex
    template<class t0, int64 r0, int64 c0, class t1>
    matrix<typename arithmetic<t0, t1>::type, r0, c0> operator *(
        const matrix<t0, r0, c0>& bb, const t1& scalar
    ) {
        const int64 rows = bb.rows();
        const int64 cols = bb.cols();
        matrix<typename arithmetic<t0, t1>::type, r0, c0> cc(rows, cols);
        for (int64 ii = 0; ii<rows; ii++) {
            for (int64 jj = 0; jj<cols; jj++) {
                cc(ii, jj) = bb(ii, jj)*scalar;
            }
        }
        return cc;
    }
    */

    
    /*
    template<class t0, int64 r0, int64 c0, class t1>
    matrix<t0, r0, c0>& operator *=(matrix<t0, r0, c0>& aa, const t1& scalar) {
        const int64 rows = aa.rows();
        const int64 cols = aa.cols();
        for (int64 ii = 0; ii<rows; ii++) {
            for (int64 jj = 0; jj<cols; jj++) {
                aa(ii, jj) *= scalar;
            }
        }
        return aa;
    }
    */
    
    /*
    template<class t0, int64 r0, int64 c0, class t1>
    matrix<t0, r0, c0>& operator /=(matrix<t0, r0, c0>& aa, const t1& scalar) {
        const int64 rows = aa.rows();
        const int64 cols = aa.cols();
        for (int64 ii = 0; ii<rows; ii++) {
            for (int64 jj = 0; jj<cols; jj++) {
                aa(ii, jj) /= scalar;
            }
        }
        return aa;
    }
    */
    //}}}
        //{{{ broken matmultype
        struct matmultype {
            typedef matrix<
                typename arithmetic<t0, t1>::type,
                // XXX: are these right?
                matmulsize<r0, c1>::size,
                matmulsize<c1, r0>::size
            > type;
        };
        //}}}
//{{{ Uncaught Exception Handler     
#if 0
namespace stack_trace_on_uncaught_exceptions {
    // There is no reason to use anything in here directly.  This block of code
    // replaces the terminate handler with our custom version that prints a stack
    // trace.  The handler is registered automatically before main is called.
    static void handler() {
        /*
        fprintf(stderr, "Termination Stack Trace:\n");
        char command[64];
        sprintf(command, "/usr/bin/pstack %d", (int)getpid());
        if (system(command) != 0) {
            fprintf(stderr, "  failed to run pstack (install gdb)\n");
        }
        */

        try {
            //std::rethrow_exception(std::current_exception());
            throw;
        } catch (const char* msg) {
            fprintf(stderr, "XXX Exception: '%s'\n", msg);
        } catch (const string& msg) {
            fprintf(stderr, "YYY Exception: '%s'\n", msg.data());
        } catch (const std::exception& ee) {
            fprintf(stderr, "ZZZ Exception: '%s'\n", ee.what());
        } catch (...) {
            fprintf(stderr, "Unknown Exception\n");
        }
    }
    static struct initializer {
        initializer() { std::set_terminate(handler); }
    } this_is_constructed_before_main_is_called;
}
#endif
//}}}
