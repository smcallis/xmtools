#ifndef XM_SHARED_H_
#define XM_SHARED_H_ 1

#include "basics.h"

namespace xm {

    //
    // This class holds on to a value using reference counting.
    // It is similar in spirit to std::shared_ptr<> in C++11,
    // but this works in C++98, and it's not just for use with
    // pointers.  It can hold values of any kind.
    // 
    // Some common examples:
    //
    // This stores a file descriptor.  It will be closed when
    // the last shared<int> is destructed:
    //     shared<int> fd(::open("/path/file.txt", O_RDONLY), ::close);
    // 
    // This stores an FFTW plan.  It will be destroyed when done:
    //     shared<fftwf_plan> plan(
    //         ::fftwf_plan_dft_1d(n, in, out, sign, flags),
    //         ::fftwf_destroy_plan
    //     );
    // 
    // This shares a pointer to an instance.  Since a cleanup
    // function isn't specified, the default cleanup function
    // will call operator delete on the pointer:
    //     struct foo { int bar; };
    //     shared<foo*> ptr(new foo);
    //

    template<class type>
    struct shared {

        ~shared();
        shared(const shared<type>& that);
        shared<type>& operator=(const shared<type>& that);

        // Constructs an empty shared value holder.
        shared();

        // Build a shared value with default cleanup function
        // Use this constructor when type is a pointer, and you
        // want it to be deleted when it's time to cleanup.
        shared(const type& val);

        // Build a shared value with a specific cleanup function.
        template<class cleanup>
        shared(const type& val, cleanup func);

        // Access the shared value.  This will throw an
        // exception if there is no current value.
        type& value();
        const type& value() const;

        // Asks if there is a current value
        bool valid() const;

        private:
            struct counted {
                virtual ~counted(){}
                counted(type data) :
                    refs(1), data(data) {}
                mutable int64 refs;
                type data;
            };

            template<class cleanup>
            struct specific : counted {
                specific(type data, cleanup finish) :
                    counted(data), finish(finish) {}
                virtual ~specific() {
                    finish(this->data);
                }
                cleanup finish;
            };

            static void deleter(type& val) {
                delete val;
            }

            void decref() const;
            counted* incref() const;

            mutable counted* ptr;

    };

    template<class type>
    shared<type>::~shared() {
        decref();
    }

    template<class type>
    shared<type>::shared() : ptr(0) {}

    template<class type>
    shared<type>::shared(const shared<type>& that) :
        ptr(that.incref()) {}

    template<class type>
    shared<type>& shared<type>::operator=(const shared<type>& that) {
        counted* copy = that.incref();
        decref();
        ptr = copy;
        return *this;
    }

    template<class type>
    shared<type>::shared(const type& val) :
        ptr(new specific<void (*)(type&)>(val, deleter)) {}

    template<class type>
    template<class cleanup>
    shared<type>::shared(const type& val, cleanup func) :
        ptr(new specific<cleanup>(val, func)) {}

    template<class type>
    type& shared<type>::value() {
        check(ptr != 0, "can't access empty shared value");
        return ptr->data;
    }

    template<class type>
    const type& shared<type>::value() const {
        check(ptr != 0, "can't access empty shared value");
        return ptr->data;
    }

    template<class type>
    bool shared<type>::valid() const {
        return ptr != 0;
    }

    template<class type>
    void shared<type>::decref() const {
        if (ptr && --ptr->refs == 0) {
            delete ptr;
            ptr = 0;
        }
    }

    template<class type>
    typename shared<type>::counted* shared<type>::incref() const {
        if (ptr) {
            ++ptr->refs;
            return ptr;
        }
        return 0;
    }

}

#endif // XM_SHARED_H_

