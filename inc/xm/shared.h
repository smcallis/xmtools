#ifndef XM_SHARED_H_
#define XM_SHARED_H_ 1

#include "basics.h"

namespace xm {

    template<class type>
    struct shared {
        ~shared();
        shared();
        shared(type* pp);
        shared(const shared<type>& other);
        shared<type>& operator =(const shared<type>& other);

        type& operator *();
        const type& operator *() const;

        type* operator ->();
        const type* operator ->() const;

        type* borrow();
        const type* borrow() const;

        private:

            template<class tt> friend void swap(
                shared<tt>& flip, shared<tt>& flop
            );

            struct counted {
                type* pointer;
                ssize_t refcount;
            };

            void decref();
            counted* incref();

            counted* storage;
    };

    template<class type>
    shared<type>::~shared() {
        decref();
    }

    template<class type>
    shared<type>::shared() : storage(0) {}

    template<class type>
    shared<type>::shared(type* pp) : storage(new counted) {
        storage->refcount = 1;
        storage->pointer = pp;
    }

    template<class type>
    shared<type>::shared(const shared<type>& other) : storage(other.incref()) {}

    template<class type>
    shared<type>& shared<type>::operator =(const shared<type>& other) {
        if (this == &other) return *this;
        if (storage == other.storage) return *this;
        decref();
        // XXX: incref() needs a mutable refcount...
        storage = other.incref();
        return *this;
    }

    template<class type>
    type& shared<type>::operator *() {
        check(storage != 0, "can't dereference a null ptr");
        return *storage->pointer;
    }

    template<class type>
    const type& shared<type>::operator *() const {
        check(storage != 0, "can't dereference a null ptr");
        return *storage->pointer;
    }

    template<class type>
    type* shared<type>::operator ->() {
        check(storage != 0, "can't dereference a null ptr");
        return storage->pointer;
    }

    template<class type>
    const type* shared<type>::operator ->() const {
        check(storage != 0, "can't dereference a null ptr");
        return storage->pointer;
    }

    template<class type>
    type* shared<type>::borrow() {
        return storage ? storage->pointer : 0;
    }

    template<class type>
    const type* shared<type>::borrow() const {
        return storage ? storage->pointer : 0;
    }

    template<class type>
    void shared<type>::decref() {
        if (storage) {
            if (--storage->refcount == 0) {
                delete storage->pointer;
                delete storage;
            }
        }
    }

    template<class type>
    typename shared<type>::counted* shared<type>::incref() {
        if (storage) ++storage->refcount;
        return storage;
    }

    template<class type> 
    void swap(shared<type>& flip, shared<type>& flop) {
        swap(flip.storage, flop.storage);
    }

}

#endif // XM_SHARED_H_

