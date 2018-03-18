#ifndef XM_MATRIX_H_
#define XM_MATRIX_H_ 1

#include "basics.h"
#include "promote.h"
#include "vector.h"

namespace xm {

    //{{{ matrix fixed size

    // This is the fixed sized implementation.  The size of the matrix is
    // determined by the template arguments, and no dynamic allocation occurs
    // at runtime.  The dynamically sized specialization is below.
    template<class type, int64 rr=-1, int64 cc=-1>
    struct matrix {
        ~matrix();
        matrix();
        matrix(int64 rows, int64 cols);
        // XXX: need an init version

        template<class tt>
        matrix(const matrix<tt, rr, cc>& other);
        template<class tt>
        matrix(const matrix<tt, -1, -1>& other);
        matrix(const matrix<type, rr, cc>& other);

        template<class tt>
        matrix<type, rr, cc>& operator =(const matrix<tt, rr, cc>& other);
        template<class tt>
        matrix<type, rr, cc>& operator =(const matrix<tt, -1, -1>& other);
        matrix<type, rr, cc>& operator =(const matrix<type, rr, cc>& other);

        type& operator [](int64 index);
        const type& operator [](int64 index) const;

        type& operator ()(int64 row, int64 col);
        const type& operator ()(int64 row, int64 col) const;

        int64 rows() const;
        int64 cols() const;
        int64 size() const;

        type* data();
        const type* data() const;

        private:
            template<class t0, int64 r0, int64 c0> friend void swap(
                matrix<t0, r0, c0>& flip, matrix<t0, r0, c0>& flop
            );
            template<class anyt, int64 anyr, int64 anyc> friend struct matrix;

            type storage[rr*cc];
    };

    template<class type, int64 rr, int64 cc>
    matrix<type, rr, cc>::~matrix() {}

    template<class type, int64 rr, int64 cc>
    matrix<type, rr, cc>::matrix() {}

    template<class type, int64 rr, int64 cc>
    matrix<type, rr, cc>::matrix(int64 rows, int64 cols) {
        check(rows == rr, "matching rows %lld for fixed size %lld", rows, rr);
        check(cols == cc, "matching cols %lld for fixed size %lld", cols, cc);
    }

    template<class type, int64 rr, int64 cc>
    template<class tt>
    matrix<type, rr, cc>::matrix(const matrix<tt, rr, cc>& other) {
        for (int64 ii = 0; ii<rr*cc; ii++) {
            storage[ii] = other.storage[ii];
        }
    }

    template<class type, int64 rr, int64 cc>
    template<class tt>
    matrix<type, rr, cc>::matrix(const matrix<tt, -1, -1>& other) {
        const int64 rows = other.rows();
        const int64 cols = other.cols();
        check(rows == rr, "matching rows %lld for fixed size %lld", rows, rr);
        check(cols == cc, "matching cols %lld for fixed size %lld", cols, cc);
        for (int64 ii = 0; ii<rr*cc; ii++) {
            storage[ii] = other[ii];
        }
    }

    template<class type, int64 rr, int64 cc>
    matrix<type, rr, cc>::matrix(const matrix<type, rr, cc>& other) {
        for (int64 ii = 0; ii<rr*cc; ii++) {
            storage[ii] = other.storage[ii];
        }
    }

    template<class type, int64 rr, int64 cc>
    template<class tt>
    matrix<type, rr, cc>& matrix<type, rr, cc>::operator =(
        const matrix<tt, rr, cc>& other
    ) {
        for (int64 ii = 0; ii<rr*cc; ii++) {
            storage[ii] = other.storage[ii];
        }
        return *this;
    }

    template<class type, int64 rr, int64 cc>
    template<class tt>
    matrix<type, rr, cc>& matrix<type, rr, cc>::operator =(
        const matrix<tt, -1, -1>& other
    ) {
        check(other.rows() == rr, "matching rows");
        check(other.cols() == cc, "matching cols");
        for (int64 ii = 0; ii<rr*cc; ii++) {
            storage[ii] = other[ii];
        }
        return *this;
    }

    template<class type, int64 rr, int64 cc>
    matrix<type, rr, cc>& matrix<type, rr, cc>::operator =(
        const matrix<type, rr, cc>& other
    ) {
        for (int64 ii = 0; ii<rr*cc; ii++) {
            storage[ii] = other.storage[ii];
        }
        return *this;
    }

    template<class type, int64 rr, int64 cc>
    type& matrix<type, rr, cc>::operator [](int64 index) {
        return storage[index];
    }

    template<class type, int64 rr, int64 cc>
    const type& matrix<type, rr, cc>::operator [](int64 index) const {
        return storage[index];
    }

    template<class type, int64 rr, int64 cc>
    type& matrix<type, rr, cc>::operator ()(int64 row, int64 col) {
        return storage[row*cc + col];
    }

    template<class type, int64 rr, int64 cc>
    const type& matrix<type, rr, cc>::operator ()(int64 row, int64 col) const {
        return storage[row*cc + col];
    }

