#ifndef XM_BASICS_H_
#define XM_BASICS_H_ 1

#include <time.h>
#include <stdio.h>
#include <stdarg.h>

#include "promote.h"

namespace xm {
    typedef long long int64;

    __attribute__ ((format (printf, 2, 3)))
    static inline void check(bool cond, const char* fmt, ...) {
        if (!cond) {
            struct failure : std::exception {
                char text[256];
                const char* what() const throw() {
                    return text;
                }
            } fail;

            va_list args;
            va_start(args, fmt);
            vsnprintf(fail.text, 256, fmt, args);
            va_end(args);
            throw fail;
        }
    }
    
    // A checked version of malloc
    template<class type>
    type* alloc(int64 extra=0) {
        int64 bytes = sizeof(type) + extra;
        void* memory = ::malloc(bytes);
        check(memory != 0, "allocating %lld bytes", bytes);
        return (type*)memory;
    }

    // A version of swap for when the STL is not included
    template<class type>
    void swap(type& flip, type& flop) {
         type temp = flip;
         flip = flop;
         flop = temp;
    }

    template<class atype, class btype>
    typename conditional<atype, btype>::type min(
        const atype& aa, const btype& bb
    ) { 
        return aa < bb ? aa : bb;
    }

    template<class atype, class btype, class ctype>
    typename conditional<atype, btype>::type min(
        const atype& aa, const btype& bb, const ctype& cc
    ) { 
        return min(aa, min(bb, cc));
    }

    template<class atype, class btype>
    typename conditional<atype, btype>::type max(
        const atype& aa, const btype& bb
    ) { 
        return bb < aa ? aa : bb;
    }

    template<class atype, class btype, class ctype>
    typename conditional<atype, btype>::type max(
        const atype& aa, const btype& bb, const ctype& cc
    ) { 
        return max(aa, max(bb, cc));
    }

    static inline double stopwatch() {
        struct timespec ts = { 0, 0 };
        clock_gettime(CLOCK_MONOTONIC, &ts);
        return ts.tv_sec + 1e-9*ts.tv_nsec;
    }

    template<class type>
    static type sqr(type xx) { return xx*xx; }

}

#endif // XM_BASICS_H_

