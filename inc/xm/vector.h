#ifndef XM_VECTOR_H_
#define XM_VECTOR_H_ 1

#include "basics.h"
#include "promote.h"

namespace xm {

    //{{{ vector fixed size

    // This is the fixed sized implementation.  The size of the vector is
    // determined by the template arguments, and no dynamic allocation occurs
    // at runtime.  The dynamically sized specialization is below.
    template<class type, int64 ss=-1>
    struct vector {
        ~vector();
        vector();
        vector(int64 size);
        vector(int64 size, const type& init);

        template<class tt>
        vector(const vector<tt, ss>& other);
        template<class tt>
        vector(const vector<tt, -1>& other);
        vector(const vector<type, ss>& other);

        template<class tt>
        vector<type, ss>& operator =(const vector<tt, ss>& other);
        template<class tt>
        vector<type, ss>& operator =(const vector<tt, -1>& other);
        vector<type, ss>& operator =(const vector<type, ss>& other);

        type& operator [](int64 index);
        const type& operator [](int64 index) const;

        int64 size() const;

        type* data();
        const type* data() const;

        private:
            template<class t0, int64 s0> friend void swap(
                vector<t0, s0>& flip, vector<t0, s0>& flop
            );
            template<class anyt, int64 anys> friend struct vector;

            type storage[ss];
    };

    template<class type, int64 ss> // XXX: do we need this one?
    vector<type, ss>::~vector() {}

    template<class type, int64 ss> // XXX: do we need this one?
    vector<type, ss>::vector() {}

    template<class type, int64 ss>
    vector<type, ss>::vector(int64 size) {
        check(size == ss, "matching size %lld for fixed size %lld", size, ss);
    }

    template<class type, int64 ss>
    vector<type, ss>::vector(int64 size, const type& init) {
        check(size == ss, "matching size %lld for fixed size %lld", size, ss);
        for (int64 ii = 0; ii<ss; ii++) {
            storage[ii] = init;
        }
    }

    template<class type, int64 ss>
    template<class tt>
    vector<type, ss>::vector(const vector<tt, ss>& other) {
        for (int64 ii = 0; ii<ss; ii++) {
            storage[ii] = other.storage[ii];
        }
    }

    template<class type, int64 ss>
    template<class tt>
    vector<type, ss>::vector(const vector<tt, -1>& other) {
        const int64 size = other.size();
        check(size == ss, "matching rows %lld for fixed size %lld", size, ss);
        for (int64 ii = 0; ii<ss; ii++) {
            storage[ii] = other[ii];
        }
    }

    template<class type, int64 ss>
    vector<type, ss>::vector(const vector<type, ss>& other) {
        for (int64 ii = 0; ii<ss; ii++) {
            storage[ii] = other.storage[ii];
        }
    }

    template<class type, int64 ss>
    template<class tt>
    vector<type, ss>& vector<type, ss>::operator =(const vector<tt, ss>& other) {
        for (int64 ii = 0; ii<ss; ii++) {
            storage[ii] = other.storage[ii];
        }
        return *this;
    }

    template<class type, int64 ss>
    template<class tt>
    vector<type, ss>& vector<type, ss>::operator =(const vector<tt, -1>& other) {
        check(other.size() == ss, "matching size");
        for (int64 ii = 0; ii<ss; ii++) {
            storage[ii] = other[ii];
        }
        return *this;
    }

    template<class type, int64 ss>
    vector<type, ss>& vector<type, ss>::operator =(const vector<type, ss>& other) {
        for (int64 ii = 0; ii<ss; ii++) {
            storage[ii] = other.storage[ii];
        }
        return *this;
    }

    template<class type, int64 ss>
    type& vector<type, ss>::operator [](int64 index) {
        return storage[index];
    }

    template<class type, int64 ss>
    const type& vector<type, ss>::operator [](int64 index) const {
        return storage[index];
    }

    template<class type, int64 ss>
    int64 vector<type, ss>::size() const { return ss; }

    template<class type, int64 ss>
    type* vector<type, ss>::data() { return storage; }

    template<class type, int64 ss>
    const type* vector<type, ss>::data() const { return storage; }

    template<class t0, int64 s0>
    void swap(vector<t0, s0>& flip, vector<t0, s0>& flop) {
        for (int64 ii = 0; ii<s0; ii++) {
            swap(flip.data[ii], flop.data[ii]);
        }
    }
    //}}}
    //{{{ vector dynamic size

    // Dynamically sized vector using the default arguments from the template
    // above.  The interface is nearly identical, and operations should mix and
    // match with either.
    template<class type>
    struct vector<type, -1> {
        ~vector();
        vector();

        vector(int64 ss);
        vector(int64 ss, const type& init);