    template<class type, int64 rr, int64 cc>
    int64 matrix<type, rr, cc>::rows() const { return rr; }

    template<class type, int64 rr, int64 cc>
    int64 matrix<type, rr, cc>::cols() const { return cc; }

    template<class type, int64 rr, int64 cc>
    int64 matrix<type, rr, cc>::size() const { return rr*cc; }

    template<class type, int64 rr, int64 cc>
    type* matrix<type, rr, cc>::data() { return storage; }

    template<class type, int64 rr, int64 cc>
    const type* matrix<type, rr, cc>::data() const { return data; }

    template<class t0, int64 r0, int64 c0>
    void swap(matrix<t0, r0, c0>& flip, matrix<t0, r0, c0>& flop) {
        for (int64 ii = 0; ii<r0; ii++) {
            for (int64 jj = 0; jj<c0; jj++) {
                swap(flip.data[ii*c0 + jj], flop.data[ii*c0 + jj]);
            }
        }
    }
    //}}}
    //{{{ matrix dynamic size

    // Dynamically sized matrix using the default arguments from the template
    // above.  The interface is nearly identical, and operations should mix and
    // match with either.
    template<class type>
    struct matrix<type, -1, -1> {
        ~matrix();
        matrix();

        matrix(int64 rr, int64 cc);
        matrix(int64 rr, int64 cc, const type& init);

        template<class tt, int64 rr, int64 cc>
        matrix(const matrix<tt, rr, cc>& other);
        matrix(const matrix<type, -1, -1>& other);

        template<class tt, int64 rr, int64 cc>
        matrix<type, -1, -1>& operator =(const matrix<tt, rr, cc>& other);
        matrix<type, -1, -1>& operator =(const matrix<type, -1, -1>& other);

        type& operator [](int64 index);
        const type& operator [](int64 index) const;

        type& operator ()(int64 row, int64 col);
        const type& operator ()(int64 row, int64 col) const;

        int64 rows() const;
        int64 cols() const;
        int64 size() const;

        void resize(int64 rr, int64 cc);
        void resize(int64 rr, int64 cc, const type& init);
        void clear();

        type* data();
        const type* data() const;

        private:
            void realloc(int64 rr, int64 cc);

            template<class tt, int64 rr, int64 cc>
            void assign(const matrix<tt, rr, cc>& other);

            template<class tt> friend void swap(
                matrix<tt, -1, -1>& flip, matrix<tt, -1, -1>& flop
            );
            template<class anyt, int64 anyr, int64 anyc> friend struct matrix;

            struct implementation {
                int64 rows, cols;
                type bucket[1];
            } *storage;
    };

    template<class type>
    matrix<type, -1, -1>::~matrix() { clear(); }

    template<class type>
    matrix<type, -1, -1>::matrix() : storage(0) {}

    template<class type>
    matrix<type, -1, -1>::matrix(int64 rr, int64 cc) : storage(0) {
        resize(rr, cc);
    }

    template<class type>
    matrix<type, -1, -1>::matrix(int64 rr, int64 cc, const type& init) : storage(0) {
        resize(rr, cc, init);
    }

    template<class type>
    template<class tt, int64 rr, int64 cc>
    matrix<type, -1, -1>::matrix(const matrix<tt, rr, cc>& other) : storage(0) {
        assign(other);
    }

    template<class type>
    matrix<type, -1, -1>::matrix(const matrix<type, -1, -1>& other) : storage(0) {
        assign(other);
    }

    template<class type>
    template<class tt, int64 rr, int64 cc>
    matrix<type, -1, -1>& matrix<type, -1, -1>::operator =(
        const matrix<tt, rr, cc>& other
    ) {
        assign(other);
        return *this;
    }

    template<class type>
    matrix<type, -1, -1>& matrix<type, -1, -1>::operator =(
            const matrix<type, -1, -1>& other
    ) {
        assign(other);
        return *this;
    }

    template<class type>
    int64 matrix<type, -1, -1>::rows() const {
        return storage ? storage->rows : 0;
    }

    template<class type>
    int64 matrix<type, -1, -1>::cols() const {
        return storage ? storage->cols : 0;
    }

    template<class type>
    int64 matrix<type, -1, -1>::size() const {
        return storage ? storage->rows*storage->cols : 0;
    }

    template<class type>
    type& matrix<type, -1, -1>::operator [](int64 index) {
        return storage->bucket[index];
    }

    template<class type>
    const type& matrix<type, -1, -1>::operator [](int64 index) const {
        return storage->bucket[index];
    }

    template<class type>
    type& matrix<type, -1, -1>::operator ()(int64 row, int64 col) {
        return storage->bucket[row*storage->cols + col];
    }

    template<class type>
    const type& matrix<type, -1, -1>::operator ()(int64 row, int64 col) const {
        return storage->bucket[row*storage->cols + col];
    }

    template<class type>
    void matrix<type, -1, -1>::resize(int64 rows, int64 cols) {
        realloc(rows, cols);
        for (int64 ii = 0; ii<rows*cols; ii++) {
            new(storage->bucket + ii) type;
        }
    }

    template<class type>
    void matrix<type, -1, -1>::resize(int64 rows, int64 cols, const type& init) {
        realloc(rows, cols);
        for (int64 ii = 0; ii<rows*cols; ii++) {
            new(storage->bucket + ii) type(init);
        }
    }

    template<class type>
    void matrix<type, -1, -1>::clear() {
        if (storage) {
            const int64 rows = storage->rows;
            const int64 cols = storage->cols;
            for (int64 ii = 0; ii<rows*cols; ii++) {
                storage->bucket[ii].~type();
            }
        }
        free(storage);
        storage = 0;
    }

    template<class type>
    type* matrix<type, -1, -1>::data() {
        return storage ? storage->bucket : 0;
    }

    template<class type>
    const type* matrix<type, -1, -1>::data() const {
        return storage ? storage->bucket : 0;
    }

    template<class type>
    void matrix<type, -1, -1>::realloc(int64 rows, int64 cols) {
        check(rows >= 0, "must have positive number of rows");
        check(cols >= 0, "must have positive number of cols");
        // In both branches of this if, we leave the objects unconstructed.
        // The calling function will construct objects into the memory.
        if (storage && storage->rows*storage->cols == rows*cols) {
            // re-use the same memory, but destruct the objects
            for (int64 ii = 0; ii<rows*cols; ii++) {
                storage->bucket[ii].~type();
            }
        } else {
            clear(); // create new unconstructed memory
            storage = alloc<implementation>((rows*cols - 1)*sizeof(type));
        }
        storage->rows = rows;
        storage->cols = cols;
    }

    template<class type>
    template<class tt, int64 rr, int64 cc>
    void matrix<type, -1, -1>::assign(const matrix<tt, rr, cc>& other) {
        if ((void*)this == (void*)&other) return;
        const int64 rows = other.rows();
        const int64 cols = other.cols();
        realloc(rows, cols);
        for (int64 ii = 0; ii<rows; ii++) {
            for (int64 jj = 0; jj<cols; jj++) {
                new(storage->bucket + ii*cols + jj) type(other(ii, jj));
            }
        }
    }

    template<class tt>
    void swap(
        matrix<tt, -1, -1>& flip, matrix<tt, -1, -1>& flop
    ) {
        swap(flip.storage, flop.storage);
    }
    //}}}
    //{{{ prefix matrix ops 

    //{{{ prefix - 
    template<class tt, int64 rr, int64 cc>
    matrix<tt, rr, cc> operator -(const matrix<tt, rr, cc>& aa) {
        const int64 rows = aa.rows();
        const int64 cols = aa.cols();
        matrix<tt, rr, cc> bb(rows, cols);
        for (int64 ii = 0; ii<rows; ii++) {
            for (int64 jj = 0; jj<cols; jj++) {
                bb(ii, jj) = -aa(ii, jj);
            }
        }
        return bb;
    }
    //}}}
    //{{{ prefix + 
    template<class tt, int64 rr, int64 cc>
    matrix<tt, rr, cc> operator +(const matrix<tt, rr, cc>& aa) {
        const int64 rows = aa.rows();
        const int64 cols = aa.cols();
        matrix<tt, rr, cc> bb(rows, cols);
        for (int64 ii = 0; ii<rows; ii++) {
            for (int64 jj = 0; jj<cols; jj++) {
                bb(ii, jj) = +aa(ii, jj);
            }
        }
        return bb;
    }
    //}}}

    //}}}
    //{{{ scalar matrix ops 

    //{{{ internal 
    namespace internal {

        template<class tt, class t0, class t1, int64 rr, int64 cc>
        matrix<tt, rr, cc> scalar_matrix_mul(
            const t0& aa, const matrix<t1, rr, cc>& bb
        ) {
            const int64 rows = bb.rows();
            const int64 cols = bb.cols();
            matrix<tt, rr, cc> uu(rows, cols);
            for (int64 ii = 0; ii<rows; ii++) {
                for (int64 jj = 0; jj<cols; jj++) {
                    uu(ii, jj) = aa*bb(ii, jj);
                }
            }
            return uu;
        }

        template<class tt, class t0, int64 rr, int64 cc, class t1>
        matrix<tt, rr, cc> matrix_scalar_mul(
            const matrix<t0, rr, cc>& aa, const t1& bb
        ) {
            const int64 rows = aa.rows();
            const int64 cols = aa.cols();
            matrix<tt, rr, cc> uu(rows, cols);
            for (int64 ii = 0; ii<rows; ii++) {
                for (int64 jj = 0; jj<cols; jj++) {
                    uu(ii, jj) = aa(ii, jj)*bb;
                }
            }
            return uu;
        }

    }
    //}}}
    //{{{ binary * 