        template<class tt, int64 ss>
        vector(const vector<tt, ss>& other);
        vector(const vector<type, -1>& other);

        template<class tt, int64 ss>
        vector<type, -1>& operator =(const vector<tt, ss>& other);
        vector<type, -1>& operator =(const vector<type, -1>& other);

        type& operator [](int64 index);
        const type& operator [](int64 index) const;

        int64 size() const;

        void resize(int64 ss);
        void resize(int64 ss, const type& init);
        void clear();

        type* data();
        const type* data() const;

        private:
            void realloc(int64 ss);

            template<class tt, int64 ss>
            void assign(const vector<tt, ss>& other);

            template<class tt> friend void swap(
                vector<tt, -1>& flip, vector<tt, -1>& flop
            );
            template<class anyt, int64 anys> friend struct vector;

            struct implementation {
                int64 size, padding_;
                type bucket[1];
            } *storage;
    };

    template<class type>
    vector<type, -1>::~vector() { clear(); }

    template<class type>
    vector<type, -1>::vector() : storage(0) {}

    template<class type>
    vector<type, -1>::vector(int64 ss) : storage(0) {
        resize(ss);
    }

    template<class type>
    vector<type, -1>::vector(int64 ss, const type& init) : storage(0) {
        resize(ss, init);
    }

    template<class type>
    template<class tt, int64 ss>
    vector<type, -1>::vector(const vector<tt, ss>& other) : storage(0) {
        assign(other);
    }

    template<class type>
    vector<type, -1>::vector(const vector<type, -1>& other) : storage(0) {
        assign(other);
    }

    template<class type>
    template<class tt, int64 ss>
    vector<type, -1>& vector<type, -1>::operator =(
        const vector<tt, ss>& other
    ) {
        assign(other);
        return *this;
    }

    template<class type>
    vector<type, -1>& vector<type, -1>::operator =(
            const vector<type, -1>& other
    ) {
        assign(other);
        return *this;
    }

    template<class type>
    int64 vector<type, -1>::size() const {
        return storage ? storage->size : 0;
    }

    template<class type>
    type& vector<type, -1>::operator [](int64 index) {
        return storage->bucket[index];
    }

    template<class type>
    const type& vector<type, -1>::operator [](int64 index) const {
        return storage->bucket[index];
    }

    template<class type>
    void vector<type, -1>::resize(int64 size) {
        realloc(size);
        for (int64 ii = 0; ii<size; ii++) {
            new(storage->bucket + ii) type;
        }
    }

    template<class type>
    void vector<type, -1>::resize(int64 size, const type& init) {
        realloc(size);
        for (int64 ii = 0; ii<size; ii++) {
            new(storage->bucket + ii) type(init);
        }
    }

    template<class type>
    void vector<type, -1>::clear() {
        if (storage) {
            const int64 size = storage->size;
            for (int64 ii = 0; ii<size; ii++) {
                storage->bucket[ii].~type();
            }
        }
        free(storage);
        storage = 0;
    }

    template<class type>
    type* vector<type, -1>::data() {
        return storage ? storage->bucket : 0;
    }

    template<class type>
    const type* vector<type, -1>::data() const {
        return storage ? storage->bucket : 0;
    }

    template<class type>
    void vector<type, -1>::realloc(int64 size) {
        check(size >= 0, "must have positive number of rows");
        // In both branches of this if, we leave the objects unconstructed.
        // The calling function will construct objects into the memory.
        if (storage && storage->size == size) {
            // re-use the same memory, but destruct the objects
            for (int64 ii = 0; ii<size; ii++) {
                storage->bucket[ii].~type();
            }
        } else {
            // XXX: If size == 0, consider leaving as a null ptr
            clear(); // create new unconstructed memory
            storage = alloc<implementation>((size - 1)*sizeof(type));
        }
        storage->size = size;
    }

    template<class type>
    template<class tt, int64 ss>
    void vector<type, -1>::assign(const vector<tt, ss>& other) {
        if ((void*)this == (void*)&other) return;
        const int64 size = other.size();
        realloc(size);
        for (int64 ii = 0; ii<size; ii++) {
            new(storage->bucket + ii) type(other[ii]);
        }
    }

    template<class tt>
    void swap(vector<tt, -1>& flip, vector<tt, -1>& flop) {
        swap(flip.storage, flop.storage);
    }
    //}}}
    //{{{ prefix vector ops 

    //{{{ prefix - 
    template<class tt, int64 ss>
    vector<tt, ss> operator -(const vector<tt, ss>& aa) {
        const int64 size = aa.size();
        vector<tt, ss> bb(size);
        for (int64 ii = 0; ii<size; ii++) {
            bb[ii] = -aa[ii];
        }
        return bb;
    }
    //}}}
    //{{{ prefix + 
    template<class tt, int64 ss>
    vector<tt, ss> operator +(const vector<tt, ss>& aa) {
        const int64 size = aa.size();
        vector<tt, ss> bb(size);
        for (int64 ii = 0; ii<size; ii++) {
            bb[ii] = +aa[ii];
        }
        return bb;
    }
    //}}}