    //{{{ scalar * matrix 
    template<class t0, class t1, int64 rr, int64 cc>
    matrix<typename arithmetic<t0, t1>::type, rr, cc> operator *(
        const t0& aa, const matrix<t1, rr, cc>& bb
    ) {
        typedef typename arithmetic<t0, t1>::type tt;
        return internal::scalar_matrix_mul<tt, t0, t1, rr, cc>(aa, bb);
    }
    //}}}
    //{{{ matrix * scalar 
    template<class t0, int64 rr, int64 cc, class t1>
    matrix<typename arithmetic<t0, t1>::type, rr, cc> operator *(
        const matrix<t0, rr, cc>& aa, const t1& bb
    ) {
        typedef typename arithmetic<t0, t1>::type tt;
        return internal::matrix_scalar_mul<tt, t0, rr, cc, t1>(aa, bb);
    }
    //}}}
    //{{{ complex * matrix 
    template<class t0, class t1, int64 rr, int64 cc>
    matrix<typename arithmetic<complex<t0>, t1>::type, rr, cc> operator *(
        const complex<t0>& aa, const matrix<t1, rr, cc>& bb
    ) {
        typedef typename arithmetic<complex<t0>, t1>::type tt;
        return internal::scalar_matrix_mul<tt, complex<t0>, t1, rr, cc>(aa, bb);
    }
    //}}}
    //{{{ matrix * complex 
    template<class t0, int64 rr, int64 cc, class t1>
    matrix<typename arithmetic<t0, complex<t1> >::type, rr, cc> operator *(
        const matrix<t0, rr, cc>& aa, const complex<t1>& bb
    ) {
        typedef typename arithmetic<t0, complex<t1> >::type tt;
        return internal::matrix_scalar_mul<tt, t0, rr, cc, complex<t1> >(aa, bb);
    }
    //}}}

    //}}}
    //{{{ binary / 

    //{{{ matrix / complex 
    template<class t0, int64 rr, int64 cc, class t1>
    matrix<typename arithmetic<t0, complex<t1> >::type, rr, cc> operator /(
        const matrix<t0, rr, cc>& aa, const complex<t1>& bb
    ) {
        typedef typename arithmetic<t0, complex<t1> >::type tt;
        return internal::matrix_scalar_mul<tt, t0, rr, cc, complex<t1> >(aa, 1/bb);
    }
    //}}}
    //{{{ matrix / scalar    
    template<class t0, int64 rr, int64 cc, class t1>
    matrix<typename arithmetic<t0, t1>::type, rr, cc> operator /(
        const matrix<t0, rr, cc>& aa, const t1& bb
    ) {
        typedef typename arithmetic<t0, t1>::type tt;
        return internal::matrix_scalar_mul<tt, t0, rr, cc, t1>(aa, 1/bb);
    }
    //}}}
    
    //}}}
    //{{{ binary *=  
    template<class t0, int64 r0, int64 c0, class t1>
    matrix<t0, r0, c0>& operator *=(matrix<t0, r0, c0>& aa, const t1& bb) {
        const int64 rows = aa.rows();
        const int64 cols = aa.cols();
        for (int64 ii = 0; ii<rows; ii++) {
            for (int64 jj = 0; jj<cols; jj++) {
                aa(ii, jj) *= bb;
            }
        }
        return aa;
    }
    //}}}
    //{{{ binary /=  
    template<class t0, int64 r0, int64 c0, class t1>
    matrix<t0, r0, c0>& operator /=(matrix<t0, r0, c0>& aa, const t1& bb) {
        return aa *= 1/bb;
    }
    //}}}

    //}}}
    //{{{ vector matrix ops 

    //{{{ internal 
    namespace internal {

        //{{{ matvectype
        template<class t0, int64 r0, int64 c0, class t1, int64 s1>
        struct matvectype {};

        template<class t0, class t1>
        struct matvectype<t0, -1, -1, t1, -1> {
            // dynamic*dynamic => dynamic
            typedef vector<typename arithmetic<t0, t1>::type, -1> type;
        };

        template<class t0, int64 r0, int64 c0, class t1>
        struct matvectype<t0, r0, c0, t1, -1> {
            // fixed*dynamic => dynamic
            typedef vector<typename arithmetic<t0, t1>::type, -1> type;
        };

        template<class t0, class t1, int64 s1>
        struct matvectype<t0, -1, -1, t1, s1> {
            // dynamic*fixed => dynamic
            typedef vector<typename arithmetic<t0, t1>::type, -1> type;
        };

        template<class t0, int64 r0, int64 c0, class t1>
        struct matvectype<t0, r0, c0, t1, c0> {
            // fixed*fixed => fixed, if matching size
            typedef vector<typename arithmetic<t0, t1>::type, r0> type;
        };
        //}}}
        //{{{ vecmattype
        template<class t0, int64 s0, class t1, int64 r1, int64 c1>
        struct vecmattype {};

        template<class t0, class t1>
        struct vecmattype<t0, -1, t1, -1, -1> {
            // dynamic*dynamic => dynamic
            typedef vector<typename arithmetic<t0, t1>::type, -1> type;
        };

        template<class t0, int64 s0, class t1>
        struct vecmattype<t0, s0, t1, -1, -1> {
            // fixed*dynamic => dynamic
            typedef vector<typename arithmetic<t0, t1>::type, -1> type;
        };

        template<class t0, class t1, int64 r1, int64 c1>
        struct vecmattype<t0, -1, t1, r1, c1> {
            // dynamic*fixed => dynamic
            typedef vector<typename arithmetic<t0, t1>::type, -1> type;
        };

        template<class t0, class t1, int64 r1, int64 c1>
        struct vecmattype<t0, r1, t1, r1, c1> {
            // fixed*fixed => fixed, if matching sizes
            typedef vector<typename arithmetic<t0, t1>::type, c1> type;
        };

        //}}}

    }
    //}}}
    //{{{ binary * 

    // matrix*vector
    template<class t0, int64 r0, int64 c0, class t1, int64 s1>
    typename internal::matvectype<t0, r0, c0, t1, s1>::type operator *(
        const matrix<t0, r0, c0>& aa, const vector<t1, s1>& bb
    ) {
        const int64 rows = aa.rows();
        const int64 cols = aa.cols();
        const int64 size = bb.size();
        check(cols == size, "compatible size %lld == %lld", cols, size);
        typename internal::matvectype<t0, r0, c0, t1, s1>::type cc(rows, 0);
        for (int64 ii = 0; ii<rows; ii++) {
            for (int64 jj = 0; jj<cols; jj++) {
                cc[ii] += aa(ii, jj)*bb[jj];
            }
        }
        return cc;
    }

    // vector*matrix
    template<class t0, int64 s0, class t1, int64 r1, int64 c1>
    typename internal::vecmattype<t0, s0, t1, r1, c1>::type operator *(
        const vector<t0, s0>& aa, const matrix<t1, r1, c1>& bb
    ) {
        const int64 size = aa.size();
        const int64 rows = bb.rows();
        const int64 cols = bb.cols();
        check(size == rows, "compatible size %lld == %lld", size, rows);
        typename internal::vecmattype<t0, s0, t1, r1, c1>::type cc(cols, 0);
        for (int64 jj = 0; jj<cols; jj++) {
            for (int64 ii = 0; ii<rows; ii++) {
                cc[ii] += aa[ii]*bb(ii, jj);
            }
        }
        return cc;
    }

    //}}}

    //}}}
    //{{{ matrix matrix ops 

    //{{{ internal 
    namespace internal {
        // Returns the compile time size, preferring dynamic over fixed.
        template<int64 s0, int64 s1> struct matrixsize {};
        template<int64 ss> struct matrixsize<ss, ss> { enum { size=ss }; };
        template<int64 s0> struct matrixsize<s0, -1> { enum { size=-1 }; };
        template<int64 s1> struct matrixsize<-1, s1> { enum { size=-1 }; };
        template<>         struct matrixsize<-1, -1> { enum { size=-1 }; };

        // Returns the compile time type appropriate for addition and subtraction
        template<class t0, int64 r0, int64 c0, class t1, int64 r1, int64 c1>
        struct mataddtype {
            typedef matrix<
                typename arithmetic<t0, t1>::type,
                matrixsize<r0, r1>::size,
                matrixsize<c0, c1>::size
            > type;
        };

        // Multiplication, we prefer the first argument unless the second is dynamic
        template<int64 s0, int64 s1> struct matmulsize { enum { size=s0 }; };
        template<int64 s0> struct matmulsize<s0, -1>   { enum { size=-1 }; };

        // Returns the compile time type appropriate for multiplication
        template<class t0, int64 r0, int64 c0, class t1, int64 r1, int64 c1>
        struct matmultype {};

        template<class t0, class t1>
        struct matmultype<t0, -1, -1, t1, -1, -1> {
            // dynamic*dynamic => dynamic
            typedef matrix<typename arithmetic<t0, t1>::type, -1, -1> type;
        };

        template<class t0, int64 r0, int64 c0, class t1>
        struct matmultype<t0, r0, c0, t1, -1, -1> {
            // fixed*dynamic => dynamic
            typedef matrix<typename arithmetic<t0, t1>::type, -1, -1> type;
        };

        template<class t0, class t1, int64 r1, int64 c1>
        struct matmultype<t0, -1, -1, t1, r1, c1> {
            // dynamic*fixed => dynamic
            typedef matrix<typename arithmetic<t0, t1>::type, -1, -1> type;
        };