    //}}}
    //{{{ scalar vector ops 

    //{{{ internal 
    namespace internal {

        template<class tt, class t0, class t1, int64 ss>
        vector<tt, ss> scalar_vector_mul(
            const t0& aa, const vector<t1, ss>& bb
        ) {
            const int64 size = bb.size();
            vector<tt, ss> cc(size);
            for (int64 ii = 0; ii<size; ii++) {
                cc[ii] = aa*bb[ii];
            }
            return cc;
        }

        template<class tt, class t0, int64 ss, class t1>
        vector<tt, ss> vector_scalar_mul(
            const vector<t0, ss>& aa, const t1& bb
        ) {
            const int64 size = aa.size();
            vector<tt, ss> cc(size);
            for (int64 ii = 0; ii<size; ii++) {
                cc[ii] = aa[ii]*bb;
            }
            return cc;
        }

    }
    //}}}
    //{{{ binary * 

    //{{{ scalar * vector 
    template<class t0, class t1, int64 ss>
    vector<typename arithmetic<t0, t1>::type, ss> operator *(
        const t0& aa, const vector<t1, ss>& bb
    ) {
        typedef typename arithmetic<t0, t1>::type tt;
        return internal::scalar_vector_mul<tt, t0, t1, ss>(aa, bb);
    }
    //}}}
    //{{{ vector * scalar 
    template<class t0, int64 ss, class t1>
    vector<typename arithmetic<t0, t1>::type, ss> operator *(
        const vector<t0, ss>& aa, const t1& bb
    ) {
        typedef typename arithmetic<t0, t1>::type tt;
        return internal::vector_scalar_mul<tt, t0, ss, t1>(aa, bb);
    }
    //}}}
    //{{{ complex * vector 
    template<class t0, class t1, int64 ss>
    vector<typename arithmetic<complex<t0>, t1>::type, ss> operator *(
        const complex<t0>& aa, const vector<t1, ss>& bb
    ) {
        typedef typename arithmetic<complex<t0>, t1>::type tt;
        return internal::scalar_vector_mul<tt, complex<t0>, t1, ss>(aa, bb);
    }
    //}}}
    //{{{ vector * complex 
    template<class t0, int64 ss, class t1> 
    vector<typename arithmetic<t0, complex<t1> >::type, ss> operator *(
        const vector<t0, ss>& aa, const complex<t1>& bb
    ) {
        typedef typename arithmetic<t0, complex<t1> >::type tt;
        return internal::vector_scalar_mul<tt, complex<t0>, ss, complex<t1> >(aa, bb);
    }
    //}}}
    
    //}}}
    //{{{ binary / 

    //{{{ vector / complex 
    template<class t0, int64 ss, class t1>
    vector<typename arithmetic<t0, complex<t1> >::type, ss> operator /(
        const vector<t0, ss>& aa, const complex<t1>& bb
    ) {
        typedef typename arithmetic<t0, complex<t1> >::type tt;
        return internal::vector_scalar_mul<tt, t0, ss, complex<t1> >(aa, 1/bb);
    }
    //}}}
    //{{{ vector / scalar 
    template<class t0, int64 ss, class t1>
    vector<typename arithmetic<t0, t1>::type, ss> operator /(
        const vector<t0, ss>& aa, const t1& bb
    ) {
        typedef typename arithmetic<t0, t1>::type tt;
        return internal::vector_scalar_mul<tt, t0, ss, t1>(aa, 1/bb);
    }
    //}}}
    
    //}}}
    //{{{ binary *= 
    template<class t0, int64 s0, class t1>
    vector<t0, s0>& operator *=(vector<t0, s0>& aa, const t1& bb) {
        const int64 size = aa.size();
        for (int64 ii = 0; ii<size; ii++) {
            aa[ii] *= bb;
        }
        return aa;
    }
    //}}}
    //{{{ binary /= 
    template<class t0, int64 s0, class t1>
    vector<t0, s0>& operator /=(vector<t0, s0>& aa, const t1& bb) {
        return aa *= 1/bb;
    }
    //}}}

    //}}}
    //{{{ vector vector ops 

    //{{{ internal
    namespace internal {
        // Returns the compile time size, preferring dynamic over fixed.
        template<int64 s0, int64 s1> struct vecdynsize {};
        template<int64 ss> struct vecdynsize<ss, ss> { enum { size=ss }; };
        template<int64 s0> struct vecdynsize<s0, -1> { enum { size=-1 }; };
        template<int64 s1> struct vecdynsize<-1, s1> { enum { size=-1 }; };
        template<>         struct vecdynsize<-1, -1> { enum { size=-1 }; };