        template<class t0, int64 r0, int64 ss, class t1, int64 c1>
        struct matmultype<t0, r0, ss, t1, ss, c1> {
            // fixed*fixed => fixed, if correct size
            typedef matrix<typename arithmetic<t0, t1>::type, r0, c1> type;
        };

    }
    //}}}
    //{{{ binary == 
    template<class t0, int64 r0, int64 c0, class t1, int64 r1, int64 c1>
    bool operator ==(const matrix<t0, r0, c0>& aa, const matrix<t1, r1, c1>& bb) {
        const int64 rows = aa.rows();
        const int64 cols = aa.cols();
        check(bb.rows() == rows, "matching rows");
        check(bb.cols() == cols, "matching cols");
        for (int64 ii = 0; ii<rows; ii++) {
            for (int64 jj = 0; jj<cols; jj++) {
                if (aa(ii, jj) != bb(ii, jj)) return false;
            }
        }
        return true;
    }
    //}}}
    //{{{ binary != 
    template<class t0, int64 r0, int64 c0, class t1, int64 r1, int64 c1>
    bool operator !=(const matrix<t0, r0, c0>& aa, const matrix<t1, r1, c1>& bb) {
        const int64 rows = aa.rows();
        const int64 cols = aa.cols();
        check(bb.rows() == rows, "matching rows");
        check(bb.cols() == cols, "matching cols");
        for (int64 ii = 0; ii<rows; ii++) {
            for (int64 jj = 0; jj<cols; jj++) {
                if (aa(ii, jj) == bb(ii, jj)) return false;
            }
        }
        return true;
    }
    //}}}
    //{{{ binary + 
    template<class t0, int64 r0, int64 c0, class t1, int64 r1, int64 c1>
    typename internal::mataddtype<t0, r0, c0, t1, r1, c1>::type operator +(
        const matrix<t0, r0, c0>& aa, const matrix<t1, r1, c1>& bb
    ) {
        const int64 rows = aa.rows();
        const int64 cols = aa.cols();
        check(bb.rows() == rows, "matching rows %lld == %lld", rows, bb.rows());
        check(bb.cols() == cols, "matching cols %lld == %lld", cols, bb.cols());
        typename internal::mataddtype<t0, r0, c0, t1, r1, c1>::type cc(rows, cols);
        for (int64 ii = 0; ii<rows; ii++) {
            for (int64 jj = 0; jj<cols; jj++) {
                cc(ii, jj) = aa(ii, jj) + bb(ii, jj);
            }
        }
        return cc;
    }
    //}}}
    //{{{ binary - 
    template<class t0, int64 r0, int64 c0, class t1, int64 r1, int64 c1>
    typename internal::mataddtype<t0, r0, c0, t1, r1, c1>::type operator -(
        const matrix<t0, r0, c0>& aa, const matrix<t1, r1, c1>& bb
    ) {
        const int64 rows = aa.rows();
        const int64 cols = aa.cols();
        check(bb.rows() == rows, "matching rows %lld == %lld", rows, bb.rows());
        check(bb.cols() == cols, "matching cols %lld == %lld", cols, bb.cols());
        typename internal::mataddtype<t0, r0, c0, t1, r1, c1>::type cc(rows, cols);
        for (int64 ii = 0; ii<rows; ii++) {
            for (int64 jj = 0; jj<cols; jj++) {
                cc(ii, jj) = aa(ii, jj) - bb(ii, jj);
            }
        }
        return cc;
    }
    //}}}
    //{{{ binary * 
    //{{{ internal
    namespace internal {
        //
        // This algorithm is taken from the Wikipedia page on matrix
        // multiplication and references "Cache-Oblivious Algorithms",
        // from Harald Prokop's 1999 Master's thesis at MIT.
        //
        // aa[rows, cols] = bb[rows, size] * cc[size, cols]
        //
        template<class atype, class btype, class ctype>
        void recursive_matrix_mul(
            atype* aa, const btype* bb, const ctype* cc,
            int64 rows, int64 cols, int64 size, bool zero,
            int64 ilo, int64 ihi,
            int64 jlo, int64 jhi,
            int64 klo, int64 khi
        ) {
            for (;;) {
                int64 di = ihi - ilo;
                int64 dj = jhi - jlo;
                int64 dk = khi - klo;
                int64 mm = max(di, dj, dk);

                if (mm <= 32) {
                    for (int64 ii = ilo; ii<ihi; ii++) {
                        for (int64 jj = jlo; jj<jhi; jj++) {
                            atype sum = zero ? 0 : aa[ii*cols + jj];
                            for (int64 kk = klo; kk<khi; kk++) {
                                sum += bb[ii*size + kk] * cc[kk*cols + jj];
                            }
                            aa[ii*cols + jj] = sum;
                        }
                    }
                    return;
                }

                if (mm == di) {
                    recursive_matrix_mul(
                        aa, bb, cc, rows, cols, size, zero,
                        ilo, ilo + di/2, jlo, jhi, klo, khi
                    );
                    ilo += di/2;
                    continue;
                }

                if (mm == dj) {
                    recursive_matrix_mul(
                        aa, bb, cc, rows, cols, size, zero,
                        ilo, ihi, jlo, jlo + dj/2, klo, khi
                    );
                    jlo += dj/2;
                    continue;
                }

                recursive_matrix_mul(
                    aa, bb, cc, rows, cols, size, zero,
                    ilo, ihi, jlo, jhi, klo, klo + dk/2
                );
                zero = false;
                klo += dk/2;
            }
        }
    }
    //}}}

    template<class t0, int64 r0, int64 c0, class t1, int64 r1, int64 c1>
    typename internal::matmultype<t0, r0, c0, t1, r1, c1>::type operator *(
        const matrix<t0, r0, c0>& bb, const matrix<t1, r1, c1>& cc
    ) {
        const int64 rows = bb.rows();
        const int64 cols = cc.cols();
        const int64 size = bb.cols();
        check(cc.rows() == size, "compatible size %lld == %lld", size, cc.rows());
        typename internal::matmultype<t0, r0, c0, t1, r1, c1>::type aa(rows, cols);
        internal::recursive_matrix_mul(
            aa.data(), bb.data(), cc.data(),
            rows, cols, size, true,
            0, rows, 0, cols, 0, size
        );
        return aa;
    }
    //}}}
    //{{{ binary % 
    // NOTE: We're abusing the % operator for elementwise multiplication.
    // We never need element-wise modulo, and it's the right precedence.
    template<class t0, int64 r0, int64 c0, class t1, int64 r1, int64 c1>
    typename internal::mataddtype<t0, r0, c0, t1, r1, c1>::type operator %(
        const matrix<t0, r0, c0>& aa, const matrix<t1, r1, c1>& bb
    ) {
        const int64 rows = aa.rows();
        const int64 cols = aa.cols();
        check(bb.rows() == rows, "matching rows %lld == %lld", rows, bb.rows());
        check(bb.cols() == cols, "matching cols %lld == %lld", cols, bb.cols());
        typename internal::mataddtype<t0, r0, c0, t1, r1, c1>::type cc(rows, cols);
        for (int64 ii = 0; ii<rows; ii++) {
            for (int64 jj = 0; jj<cols; jj++) {
                cc(ii, jj) = aa(ii, jj) * bb(ii, jj);
            }
        }
        return cc;
    }
    //}}}
    //{{{ binary += 
    template<class t0, int64 r0, int64 c0, class t1, int64 r1, int64 c1>
    matrix<t0, r0, c0>& operator +=(matrix<t0, r0, c0>& aa, const matrix<t1, r1, c1>& bb) {
        const int64 rows = aa.rows();
        const int64 cols = aa.cols();
        check(bb.rows() == rows, "matching rows %lld == %lld", rows, bb.rows());
        check(bb.cols() == cols, "matching cols %lld == %lld", cols, bb.cols());
        for (int64 ii = 0; ii<rows; ii++) {
            for (int64 jj = 0; jj<cols; jj++) {
                aa(ii, jj) += bb(ii, jj);
            }
        }
        return aa;
    }
    //}}}
    //{{{ binary -= 
    template<class t0, int64 r0, int64 c0, class t1, int64 r1, int64 c1>
    matrix<t0, r0, c0>& operator -=(matrix<t0, r0, c0>& aa, const matrix<t1, r1, c1>& bb) {
        const int64 rows = aa.rows();
        const int64 cols = aa.cols();
        check(bb.rows() == rows, "matching rows %lld == %lld", rows, bb.rows());
        check(bb.cols() == cols, "matching cols %lld == %lld", cols, bb.cols());
        for (int64 ii = 0; ii<rows; ii++) {
            for (int64 jj = 0; jj<cols; jj++) {
                aa(ii, jj) -= bb(ii, jj);
            }
        }
        return aa;
    }
    //}}}
    //{{{ binary *= 
    template<class t0, int64 r0, int64 c0, class t1, int64 r1, int64 c1>
    matrix<t0, r0, c0>& operator *=(matrix<t0, r0, c0>& aa, const matrix<t1, r1, c1>& bb) {
        return aa = aa*bb;
    }
    //}}}
    //{{{ binary %= 
    template<class t0, int64 r0, int64 c0, class t1, int64 r1, int64 c1>
    matrix<t0, r0, c0>& operator %=(matrix<t0, r0, c0>& aa, const matrix<t1, r1, c1>& bb) {
        const int64 rows = aa.rows();
        const int64 cols = aa.cols();
        check(bb.rows() == rows, "matching rows %lld == %lld", rows, bb.rows());
        check(bb.cols() == cols, "matching cols %lld == %lld", cols, bb.cols());
        for (int64 ii = 0; ii<rows; ii++) {
            for (int64 jj = 0; jj<cols; jj++) {
                aa(ii, jj) *= bb(ii, jj);
            }
        }
        return aa;
    }
    //}}}

    //}}}
    //{{{ diag, conj, trans, herm
    
    template<class tt, int64 ss> // build a diagonal matrix from a vector
    matrix<tt, ss, ss> diag(const vector<tt, ss>& dd) {
        const int64 size = dd.size();
        matrix<tt, ss, ss> result(size, size);
        for (int64 ii = 0; ii<size; ii++) {
            for (int64 jj = 0; jj<size; jj++) {
                result(ii, jj) = (ii == jj) ? dd[ii] : 0;
            }
        }
        return result;
    }

    template<class tt, int64 rr, int64 cc> // build a conjugate (not transpose) of the matrix
    matrix<tt, rr, cc> conj(const matrix<tt, rr, cc>& aa) {
        const int64 rows = aa.rows();
        const int64 cols = aa.cols();
        matrix<tt, rr, cc> result(rows, cols);
        for (int64 ii = 0; ii<rows; ii++) {
            for (int64 jj = 0; jj<cols; jj++) {
                result(ii, jj) = conj(aa(ii, jj));
            }
        }
        return result;
    }