        // Returns the compile time type appropriate for elementwise ops
        template<class t0, int64 s0, class t1, int64 s1>
        struct vecaddtype {
            typedef vector<
                typename arithmetic<t0, t1>::type,
                vecdynsize<s0, s1>::size
            > type;
        };
    }
    //}}}
    //{{{ binary == 
    template<class t0, int64 s0, class t1, int64 s1>
    bool operator ==(const vector<t0, s0>& aa, const vector<t1, s1>& bb) {
        const int64 size = aa.size();
        check(bb.size() == size, "matching size");
        for (int64 ii = 0; ii<size; ii++) {
            if (aa[ii] != bb[ii]) return false;
        }
        return true;
    }
    //}}}
    //{{{ binary != 
    template<class t0, int64 s0, class t1, int64 s1>
    bool operator !=(const vector<t0, s0>& aa, const vector<t1, s1>& bb) {
        const int64 size = aa.size();
        check(bb.size() == size, "matching size");
        for (int64 ii = 0; ii<size; ii++) {
            if (aa[ii] == bb[ii]) return false;
        }
        return true;
    }
    //}}}
    //{{{ binary + 
    template<class t0, int64 s0, class t1, int64 s1>
    typename internal::vecaddtype<t0, s0, t1, s1>::type operator +(
        const vector<t0, s0>& aa, const vector<t1, s1>& bb
    ) {
        const int64 size = aa.size();
        check(bb.size() == size, "matching size %lld == %lld", size, bb.size());
        typename internal::vecaddtype<t0, s0, t1, s1>::type cc(size);
        for (int64 ii = 0; ii<size; ii++) {
            cc[ii] = aa[ii] + bb[ii];
        }
        return cc;
    }
    //}}}
    //{{{ binary - 
    template<class t0, int64 s0, class t1, int64 s1>
    typename internal::vecaddtype<t0, s0, t1, s1>::type operator -(
        const vector<t0, s0>& aa, const vector<t1, s1>& bb
    ) {
        const int64 size = aa.size();
        check(bb.size() == size, "matching size %lld == %lld", size, bb.size());
        typename internal::vecaddtype<t0, s0, t1, s1>::type cc(size);
        for (int64 ii = 0; ii<size; ii++) {
            cc[ii] = aa[ii] - bb[ii];
        }
        return cc;
    }
    //}}}
    //{{{ binary % 
    // NOTE: We're abusing the % operator for elementwise multiplication.
    // We never need element-wise modulo, and it's the right precedence.
    template<class t0, int64 s0, class t1, int64 s1>
    typename internal::vecaddtype<t0, s0, t1, s1>::type operator %(
        const vector<t0, s0>& aa, const vector<t1, s1>& bb
    ) {
        const int64 size = aa.size();
        check(bb.size() == size, "matching size %lld == %lld", size, bb.size());
        typename internal::vecaddtype<t0, s0, t1, s1>::type cc(size);
        for (int64 ii = 0; ii<size; ii++) {
            cc[ii] = aa[ii] * bb[ii];
        }
        return cc;
    }
    //}}}
    //{{{ binary += 
    template<class t0, int64 s0, class t1, int64 s1>
    vector<t0, s0>& operator +=(vector<t0, s0>& aa, const vector<t1, s1>& bb) {
        const int64 size = aa.size();
        check(bb.size() == size, "matching size %lld == %lld", size, bb.size());
        for (int64 ii = 0; ii<size; ii++) {
            aa[ii] += bb[ii];
        }
        return aa;
    }
    //}}}
    //{{{ binary -= 
    template<class t0, int64 s0, class t1, int64 s1>
    vector<t0, s0>& operator -=(vector<t0, s0>& aa, const vector<t1, s1>& bb) {
        const int64 size = aa.size();
        check(bb.size() == size, "matching size %lld == %lld", size, bb.size());
        for (int64 ii = 0; ii<size; ii++) {
            aa[ii] -= bb[ii];
        }
        return aa;
    }
    //}}}
    //{{{ binary %= 
    template<class t0, int64 s0, class t1, int64 s1>
    vector<t0, s0>& operator %=(vector<t0, s0>& aa, const vector<t1, s1>& bb) {
        const int64 size = aa.size();
        check(bb.size() == size, "matching size %lld == %lld", size, bb.size());
        for (int64 ii = 0; ii<size; ii++) {
            aa[ii] += bb[ii];
        }
        return aa;
    }
    //}}}

    //}}}

    // XXX: inner(a, b)
    // XXX: outer(a, b)
}

#endif // XM_VECTOR_H_