    //{{{ internal 
    namespace internal {
        //
        // This algorithm, and the one Hermition equivalent below, is taken
        // from Wikipedia which references "Cache-Oblivious Algorithms", from
        // Harald Prokop's 1999 Master's thesis at MIT.
        //
        // aa[rows, cols] = trans(bb[cols, rows])
        //
        template<class type>
        void recursive_matrix_trans(
            type* aa, const type* bb,
            int64 rows, int64 cols,
            int64 ilo, int64 ihi,
            int64 jlo, int64 jhi
        ) {
            for (;;) {
                int64 di = ihi - ilo;
                int64 dj = jhi - jlo;
                int64 mm = max(di, dj);

                if (mm <= 32) {
                    for (int64 ii = ilo; ii<ihi; ii++) {
                        for (int64 jj = jlo; jj<jhi; jj++) {
                            aa[ii*cols + jj] = bb[jj*rows + ii];
                        }
                    }
                    return;
                }

                if (mm == di) {
                    recursive_matrix_trans(
                        aa, bb, rows, cols,
                        ilo, ilo + di/2, jlo, jhi
                    );
                    ilo += di/2;
                    continue;
                }

                if (mm == dj) {
                    recursive_matrix_trans(
                        aa, bb, rows, cols,
                        ilo, ihi, jlo, jlo + dj/2
                    );
                    jlo += dj/2;
                    continue;
                }
            }
        }

        //
        // aa[rows, cols] = herm(bb[cols, rows])
        //
        template<class type>
        void recursive_matrix_herm(
            type* aa, const type* bb,
            int64 rows, int64 cols,
            int64 ilo, int64 ihi,
            int64 jlo, int64 jhi
        ) {
            for (;;) {
                int64 di = ihi - ilo;
                int64 dj = jhi - jlo;
                int64 mm = max(di, dj);

                if (mm <= 32) {
                    for (int64 ii = ilo; ii<ihi; ii++) {
                        for (int64 jj = jlo; jj<jhi; jj++) {
                            aa[ii*cols + jj] = conj(bb[jj*rows + ii]);
                        }
                    }
                    return;
                }

                if (mm == di) {
                    recursive_matrix_herm(
                        aa, bb, rows, cols,
                        ilo, ilo + di/2, jlo, jhi
                    );
                    ilo += di/2;
                    continue;
                }

                if (mm == dj) {
                    recursive_matrix_herm(
                        aa, bb, rows, cols,
                        ilo, ihi, jlo, jlo + dj/2
                    );
                    jlo += dj/2;
                    continue;
                }
            }
        }

    }
    //}}}

    template<class tt, int64 rr, int64 cc> // build a transpose (not conjugate) of the matrix
    matrix<tt, rr, cc> trans(const matrix<tt, cc, rr>& bb) {
        // these are swapped on purpose
        const int64 rows = bb.cols();
        const int64 cols = bb.rows();
        matrix<tt, rr, cc> aa(rows, cols);
        internal::recursive_matrix_trans(
            aa.data(), bb.data(), rows, cols,
            0, rows, 0, cols
        );
        return aa;
    }

    template<class tt, int64 rr, int64 cc> // build the conjugate transpose of the matrix
    matrix<tt, rr, cc> herm(const matrix<tt, cc, rr>& bb) {
        // these are swapped on purpose
        const int64 rows = bb.cols();
        const int64 cols = bb.rows();
        matrix<tt, rr, cc> aa(rows, cols);
        internal::recursive_matrix_herm(
            aa.data(), bb.data(), rows, cols,
            0, rows, 0, cols
        );
        return aa;
    }

    //}}}

    /* XXX: how to handle the template sizes?
    template<class tt, int64 rr, int64 cc> // build a (maybe rectangular) identity matrix
    matrix<tt, rr, cc> zeros(int64 rows, int64 cols) {
        return matrix<tt, rr, cc>(rows, cols, 0);
    }
    */

    /* XXX: how to handle the template sizes?
    template<class tt, int64 rr, int64 cc> // build a (maybe rectangular) identity matrix
    matrix<tt, rr, cc> ident(int64 rows, int64 cols) {
        matrix<tt, rr, cc> result(rows, cols);
        for (int64 ii = 0; ii<rows; ii++) {
            for (int64 jj = 0; jj<cols; jj++) {
                result(ii, jj) = (ii == jj) ? 1 : 0;
            }
        }
        return result;
    }
    */

    /* XXX: rename to setzero() ?
    template<class type>
    void zero(matrix<type>& aa) {
        const int64 rows = aa.rows();
        const int64 cols = aa.cols();
        for (int64 ii = 0; ii<rows; ii++) {
            for (int64 jj = 0; jj<cols; jj++) {
                aa(ii, jj) = 0;
            }
        }
    }
    */

}

#endif // XM_MATRIX_H_

