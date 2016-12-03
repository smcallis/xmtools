#ifndef XMTOOLS_H_
#define XMTOOLS_H_

// This file provides tools for working with X-Midas data and problems.  This
// library uses C++ 98 only but should be compatible with C++ 11 or later.

//{{{ includes

#include <new>
//#include <stdexcept>
#include <math.h>
#include <time.h>
#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <fcntl.h>
#include <stdarg.h>
#define __STDC_LIMIT_MACROS
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <regex.h>

// purely an aesthetic thing...
typedef long long int64;

//}}}

namespace xm {

    //{{{ Type Promotions:               conditional, arithmetic 
    //{{{ arithmetic
    template<class t0, class t1> struct arithmetic { struct type {}; };

    //
    // These are the "usual arithmetic conversions" according to the standard,
    // but note some of them are not ideal for retaining precision, and there
    // are some ugly conversions with signed and unsigned of the same size.
    //
    // C++ 11 has decltype(...) for this, but we're C++ 98/03 compatible.  Also
    // similar to common_type<...> in C++ 11.
    //
    // These were tested with both gcc and clang, and are part of xmtest.cc
    //
    
    template<> struct arithmetic<              char,               char> { typedef         signed int type; };
    template<> struct arithmetic<              char,        signed char> { typedef         signed int type; };
    template<> struct arithmetic<              char,      unsigned char> { typedef         signed int type; };
    template<> struct arithmetic<              char,       signed short> { typedef         signed int type; };
    template<> struct arithmetic<              char,     unsigned short> { typedef         signed int type; };
    template<> struct arithmetic<              char,         signed int> { typedef         signed int type; };
    template<> struct arithmetic<              char,       unsigned int> { typedef       unsigned int type; };
    template<> struct arithmetic<              char,        signed long> { typedef        signed long type; };
    template<> struct arithmetic<              char,      unsigned long> { typedef      unsigned long type; };
    template<> struct arithmetic<              char,   signed long long> { typedef   signed long long type; };
    template<> struct arithmetic<              char, unsigned long long> { typedef unsigned long long type; };
    template<> struct arithmetic<              char,              float> { typedef              float type; };
    template<> struct arithmetic<              char,             double> { typedef             double type; };
    template<> struct arithmetic<              char,        long double> { typedef        long double type; };

    template<> struct arithmetic<       signed char,               char> { typedef         signed int type; };
    template<> struct arithmetic<       signed char,        signed char> { typedef         signed int type; };
    template<> struct arithmetic<       signed char,      unsigned char> { typedef         signed int type; };
    template<> struct arithmetic<       signed char,       signed short> { typedef         signed int type; };
    template<> struct arithmetic<       signed char,     unsigned short> { typedef         signed int type; };
    template<> struct arithmetic<       signed char,         signed int> { typedef         signed int type; };
    template<> struct arithmetic<       signed char,       unsigned int> { typedef       unsigned int type; };
    template<> struct arithmetic<       signed char,        signed long> { typedef        signed long type; };
    template<> struct arithmetic<       signed char,      unsigned long> { typedef      unsigned long type; };
    template<> struct arithmetic<       signed char,   signed long long> { typedef   signed long long type; };
    template<> struct arithmetic<       signed char, unsigned long long> { typedef unsigned long long type; };
    template<> struct arithmetic<       signed char,              float> { typedef              float type; };
    template<> struct arithmetic<       signed char,             double> { typedef             double type; };
    template<> struct arithmetic<       signed char,        long double> { typedef        long double type; };

    template<> struct arithmetic<     unsigned char,               char> { typedef         signed int type; };
    template<> struct arithmetic<     unsigned char,        signed char> { typedef         signed int type; };
    template<> struct arithmetic<     unsigned char,      unsigned char> { typedef         signed int type; };
    template<> struct arithmetic<     unsigned char,       signed short> { typedef         signed int type; };
    template<> struct arithmetic<     unsigned char,     unsigned short> { typedef         signed int type; };
    template<> struct arithmetic<     unsigned char,         signed int> { typedef         signed int type; };
    template<> struct arithmetic<     unsigned char,       unsigned int> { typedef       unsigned int type; };
    template<> struct arithmetic<     unsigned char,        signed long> { typedef        signed long type; };
    template<> struct arithmetic<     unsigned char,      unsigned long> { typedef      unsigned long type; };
    template<> struct arithmetic<     unsigned char,   signed long long> { typedef   signed long long type; };
    template<> struct arithmetic<     unsigned char, unsigned long long> { typedef unsigned long long type; };
    template<> struct arithmetic<     unsigned char,              float> { typedef              float type; };
    template<> struct arithmetic<     unsigned char,             double> { typedef             double type; };
    template<> struct arithmetic<     unsigned char,        long double> { typedef        long double type; };

    template<> struct arithmetic<      signed short,               char> { typedef         signed int type; };
    template<> struct arithmetic<      signed short,        signed char> { typedef         signed int type; };
    template<> struct arithmetic<      signed short,      unsigned char> { typedef         signed int type; };
    template<> struct arithmetic<      signed short,       signed short> { typedef         signed int type; };
    template<> struct arithmetic<      signed short,     unsigned short> { typedef         signed int type; };
    template<> struct arithmetic<      signed short,         signed int> { typedef         signed int type; };
    template<> struct arithmetic<      signed short,       unsigned int> { typedef       unsigned int type; };
    template<> struct arithmetic<      signed short,        signed long> { typedef        signed long type; };
    template<> struct arithmetic<      signed short,      unsigned long> { typedef      unsigned long type; };
    template<> struct arithmetic<      signed short,   signed long long> { typedef   signed long long type; };
    template<> struct arithmetic<      signed short, unsigned long long> { typedef unsigned long long type; };
    template<> struct arithmetic<      signed short,              float> { typedef              float type; };
    template<> struct arithmetic<      signed short,             double> { typedef             double type; };
    template<> struct arithmetic<      signed short,        long double> { typedef        long double type; };

    template<> struct arithmetic<    unsigned short,               char> { typedef         signed int type; };
    template<> struct arithmetic<    unsigned short,        signed char> { typedef         signed int type; };
    template<> struct arithmetic<    unsigned short,      unsigned char> { typedef         signed int type; };
    template<> struct arithmetic<    unsigned short,       signed short> { typedef         signed int type; };
    template<> struct arithmetic<    unsigned short,     unsigned short> { typedef         signed int type; };
    template<> struct arithmetic<    unsigned short,         signed int> { typedef         signed int type; };
    template<> struct arithmetic<    unsigned short,       unsigned int> { typedef       unsigned int type; };
    template<> struct arithmetic<    unsigned short,        signed long> { typedef        signed long type; };
    template<> struct arithmetic<    unsigned short,      unsigned long> { typedef      unsigned long type; };
    template<> struct arithmetic<    unsigned short,   signed long long> { typedef   signed long long type; };
    template<> struct arithmetic<    unsigned short, unsigned long long> { typedef unsigned long long type; };
    template<> struct arithmetic<    unsigned short,              float> { typedef              float type; };
    template<> struct arithmetic<    unsigned short,             double> { typedef             double type; };
    template<> struct arithmetic<    unsigned short,        long double> { typedef        long double type; };

    template<> struct arithmetic<        signed int,               char> { typedef         signed int type; };
    template<> struct arithmetic<        signed int,        signed char> { typedef         signed int type; };
    template<> struct arithmetic<        signed int,      unsigned char> { typedef         signed int type; };
    template<> struct arithmetic<        signed int,       signed short> { typedef         signed int type; };
    template<> struct arithmetic<        signed int,     unsigned short> { typedef         signed int type; };
    template<> struct arithmetic<        signed int,         signed int> { typedef         signed int type; };
    template<> struct arithmetic<        signed int,       unsigned int> { typedef       unsigned int type; };
    template<> struct arithmetic<        signed int,        signed long> { typedef        signed long type; };
    template<> struct arithmetic<        signed int,      unsigned long> { typedef      unsigned long type; };
    template<> struct arithmetic<        signed int,   signed long long> { typedef   signed long long type; };
    template<> struct arithmetic<        signed int, unsigned long long> { typedef unsigned long long type; };
    template<> struct arithmetic<        signed int,              float> { typedef              float type; };
    template<> struct arithmetic<        signed int,             double> { typedef             double type; };
    template<> struct arithmetic<        signed int,        long double> { typedef        long double type; };

    template<> struct arithmetic<      unsigned int,               char> { typedef       unsigned int type; };
    template<> struct arithmetic<      unsigned int,        signed char> { typedef       unsigned int type; };
    template<> struct arithmetic<      unsigned int,      unsigned char> { typedef       unsigned int type; };
    template<> struct arithmetic<      unsigned int,       signed short> { typedef       unsigned int type; };
    template<> struct arithmetic<      unsigned int,     unsigned short> { typedef       unsigned int type; };
    template<> struct arithmetic<      unsigned int,         signed int> { typedef       unsigned int type; };
    template<> struct arithmetic<      unsigned int,       unsigned int> { typedef       unsigned int type; };
    template<> struct arithmetic<      unsigned int,        signed long> { typedef        signed long type; };
    template<> struct arithmetic<      unsigned int,      unsigned long> { typedef      unsigned long type; };
    template<> struct arithmetic<      unsigned int,   signed long long> { typedef   signed long long type; };
    template<> struct arithmetic<      unsigned int, unsigned long long> { typedef unsigned long long type; };
    template<> struct arithmetic<      unsigned int,              float> { typedef              float type; };
    template<> struct arithmetic<      unsigned int,             double> { typedef             double type; };
    template<> struct arithmetic<      unsigned int,        long double> { typedef        long double type; };

    template<> struct arithmetic<       signed long,               char> { typedef        signed long type; };
    template<> struct arithmetic<       signed long,        signed char> { typedef        signed long type; };
    template<> struct arithmetic<       signed long,      unsigned char> { typedef        signed long type; };
    template<> struct arithmetic<       signed long,       signed short> { typedef        signed long type; };
    template<> struct arithmetic<       signed long,     unsigned short> { typedef        signed long type; };
    template<> struct arithmetic<       signed long,         signed int> { typedef        signed long type; };
    template<> struct arithmetic<       signed long,       unsigned int> { typedef        signed long type; };
    template<> struct arithmetic<       signed long,        signed long> { typedef        signed long type; };
    template<> struct arithmetic<       signed long,      unsigned long> { typedef      unsigned long type; };
    template<> struct arithmetic<       signed long,   signed long long> { typedef   signed long long type; };
    template<> struct arithmetic<       signed long, unsigned long long> { typedef unsigned long long type; };
    template<> struct arithmetic<       signed long,              float> { typedef              float type; };
    template<> struct arithmetic<       signed long,             double> { typedef             double type; };
    template<> struct arithmetic<       signed long,        long double> { typedef        long double type; };

    template<> struct arithmetic<     unsigned long,               char> { typedef      unsigned long type; };
    template<> struct arithmetic<     unsigned long,        signed char> { typedef      unsigned long type; };
    template<> struct arithmetic<     unsigned long,      unsigned char> { typedef      unsigned long type; };
    template<> struct arithmetic<     unsigned long,       signed short> { typedef      unsigned long type; };
    template<> struct arithmetic<     unsigned long,     unsigned short> { typedef      unsigned long type; };
    template<> struct arithmetic<     unsigned long,         signed int> { typedef      unsigned long type; };
    template<> struct arithmetic<     unsigned long,       unsigned int> { typedef      unsigned long type; };
    template<> struct arithmetic<     unsigned long,        signed long> { typedef      unsigned long type; };
    template<> struct arithmetic<     unsigned long,      unsigned long> { typedef      unsigned long type; };
    template<> struct arithmetic<     unsigned long,   signed long long> { typedef unsigned long long type; };
    template<> struct arithmetic<     unsigned long, unsigned long long> { typedef unsigned long long type; };
    template<> struct arithmetic<     unsigned long,              float> { typedef              float type; };
    template<> struct arithmetic<     unsigned long,             double> { typedef             double type; };
    template<> struct arithmetic<     unsigned long,        long double> { typedef        long double type; };

    template<> struct arithmetic<  signed long long,               char> { typedef   signed long long type; };
    template<> struct arithmetic<  signed long long,        signed char> { typedef   signed long long type; };
    template<> struct arithmetic<  signed long long,      unsigned char> { typedef   signed long long type; };
    template<> struct arithmetic<  signed long long,       signed short> { typedef   signed long long type; };
    template<> struct arithmetic<  signed long long,     unsigned short> { typedef   signed long long type; };
    template<> struct arithmetic<  signed long long,         signed int> { typedef   signed long long type; };
    template<> struct arithmetic<  signed long long,       unsigned int> { typedef   signed long long type; };
    template<> struct arithmetic<  signed long long,        signed long> { typedef   signed long long type; };
    template<> struct arithmetic<  signed long long,      unsigned long> { typedef unsigned long long type; };
    template<> struct arithmetic<  signed long long,   signed long long> { typedef   signed long long type; };
    template<> struct arithmetic<  signed long long, unsigned long long> { typedef unsigned long long type; };
    template<> struct arithmetic<  signed long long,              float> { typedef              float type; };
    template<> struct arithmetic<  signed long long,             double> { typedef             double type; };
    template<> struct arithmetic<  signed long long,        long double> { typedef        long double type; };

    template<> struct arithmetic<unsigned long long,               char> { typedef unsigned long long type; };
    template<> struct arithmetic<unsigned long long,        signed char> { typedef unsigned long long type; };
    template<> struct arithmetic<unsigned long long,      unsigned char> { typedef unsigned long long type; };
    template<> struct arithmetic<unsigned long long,       signed short> { typedef unsigned long long type; };
    template<> struct arithmetic<unsigned long long,     unsigned short> { typedef unsigned long long type; };
    template<> struct arithmetic<unsigned long long,         signed int> { typedef unsigned long long type; };
    template<> struct arithmetic<unsigned long long,       unsigned int> { typedef unsigned long long type; };
    template<> struct arithmetic<unsigned long long,        signed long> { typedef unsigned long long type; };
    template<> struct arithmetic<unsigned long long,      unsigned long> { typedef unsigned long long type; };
    template<> struct arithmetic<unsigned long long,   signed long long> { typedef unsigned long long type; };
    template<> struct arithmetic<unsigned long long, unsigned long long> { typedef unsigned long long type; };
    template<> struct arithmetic<unsigned long long,              float> { typedef              float type; };
    template<> struct arithmetic<unsigned long long,             double> { typedef             double type; };
    template<> struct arithmetic<unsigned long long,        long double> { typedef        long double type; };

    template<> struct arithmetic<             float,               char> { typedef              float type; };
    template<> struct arithmetic<             float,        signed char> { typedef              float type; };
    template<> struct arithmetic<             float,      unsigned char> { typedef              float type; };
    template<> struct arithmetic<             float,       signed short> { typedef              float type; };
    template<> struct arithmetic<             float,     unsigned short> { typedef              float type; };
    template<> struct arithmetic<             float,         signed int> { typedef              float type; };
    template<> struct arithmetic<             float,       unsigned int> { typedef              float type; };
    template<> struct arithmetic<             float,        signed long> { typedef              float type; };
    template<> struct arithmetic<             float,      unsigned long> { typedef              float type; };
    template<> struct arithmetic<             float,   signed long long> { typedef              float type; };
    template<> struct arithmetic<             float, unsigned long long> { typedef              float type; };
    template<> struct arithmetic<             float,              float> { typedef              float type; };
    template<> struct arithmetic<             float,             double> { typedef             double type; };
    template<> struct arithmetic<             float,        long double> { typedef        long double type; };

    template<> struct arithmetic<            double,               char> { typedef             double type; };
    template<> struct arithmetic<            double,        signed char> { typedef             double type; };
    template<> struct arithmetic<            double,      unsigned char> { typedef             double type; };
    template<> struct arithmetic<            double,       signed short> { typedef             double type; };
    template<> struct arithmetic<            double,     unsigned short> { typedef             double type; };
    template<> struct arithmetic<            double,         signed int> { typedef             double type; };
    template<> struct arithmetic<            double,       unsigned int> { typedef             double type; };
    template<> struct arithmetic<            double,        signed long> { typedef             double type; };
    template<> struct arithmetic<            double,      unsigned long> { typedef             double type; };
    template<> struct arithmetic<            double,   signed long long> { typedef             double type; };
    template<> struct arithmetic<            double, unsigned long long> { typedef             double type; };
    template<> struct arithmetic<            double,              float> { typedef             double type; };
    template<> struct arithmetic<            double,             double> { typedef             double type; };
    template<> struct arithmetic<            double,        long double> { typedef        long double type; };

    template<> struct arithmetic<       long double,               char> { typedef        long double type; };
    template<> struct arithmetic<       long double,        signed char> { typedef        long double type; };
    template<> struct arithmetic<       long double,      unsigned char> { typedef        long double type; };
    template<> struct arithmetic<       long double,       signed short> { typedef        long double type; };
    template<> struct arithmetic<       long double,     unsigned short> { typedef        long double type; };
    template<> struct arithmetic<       long double,         signed int> { typedef        long double type; };
    template<> struct arithmetic<       long double,       unsigned int> { typedef        long double type; };
    template<> struct arithmetic<       long double,        signed long> { typedef        long double type; };
    template<> struct arithmetic<       long double,      unsigned long> { typedef        long double type; };
    template<> struct arithmetic<       long double,   signed long long> { typedef        long double type; };
    template<> struct arithmetic<       long double, unsigned long long> { typedef        long double type; };
    template<> struct arithmetic<       long double,              float> { typedef        long double type; };
    template<> struct arithmetic<       long double,             double> { typedef        long double type; };
    template<> struct arithmetic<       long double,        long double> { typedef        long double type; };
    //}}}
    //{{{ conditional
    template<class t0, class t1> struct conditional { struct type {}; };

    //
    // These are the conversions that result from the ternary (conditional)
    // operator.  We really only need these for min() and max() to be correct.
    //
    // These were tested with both gcc and clang, and are part of xmtest.cc
    //
    
    template<> struct conditional<              char,               char> { typedef               char type; };
    template<> struct conditional<              char,        signed char> { typedef         signed int type; };
    template<> struct conditional<              char,      unsigned char> { typedef         signed int type; };
    template<> struct conditional<              char,       signed short> { typedef         signed int type; };
    template<> struct conditional<              char,     unsigned short> { typedef         signed int type; };
    template<> struct conditional<              char,         signed int> { typedef         signed int type; };
    template<> struct conditional<              char,       unsigned int> { typedef       unsigned int type; };
    template<> struct conditional<              char,        signed long> { typedef        signed long type; };
    template<> struct conditional<              char,      unsigned long> { typedef      unsigned long type; };
    template<> struct conditional<              char,   signed long long> { typedef   signed long long type; };
    template<> struct conditional<              char, unsigned long long> { typedef unsigned long long type; };
    template<> struct conditional<              char,              float> { typedef              float type; };
    template<> struct conditional<              char,             double> { typedef             double type; };
    template<> struct conditional<              char,        long double> { typedef        long double type; };

    template<> struct conditional<       signed char,               char> { typedef         signed int type; };
    template<> struct conditional<       signed char,        signed char> { typedef        signed char type; };
    template<> struct conditional<       signed char,      unsigned char> { typedef         signed int type; };
    template<> struct conditional<       signed char,       signed short> { typedef         signed int type; };
    template<> struct conditional<       signed char,     unsigned short> { typedef         signed int type; };
    template<> struct conditional<       signed char,         signed int> { typedef         signed int type; };
    template<> struct conditional<       signed char,       unsigned int> { typedef       unsigned int type; };
    template<> struct conditional<       signed char,        signed long> { typedef        signed long type; };
    template<> struct conditional<       signed char,      unsigned long> { typedef      unsigned long type; };
    template<> struct conditional<       signed char,   signed long long> { typedef   signed long long type; };
    template<> struct conditional<       signed char, unsigned long long> { typedef unsigned long long type; };
    template<> struct conditional<       signed char,              float> { typedef              float type; };
    template<> struct conditional<       signed char,             double> { typedef             double type; };
    template<> struct conditional<       signed char,        long double> { typedef        long double type; };

    template<> struct conditional<     unsigned char,               char> { typedef         signed int type; };
    template<> struct conditional<     unsigned char,        signed char> { typedef         signed int type; };
    template<> struct conditional<     unsigned char,      unsigned char> { typedef      unsigned char type; };
    template<> struct conditional<     unsigned char,       signed short> { typedef         signed int type; };
    template<> struct conditional<     unsigned char,     unsigned short> { typedef         signed int type; };
    template<> struct conditional<     unsigned char,         signed int> { typedef         signed int type; };
    template<> struct conditional<     unsigned char,       unsigned int> { typedef       unsigned int type; };
    template<> struct conditional<     unsigned char,        signed long> { typedef        signed long type; };
    template<> struct conditional<     unsigned char,      unsigned long> { typedef      unsigned long type; };
    template<> struct conditional<     unsigned char,   signed long long> { typedef   signed long long type; };
    template<> struct conditional<     unsigned char, unsigned long long> { typedef unsigned long long type; };
    template<> struct conditional<     unsigned char,              float> { typedef              float type; };
    template<> struct conditional<     unsigned char,             double> { typedef             double type; };
    template<> struct conditional<     unsigned char,        long double> { typedef        long double type; };

    template<> struct conditional<      signed short,               char> { typedef         signed int type; };
    template<> struct conditional<      signed short,        signed char> { typedef         signed int type; };
    template<> struct conditional<      signed short,      unsigned char> { typedef         signed int type; };
    template<> struct conditional<      signed short,       signed short> { typedef       signed short type; };
    template<> struct conditional<      signed short,     unsigned short> { typedef         signed int type; };
    template<> struct conditional<      signed short,         signed int> { typedef         signed int type; };
    template<> struct conditional<      signed short,       unsigned int> { typedef       unsigned int type; };
    template<> struct conditional<      signed short,        signed long> { typedef        signed long type; };
    template<> struct conditional<      signed short,      unsigned long> { typedef      unsigned long type; };
    template<> struct conditional<      signed short,   signed long long> { typedef   signed long long type; };
    template<> struct conditional<      signed short, unsigned long long> { typedef unsigned long long type; };
    template<> struct conditional<      signed short,              float> { typedef              float type; };
    template<> struct conditional<      signed short,             double> { typedef             double type; };
    template<> struct conditional<      signed short,        long double> { typedef        long double type; };

    template<> struct conditional<    unsigned short,               char> { typedef         signed int type; };
    template<> struct conditional<    unsigned short,        signed char> { typedef         signed int type; };
    template<> struct conditional<    unsigned short,      unsigned char> { typedef         signed int type; };
    template<> struct conditional<    unsigned short,       signed short> { typedef         signed int type; };
    template<> struct conditional<    unsigned short,     unsigned short> { typedef     unsigned short type; };
    template<> struct conditional<    unsigned short,         signed int> { typedef         signed int type; };
    template<> struct conditional<    unsigned short,       unsigned int> { typedef       unsigned int type; };
    template<> struct conditional<    unsigned short,        signed long> { typedef        signed long type; };
    template<> struct conditional<    unsigned short,      unsigned long> { typedef      unsigned long type; };
    template<> struct conditional<    unsigned short,   signed long long> { typedef   signed long long type; };
    template<> struct conditional<    unsigned short, unsigned long long> { typedef unsigned long long type; };
    template<> struct conditional<    unsigned short,              float> { typedef              float type; };
    template<> struct conditional<    unsigned short,             double> { typedef             double type; };
    template<> struct conditional<    unsigned short,        long double> { typedef        long double type; };

    template<> struct conditional<        signed int,               char> { typedef         signed int type; };
    template<> struct conditional<        signed int,        signed char> { typedef         signed int type; };
    template<> struct conditional<        signed int,      unsigned char> { typedef         signed int type; };
    template<> struct conditional<        signed int,       signed short> { typedef         signed int type; };
    template<> struct conditional<        signed int,     unsigned short> { typedef         signed int type; };
    template<> struct conditional<        signed int,         signed int> { typedef         signed int type; };
    template<> struct conditional<        signed int,       unsigned int> { typedef       unsigned int type; };
    template<> struct conditional<        signed int,        signed long> { typedef        signed long type; };
    template<> struct conditional<        signed int,      unsigned long> { typedef      unsigned long type; };
    template<> struct conditional<        signed int,   signed long long> { typedef   signed long long type; };
    template<> struct conditional<        signed int, unsigned long long> { typedef unsigned long long type; };
    template<> struct conditional<        signed int,              float> { typedef              float type; };
    template<> struct conditional<        signed int,             double> { typedef             double type; };
    template<> struct conditional<        signed int,        long double> { typedef        long double type; };

    template<> struct conditional<      unsigned int,               char> { typedef       unsigned int type; };
    template<> struct conditional<      unsigned int,        signed char> { typedef       unsigned int type; };
    template<> struct conditional<      unsigned int,      unsigned char> { typedef       unsigned int type; };
    template<> struct conditional<      unsigned int,       signed short> { typedef       unsigned int type; };
    template<> struct conditional<      unsigned int,     unsigned short> { typedef       unsigned int type; };
    template<> struct conditional<      unsigned int,         signed int> { typedef       unsigned int type; };
    template<> struct conditional<      unsigned int,       unsigned int> { typedef       unsigned int type; };
    template<> struct conditional<      unsigned int,        signed long> { typedef        signed long type; };
    template<> struct conditional<      unsigned int,      unsigned long> { typedef      unsigned long type; };
    template<> struct conditional<      unsigned int,   signed long long> { typedef   signed long long type; };
    template<> struct conditional<      unsigned int, unsigned long long> { typedef unsigned long long type; };
    template<> struct conditional<      unsigned int,              float> { typedef              float type; };
    template<> struct conditional<      unsigned int,             double> { typedef             double type; };
    template<> struct conditional<      unsigned int,        long double> { typedef        long double type; };

    template<> struct conditional<       signed long,               char> { typedef        signed long type; };
    template<> struct conditional<       signed long,        signed char> { typedef        signed long type; };
    template<> struct conditional<       signed long,      unsigned char> { typedef        signed long type; };
    template<> struct conditional<       signed long,       signed short> { typedef        signed long type; };
    template<> struct conditional<       signed long,     unsigned short> { typedef        signed long type; };
    template<> struct conditional<       signed long,         signed int> { typedef        signed long type; };
    template<> struct conditional<       signed long,       unsigned int> { typedef        signed long type; };
    template<> struct conditional<       signed long,        signed long> { typedef        signed long type; };
    template<> struct conditional<       signed long,      unsigned long> { typedef      unsigned long type; };
    template<> struct conditional<       signed long,   signed long long> { typedef   signed long long type; };
    template<> struct conditional<       signed long, unsigned long long> { typedef unsigned long long type; };
    template<> struct conditional<       signed long,              float> { typedef              float type; };
    template<> struct conditional<       signed long,             double> { typedef             double type; };
    template<> struct conditional<       signed long,        long double> { typedef        long double type; };

    template<> struct conditional<     unsigned long,               char> { typedef      unsigned long type; };
    template<> struct conditional<     unsigned long,        signed char> { typedef      unsigned long type; };
    template<> struct conditional<     unsigned long,      unsigned char> { typedef      unsigned long type; };
    template<> struct conditional<     unsigned long,       signed short> { typedef      unsigned long type; };
    template<> struct conditional<     unsigned long,     unsigned short> { typedef      unsigned long type; };
    template<> struct conditional<     unsigned long,         signed int> { typedef      unsigned long type; };
    template<> struct conditional<     unsigned long,       unsigned int> { typedef      unsigned long type; };
    template<> struct conditional<     unsigned long,        signed long> { typedef      unsigned long type; };
    template<> struct conditional<     unsigned long,      unsigned long> { typedef      unsigned long type; };
    template<> struct conditional<     unsigned long,   signed long long> { typedef unsigned long long type; };
    template<> struct conditional<     unsigned long, unsigned long long> { typedef unsigned long long type; };
    template<> struct conditional<     unsigned long,              float> { typedef              float type; };
    template<> struct conditional<     unsigned long,             double> { typedef             double type; };
    template<> struct conditional<     unsigned long,        long double> { typedef        long double type; };

    template<> struct conditional<  signed long long,               char> { typedef   signed long long type; };
    template<> struct conditional<  signed long long,        signed char> { typedef   signed long long type; };
    template<> struct conditional<  signed long long,      unsigned char> { typedef   signed long long type; };
    template<> struct conditional<  signed long long,       signed short> { typedef   signed long long type; };
    template<> struct conditional<  signed long long,     unsigned short> { typedef   signed long long type; };
    template<> struct conditional<  signed long long,         signed int> { typedef   signed long long type; };
    template<> struct conditional<  signed long long,       unsigned int> { typedef   signed long long type; };
    template<> struct conditional<  signed long long,        signed long> { typedef   signed long long type; };
    template<> struct conditional<  signed long long,      unsigned long> { typedef unsigned long long type; };
    template<> struct conditional<  signed long long,   signed long long> { typedef   signed long long type; };
    template<> struct conditional<  signed long long, unsigned long long> { typedef unsigned long long type; };
    template<> struct conditional<  signed long long,              float> { typedef              float type; };
    template<> struct conditional<  signed long long,             double> { typedef             double type; };
    template<> struct conditional<  signed long long,        long double> { typedef        long double type; };

    template<> struct conditional<unsigned long long,               char> { typedef unsigned long long type; };
    template<> struct conditional<unsigned long long,        signed char> { typedef unsigned long long type; };
    template<> struct conditional<unsigned long long,      unsigned char> { typedef unsigned long long type; };
    template<> struct conditional<unsigned long long,       signed short> { typedef unsigned long long type; };
    template<> struct conditional<unsigned long long,     unsigned short> { typedef unsigned long long type; };
    template<> struct conditional<unsigned long long,         signed int> { typedef unsigned long long type; };
    template<> struct conditional<unsigned long long,       unsigned int> { typedef unsigned long long type; };
    template<> struct conditional<unsigned long long,        signed long> { typedef unsigned long long type; };
    template<> struct conditional<unsigned long long,      unsigned long> { typedef unsigned long long type; };
    template<> struct conditional<unsigned long long,   signed long long> { typedef unsigned long long type; };
    template<> struct conditional<unsigned long long, unsigned long long> { typedef unsigned long long type; };
    template<> struct conditional<unsigned long long,              float> { typedef              float type; };
    template<> struct conditional<unsigned long long,             double> { typedef             double type; };
    template<> struct conditional<unsigned long long,        long double> { typedef        long double type; };

    template<> struct conditional<             float,               char> { typedef              float type; };
    template<> struct conditional<             float,        signed char> { typedef              float type; };
    template<> struct conditional<             float,      unsigned char> { typedef              float type; };
    template<> struct conditional<             float,       signed short> { typedef              float type; };
    template<> struct conditional<             float,     unsigned short> { typedef              float type; };
    template<> struct conditional<             float,         signed int> { typedef              float type; };
    template<> struct conditional<             float,       unsigned int> { typedef              float type; };
    template<> struct conditional<             float,        signed long> { typedef              float type; };
    template<> struct conditional<             float,      unsigned long> { typedef              float type; };
    template<> struct conditional<             float,   signed long long> { typedef              float type; };
    template<> struct conditional<             float, unsigned long long> { typedef              float type; };
    template<> struct conditional<             float,              float> { typedef              float type; };
    template<> struct conditional<             float,             double> { typedef             double type; };
    template<> struct conditional<             float,        long double> { typedef        long double type; };

    template<> struct conditional<            double,               char> { typedef             double type; };
    template<> struct conditional<            double,        signed char> { typedef             double type; };
    template<> struct conditional<            double,      unsigned char> { typedef             double type; };
    template<> struct conditional<            double,       signed short> { typedef             double type; };
    template<> struct conditional<            double,     unsigned short> { typedef             double type; };
    template<> struct conditional<            double,         signed int> { typedef             double type; };
    template<> struct conditional<            double,       unsigned int> { typedef             double type; };
    template<> struct conditional<            double,        signed long> { typedef             double type; };
    template<> struct conditional<            double,      unsigned long> { typedef             double type; };
    template<> struct conditional<            double,   signed long long> { typedef             double type; };
    template<> struct conditional<            double, unsigned long long> { typedef             double type; };
    template<> struct conditional<            double,              float> { typedef             double type; };
    template<> struct conditional<            double,             double> { typedef             double type; };
    template<> struct conditional<            double,        long double> { typedef        long double type; };

    template<> struct conditional<       long double,               char> { typedef        long double type; };
    template<> struct conditional<       long double,        signed char> { typedef        long double type; };
    template<> struct conditional<       long double,      unsigned char> { typedef        long double type; };
    template<> struct conditional<       long double,       signed short> { typedef        long double type; };
    template<> struct conditional<       long double,     unsigned short> { typedef        long double type; };
    template<> struct conditional<       long double,         signed int> { typedef        long double type; };
    template<> struct conditional<       long double,       unsigned int> { typedef        long double type; };
    template<> struct conditional<       long double,        signed long> { typedef        long double type; };
    template<> struct conditional<       long double,      unsigned long> { typedef        long double type; };
    template<> struct conditional<       long double,   signed long long> { typedef        long double type; };
    template<> struct conditional<       long double, unsigned long long> { typedef        long double type; };
    template<> struct conditional<       long double,              float> { typedef        long double type; };
    template<> struct conditional<       long double,             double> { typedef        long double type; };
    template<> struct conditional<       long double,        long double> { typedef        long double type; };

    //}}}
    //}}}
    //{{{ Fundamental Functions:         check, alloc, swap, hash, max, min 

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

    // MurmurHash64A from Austin Appleby.  This was copied from a public
    // domain version of the code, and modified slightly for formatting.
    static inline size_t hash(
        const void *ptr, int64 len, unsigned long long seed
    ) {
        typedef unsigned char uint8_t;
        typedef unsigned long long uint64_t;
        const uint64_t mult = 0xC6A4A7935BD1E995ull;
        uint64_t code = seed ^ len*mult;

        const uint64_t* words = (const uint64_t*)ptr;
        for (int64 ii = 0; ii<len/8; ii++) {
            uint64_t word = *words++;
            word *= mult; 
            word ^= word >> 47; 
            word *= mult; 
            code ^= word;
            code *= mult; 
        }

        const uint8_t *bytes = (const uint8_t*)words;
        switch (len & 7) {
            case 7: code ^= (uint64_t)bytes[6] << 48;
            case 6: code ^= (uint64_t)bytes[5] << 40;
            case 5: code ^= (uint64_t)bytes[4] << 32;
            case 4: code ^= (uint64_t)bytes[3] << 24;
            case 3: code ^= (uint64_t)bytes[2] << 16;
            case 2: code ^= (uint64_t)bytes[1] <<  8;
            case 1: code ^= (uint64_t)bytes[0];
            code *= mult;
        };

        code ^= code >> 47;
        code *= mult;
        code ^= code >> 47;

        return code;
    } 

    size_t hash(char     val) { return hash(&val, sizeof(char    ), 1); }
    size_t hash(int8_t   val) { return hash(&val, sizeof(int8_t  ), 1); }
    size_t hash(int16_t  val) { return hash(&val, sizeof(int16_t ), 1); }
    size_t hash(int32_t  val) { return hash(&val, sizeof(int32_t ), 1); }
    size_t hash(int64_t  val) { return hash(&val, sizeof(int64_t ), 1); }
    size_t hash(uint8_t  val) { return hash(&val, sizeof(uint8_t ), 1); }
    size_t hash(uint16_t val) { return hash(&val, sizeof(uint16_t), 1); }
    size_t hash(uint32_t val) { return hash(&val, sizeof(uint32_t), 1); }
    size_t hash(uint64_t val) { return hash(&val, sizeof(uint64_t), 1); }
    size_t hash(int64    val) { return hash(&val, sizeof(int64   ), 1); }

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
    //}}}
    //{{{ Basic Containers:              list, dict, shared 
    //{{{ list 
    template<class type>
    struct list {
        ~list();
        list();
        template<class tt>
        list(const list<tt>& other);
        list(const list<type>& other);

        template<class tt>
        list<type>& operator =(const list<tt>&);
        list<type>& operator =(const list<type>&);

        type& operator [](int64 index);
        const type& operator [](int64 index) const;

        void append(const type& value);
        void insert(int64 index, const type& value);
        void remove(int64 index);

        template<class tt>
        void extend(const list<tt>& other);

        void reserve(int64 count);
        void shrink();
        void clear();

        type* data();
        const type* data() const;
        int64 size() const;

        private:
            const type* contiguous() const;

            void realloc(int64 capacity);
            type* address(int64 index) const;

            template<class tt>
            void assign(const list<tt>& other);

            template<class tt> friend struct list;
            template<class tt> friend void swap(
                list<tt>& flip, list<tt>& flop
            );

            mutable struct implementation {
                int64 offset, quantity, capacity;
                type bucket[1];
            } *storage;
    };

    template<class type>
    list<type>::~list() { clear(); }

    template<class type>
    list<type>::list() : storage(0) {}

    template<class type>
    list<type>::list(const list<type>& other) : storage(0) {
        assign(other);
    }

    template<class type>
    template<class tt>
    list<type>::list(const list<tt>& other) : storage(0) {
        assign(other);
    }

    template<class type>
    list<type>& list<type>::operator =(const list<type>& other) {
        if (this == &other) return *this;
        clear();
        assign(other);
        return *this;
    }

    template<class type>
    template<class tt>
    list<type>& list<type>::operator =(const list<tt>& other) {
        clear();
        assign(other);
        return *this;
    }

    template<class type>
    type& list<type>::operator [](const int64 index) {
        return *address(index);
    }

    template<class type>
    const type& list<type>::operator [](const int64 index) const {
        return *address(index);
    }

    template<class type>
    void list<type>::append(const type& value) {
        if (!storage || storage->quantity >= storage->capacity) {
            realloc(storage ? 2*storage->capacity : 8);
        }
        new(address(storage->quantity++)) type(value);
    }

    template<class type>
    void list<type>::insert(int64 index, const type& value) {
        const int64 len = size();
        check(0 <= index && index <= len, "index in [0, %lld]", len);
        if (storage && len < storage->capacity) {
            if (index < len/2) {
                // shuffle the early elements back to make room
                type* dst = address(-1);
                new(dst) type(value);
                for (int64 ii = 0; ii<index; ++ii) {
                    type* src = (type*)address(ii);
                    swap(*dst, *src);
                    dst = src;
                }
                storage->offset -= 1;
                if (storage->offset < 0) {
                    storage->offset += storage->capacity;
                }
            } else {
                // shuffle the later elements forward to make room
                type* dst = address(len);
                new(dst) type(value);
                for (int64 ii = len - 1; ii>=index; --ii) {
                    type* src = address(ii);
                    swap(*dst, *src);
                    dst = src;
                }
            }
        } else {
            // special version of resize, swapping elements only once
            int64 newsize = storage ? 2*storage->capacity : 8;
            implementation* larger = alloc<implementation>(
                (newsize - 1)*sizeof(type)
            );
            larger->offset = 0;
            larger->quantity = len;
            larger->capacity = newsize;
            for (int64 ii = 0; ii<index; ++ii) {
                type* dst = larger->bucket + ii;
                new(dst) type;
                type* src = address(ii);
                swap(*dst, *src);
                src->~type();
            }
            new(larger->bucket + index) type(value);
            for (int64 ii = index; ii<len; ++ii) {
                type* dst = larger->bucket + ii + 1;
                new(dst) type;
                type* src = address(ii);
                swap(*src, *dst);
                src->~type();
            }
            if (storage) free(storage);
            storage = larger;
        }
        storage->quantity++;
    }

    template<class type>
    void list<type>::remove(int64 index) {
        const int64 len = size();
        check(0 <= index && index < len, "index in [0, %lld)", len);
        if (index < len/2) {
            // shuffle the early elements forward
            for (int64 ii = index; ii>0; --ii) {
                swap(*address(ii - 1), *address(ii));
            }
            address(0)->~type();
            ++storage->offset;
            --storage->quantity;
        } else {
            // shuffle the later elements backward
            for (int64 ii = index; ii<len - 1; ++ii) {
                swap(*address(ii), *address(ii + 1));
            }
            address(len - 1)->~type();
            --storage->quantity;
        }
    }

    template<class type>
    template<class tt>
    void list<type>::extend(const list<tt>& other) {
        const int64 len = size();
        const int64 count = other.size();
        if (len + count) reserve(len + count);
        for (int64 ii = 0; ii<count; ++ii) {
            new (address(len + ii)) type(other[ii]);
        }
        if (storage) storage->quantity += count;
    }

    template<class type>
    void list<type>::reserve(int64 count) {
        check(count > 0, "must reserve a positive count: %lld", count);
        int64 needed = 8; // must be power of 2
        while (needed < count) needed *= 2;
        if (!storage || storage->capacity < needed) realloc(needed);
    }

    template<class type>
    void list<type>::shrink() {
        if (!storage) return;
        const int64 len = size();
        if (len == 0) {
            free(storage);
            storage = 0;
            return;
        }
        int64 newsize = 4;
        while (newsize < len) newsize *= 2;
        if (newsize >= storage->capacity) return;
        implementation* smaller = alloc<implementation>(
            (newsize - 1)*sizeof(type)
        );
        for (int64 ii = 0; ii<len; ++ii) {
            type* dst = smaller->bucket + ii;
            type* src = address(ii);
            new(dst) type;
            swap(*dst, *src);
            src->~type();
        }
        free(storage);
        smaller->offset = 0;
        smaller->quantity = len;
        smaller->capacity = newsize;
        storage = smaller;
    }

    template<class type>
    void list<type>::clear() {
        if (storage) {
            for (int64 ii = 0; ii<storage->quantity; ++ii) {
                type* ptr = address(ii);
                ptr->~type();
            }
            free(storage);
            storage = 0;
        }
    }

    template<class type>
    type* list<type>::data() {
        return (type*)contiguous();
    }

    template<class type>
    const type* list<type>::data() const {
        return contiguous();
    }

    template<class type>
    const type* list<type>::contiguous() const {
        if (!storage || storage->quantity == 0) return 0;
        const int64 len = size();
        if (storage->offset + len <= storage->capacity) {
            return storage->bucket + storage->offset;
        }

        // Shuffle the head data into the whole using swaps
        // [---- tail ----][--- hole ---][----- head -----]
        int64 head = storage->capacity - storage->offset;
        int64 tail = storage->quantity - head;
        int64 hole = storage->capacity - storage->quantity;
        int64 temp = xm::min(hole, head);
        for (int64 ii = 0; ii<temp; ii++) {
            new(storage->bucket + tail + ii) type;
        }
        for (int64 ii = 0; ii<head; ii++) {
            swap(
                storage->bucket[tail + ii],
                storage->bucket[storage->offset + ii]
            );
        }
        // [---- tail ----][----- head -----][--- hole ---]
        for (int64 ii = 0; ii<temp; ii++) {
            storage->bucket[storage->capacity - 1 - ii].~type();
        }

        // Rotate the data using the "Juggling" algorithm
        int64 gcd = len;
        int64 tmp = tail;
        while (tmp) {
            int64 tt = gcd%tmp;
            gcd = tmp;
            tmp = tt;
        }

        int64 cyc = len/gcd - 1;
        for (int64 ii = 0; ii<gcd; ii++) {
            int64 lo = ii;
            int64 hi = ii + tail;
            for (int64 kk = 0; kk<cyc; kk++) {
                swap(storage->bucket[lo], storage->bucket[hi]);
                lo = hi; hi += tail;
                if (hi >= len) hi -= len;
            }
        }

        storage->offset = 0;
        return storage->bucket;
    }

    template<class type>
    int64 list<type>::size() const {
        return storage ? storage->quantity : 0;
    }

    template<class type>
    void list<type>::realloc(int64 capacity) {
        implementation* changed = alloc<implementation>(
            (capacity - 1)*sizeof(type)
        );
        changed->offset = 0;
        changed->quantity = 0;
        changed->capacity = capacity;
        if (storage) {
            for (int64 ii = 0; ii<storage->quantity; ++ii) {
                new(changed->bucket + ii) type;
                type* old = address(ii);
                swap(changed->bucket[ii], *old);
                old->~type();
            }
            changed->quantity = storage->quantity;
            free(storage);
        }
        storage = changed;
    }

    template<class type>
    type* list<type>::address(int64 index) const {
        // circular indexing, requires capacity is power of 2
        return storage->bucket + (
            (storage->offset + index)&(storage->capacity - 1)
        );
    }

    template<class type>
    template<class tt>
    void list<type>::assign(const list<tt>& other) {
        if (other.storage) {
            const int64 len = other.storage->quantity;
            const int64 cap = other.storage->capacity;
            storage = alloc<implementation>((cap - 1)*sizeof(type));
            storage->offset = 0;
            storage->quantity = len;
            storage->capacity = cap;
            for (int64 ii = 0; ii<len; ++ii) {
                new(address(ii)) type(other[ii]);
            }
        }
    }

    template<class type>
    void swap(list<type>& flip, list<type>& flop) {
        swap(flip.storage, flop.storage);
    }
    //}}}
    //{{{ dict 
    // A sentinel type for dict sets with only keys (no vals)
    struct none {};

    template<class ktype, class vtype=none>
    struct dict {
        ~dict();
        dict();
        dict(const dict<ktype, vtype>& other);

        template<class kk, class vv>
        dict(const dict<kk, vv>& other);

        dict<ktype, vtype>& operator =(
            const dict<ktype, vtype>& other
        );

        template<class kk, class vv>
        dict<ktype, vtype>& operator =(
            const dict<kk, vv>& other
        );

        vtype& operator [](const ktype& key);

        void insert(const ktype& key);
        void insert(const ktype& key, const vtype& val);
        void remove(const ktype& key);
        bool haskey(const ktype& key) const;

        void reserve(int64 count);
        void shrink();
        void clear();
        int64 size() const;

        list<ktype> keys() const;
        list<vtype> vals() const;

        // Iterating through all keys and vals:
        //
        //    for (int64 ii = 0; ii<dd.bins(); ii++) {
        //        // must skip empty bins
        //        if (dd.skip(ii)) continue;
        //
        //        foo(dd.key(ii));
        //        bar(dd.val(ii));
        //
        //        // can also assign to dd.val(ii)
        //        dd.val(ii) = hmm;    
        //    }
        //
        int64 bins() const;
        bool skip(int64 index) const;
        const ktype& key(int64 index) const;
        const vtype& val(int64 index) const;
        vtype& val(int64 index);

        void debug() const;

        private:
            // general purpose implemenation
            template<class keytype, class valtype>
            struct keyval {
                size_t code;
                keytype key_;
                valtype val_;
                keytype& getkey() { return key_; }
                valtype& getval() { return val_; }
                const keytype& getkey() const { return key_; }
                const valtype& getval() const { return val_; }
                void setval(const valtype& vv) { val_ = vv; }
                keyval() {}
                keyval(
                    size_t cc,
                    const keytype& kk, 
                    const valtype& vv
                ) : code(cc), key_(kk), val_(vv) {}
                ~keyval() { code = 0; }
            };

            // specialization for vtype none
            template<class keytype>
            struct keyval<keytype, none> {
                size_t code;
                keytype key_;
                keytype& getkey() { return key_; }
                none& getval() {
                    static none nil;
                    return nil;
                }
                const keytype& getkey() const { return key_; }
                const none& getval() const {
                    static none nil;
                    return nil;
                }
                void setval(const none& vv) { (void)vv; }
                keyval() {}
                keyval(
                    size_t cc,
                    const keytype& kk, 
                    const none&
                ) : code(cc), key_(kk) {}
                ~keyval() { code = 0; }
            };

            template<class keytype, class valtype>
            static void trade(keyval<keytype, valtype>& flip, keyval<keytype, valtype>& flop) {
                swap(flip.code, flop.code);
                swap(flip.key_, flop.key_);
                swap(flip.val_, flop.val_);
            }

            template<class keytype>
            static void trade(keyval<keytype, none>& flip, keyval<keytype, none>& flop) {
                swap(flip.code, flop.code);
                swap(flip.key_, flop.key_);
            }

            enum { HIGHBIT = (((size_t)1) << (8*sizeof(size_t) - 1)) };
            template<class kk, class vv>
            void assign(const dict<kk, vv>& other);

            void rehash(const int64 capacity);
            int64 inject(const ktype& key, const vtype* ptr, size_t hh);
            // using Robin Hood collision resolution, searching, and backshifting
            void robinhood(
                int64 index, keyval<ktype, vtype>& kv, int64 cost
            );
            int64 search(const ktype& key, size_t hh) const;
            void backshift(int64 index);

            template<class kk, class vv> friend struct dict;
            //template<class kk, class vv> friend struct iter;
            template<class kk, class vv> friend void swap(
                dict<kk, vv>& flip, dict<kk, vv>&flop
            );

            struct implementation {
                int64 quantity, capacity;
                keyval<ktype, vtype> bucket[1];
            } *storage;
    };

    template<class ktype, class vtype>
    dict<ktype, vtype>::~dict() { clear(); }

    template<class ktype, class vtype>
    dict<ktype, vtype>::dict() : storage(0) {}

    template<class ktype, class vtype>
    dict<ktype, vtype>::dict(const dict<ktype, vtype>& other) : storage(0) {
        assign(other);
    }

    template<class ktype, class vtype>
    template<class kk, class vv>
    dict<ktype, vtype>::dict(const dict<kk, vv>& other) : storage(0) {
        assign(other);
    }

    template<class ktype, class vtype>
    dict<ktype, vtype>& dict<ktype, vtype>::operator =(
        const dict<ktype, vtype>& other
    ) {
        if (this == &other) return *this;
        clear();
        assign(other);
    }

    template<class ktype, class vtype>
    template<class kk, class vv>
    dict<ktype, vtype>& dict<ktype, vtype>::operator =(
        const dict<kk, vv>& other
    ) {
        clear();
        assign(other);
    }

    template<class ktype, class vtype>
    vtype& dict<ktype, vtype>::operator [](const ktype& key) {
        if (!storage) rehash(4);
        size_t hh = hash(key) | HIGHBIT;
        int64 index = search(key, hh);
        if (index < 0) index = inject(key, 0, hh);
        return storage->bucket[index].getval();
    }

    template<class ktype, class vtype>
    void dict<ktype, vtype>::insert(const ktype& key) {
        size_t hh = hash(key) | HIGHBIT;
        inject(key, 0, hh);
    }

    template<class ktype, class vtype>
    void dict<ktype, vtype>::insert(const ktype& key, const vtype& val) {
        size_t hh = hash(key) | HIGHBIT;
        inject(key, &val, hh);
    }

    template<class ktype, class vtype>
    void dict<ktype, vtype>::remove(const ktype& key) {
        if (!storage) return;
        size_t hh = hash(key) | HIGHBIT;
        int64 index = search(key, hh);
        if (index >= 0) backshift(index);
    }

    template<class ktype, class vtype>
    bool dict<ktype, vtype>::haskey(const ktype& key) const {
        if (!storage) return false;
        size_t hh = hash(key) | HIGHBIT;
        return search(key, hh) >= 0;
    }

    template<class ktype, class vtype>
    void dict<ktype, vtype>::reserve(int64 count) {
        check(count > 0, "must reserve a positive count: %lld", count);
        int64 newsize = 4;
        while (3*newsize/4 < count) newsize *= 2;
        if (!storage || storage->capacity < newsize) rehash(newsize);
    }

    template<class ktype, class vtype>
    void dict<ktype, vtype>::shrink() {
        if (!storage) return;
        const int64 len = storage->quantity;
        if (storage->quantity == 0) {
            delete storage;
            storage = 0;
        } else {
            int64 newsize = 4;
            while (3*newsize/4 < len) newsize *= 2;
            if (newsize == storage->capacity) return;
            rehash(newsize);
        }
    }

    template<class ktype, class vtype>
    void dict<ktype, vtype>::clear() {
        if (storage) {
            const int64 len = storage->capacity;
            for (int64 ii = 0; ii<len; ii++) {
                if (storage->bucket[ii].code) {
                    storage->bucket[ii].~keyval<ktype, vtype>();
                }
            }
            free(storage);
            storage = 0;
        }
    }

    template<class ktype, class vtype>
    int64 dict<ktype, vtype>::size() const {
        return storage ? storage->quantity : 0;
    }

    template<class ktype, class vtype>
    list<ktype> dict<ktype, vtype>::keys() const {
        list<ktype> result;
        if (storage) {
            const int64 len = storage->capacity;
            result.reserve(storage->quantity);
            for (int64 ii = 0; ii<len; ii++) {
                if (storage->bucket[ii].code) {
                    result.append(storage->bucket[ii].getkey());
                }
            }
        }
        return result;
    }

    template<class ktype, class vtype>
    list<vtype> dict<ktype, vtype>::vals() const {
        list<vtype> result;
        if (storage) {
            const int64 len = storage->capacity;
            result.reserve(storage->quantity);
            for (int64 ii = 0; ii<len; ii++) {
                if (storage->bucket[ii].code) {
                    result.append(storage->bucket[ii].getval());
                }
            }
        }
        return result;
    }

    template<class ktype, class vtype>
    int64 dict<ktype, vtype>::bins() const {
        return storage ? storage->capacity : 0;
    }

    template<class ktype, class vtype>
    bool dict<ktype, vtype>::skip(int64 index) const {
        return storage->bucket[index].code == 0;
    }

    template<class ktype, class vtype>
    const ktype& dict<ktype, vtype>::key(int64 index) const {
        return storage->bucket[index].getkey();
    }

    template<class ktype, class vtype>
    const vtype& dict<ktype, vtype>::val(int64 index) const {
        return storage->bucket[index].getval();
    }

    template<class ktype, class vtype>
    vtype& dict<ktype, vtype>::val(int64 index) {
        return storage->bucket[index].getval();
    }

    template<class ktype, class vtype>
    void dict<ktype, vtype>::debug() const {
        if (storage) {
            printf("quantity: %lld, capacity: %lld\n", storage->quantity, storage->capacity);
            for (int64 ii = 0; ii<storage->capacity/4; ii++) {
                for (int64 jj = 0; jj<4; jj++) {
                    int64 kk = jj*storage->capacity/4 + ii;
                    if (storage->bucket[kk].code) {
                        int64 want = storage->bucket[kk].code & (storage->capacity - 1);
                        int64 cost = kk - want;
                        if (cost < 0) cost += storage->capacity;
                        printf(
                            "%3lld: [%16zx (%3lld): %8ld -> %-8ld], ",
                            kk, storage->bucket[kk].code, cost,
                            storage->bucket[kk].getkey(),
                            storage->bucket[kk].getval()
                        );
                    } else {
                        printf("%3lld: ----------------------------------------------, ", kk);
                    }
                }
                printf("\n");
            }
        } else {
            printf("empty\n");
        }
    }

    template<class ktype, class vtype>
    template<class kk, class vv>
    void dict<ktype, vtype>::assign(const dict<kk, vv>& other) {
        if (other.storage) {
            int64 cap = other.storage->capacity;
            storage = alloc<implementation>((cap - 1)*sizeof(keyval<ktype, vtype>));
            storage->capacity = cap;
            for (int64 ii = 0; ii<cap; ii++) {
                storage->bucket[ii].code = 0;
            }
            for (int64 ii = 0; ii<cap; ii++) {
                if (other.storage->bucket[ii].code) {
                    keyval<ktype, vtype> kv(0,
                        other.storage->bucket[ii].getkey(),
                        other.storage->bucket[ii].getval()
                    );
                    // XXX: should copy the old hash code if we have the same keytype
                    kv.code = hash(kv.getkey()) | HIGHBIT;
                    int64 newspot = kv.code&(cap-1);
                    robinhood(newspot, kv, 0);
                }
            }
            storage->quantity = other.storage->quantity;
        }
    }

    template<class ktype, class vtype>
    void dict<ktype, vtype>::rehash(int64 capacity) {
        implementation* old = storage;
        storage = alloc<implementation>((capacity - 1)*sizeof(keyval<ktype, vtype>));
        storage->capacity = capacity;
        for (int64 ii = 0; ii<capacity; ++ii) {
            storage->bucket[ii].code = 0;
        }
        if (old) {
            storage->quantity = old->quantity;
            const int64 cap = old->capacity;
            for (int64 ii = 0; ii<cap; ii++) {
                if (old->bucket[ii].code) {
                    int64 newspot = old->bucket[ii].code&(capacity - 1);
                    robinhood(newspot, old->bucket[ii], 0);
                    old->bucket[ii].~keyval<ktype, vtype>();
                }
            }
            free(old);
        } else {
            storage->quantity = 0;
        }
    }

    template<class ktype, class vtype>
    int64 dict<ktype, vtype>::inject(const ktype& key, const vtype* ptr, size_t hh) {
        if (!storage || storage->quantity >= 3*storage->capacity/4) {
            rehash(storage ? 2*storage->capacity : 4);
        }
        const int64 cap = storage->capacity;
        for (int64 probe = 0; probe<cap; probe++) {
            size_t xx = (probe + hh)&(cap - 1);
            if (!storage->bucket[xx].code) {
                // we found an empty bucket to place our key and val
                if (ptr) {
                    new(storage->bucket + xx) keyval<ktype, vtype>(hh, key, *ptr);
                } else {
                    new(storage->bucket + xx) keyval<ktype, vtype>(hh, key, vtype());
                }
                ++storage->quantity;
                return xx;
            }
            if (storage->bucket[xx].code == hh) {
                // matching hash value, need to check the key
                if (storage->bucket[xx].getkey() == key) {
                    // we found the same key, so replace the val
                    storage->bucket[xx].setval(ptr ? *ptr : vtype());
                    return xx;
                }
            }
            // someone is in our spot, maybe we should move them
            int64 oldcost = xx - (storage->bucket[xx].code&(cap - 1));
            if (oldcost < 0) oldcost += cap;
            if (probe > oldcost) {
                keyval<ktype, vtype> kv(hh, key, ptr ? *ptr : vtype());
                trade(kv, storage->bucket[xx]);
                ++storage->quantity;
                robinhood(xx + 1, kv, oldcost + 1);
                return xx;
            }
            // their cost is greater than ours, keep probing
        }
        check(false, "should never get here");
        return -1;
    }

    template<class ktype, class vtype>
    void dict<ktype, vtype>::robinhood(
        int64 probe, keyval<ktype, vtype>& kv, int64 cost
    ) {
        const int64 cap = storage->capacity;
        for (;; ++probe) {
            int64 xx = probe&(cap - 1);
            if (!storage->bucket[xx].code) {
                new(storage->bucket + xx) keyval<ktype, vtype>();
                trade(kv, storage->bucket[xx]);
                return;
            }
            // compare costs to see who has to keep moving
            int64 oldcost = xx - (storage->bucket[xx].code&(cap - 1));
            if (oldcost < 0) oldcost += cap;
            if (cost >= oldcost) {
                // place the current one here, but keep going
                trade(kv, storage->bucket[xx]);
                cost = oldcost;
            } else {
                ++cost;
            }
            // swap or not, keep probing for an empty bin
        }
    }

    template<class ktype, class vtype>
    int64 dict<ktype, vtype>::search(const ktype& key, size_t hh) const {
        const int64 cap = storage->capacity;
        for (int64 probe = 0; probe<cap; probe++) {
            int64 xx = (probe + hh)&(cap - 1);
            if (!storage->bucket[xx].code) return -1;
            if (storage->bucket[xx].code == hh) {
                if (storage->bucket[xx].getkey() == key) return xx;
            }
            int64 cost = xx - (storage->bucket[xx].code&(cap - 1));
            if (cost < 0) cost += cap;
            // we can't be past a higher cost point
            if (probe > cost) return -1;
        }
        check(false, "should never get here");
        return -1;
    }

    template<class ktype, class vtype>
    void dict<ktype, vtype>::backshift(int64 index) {
        const int64 cap = storage->capacity;
        for (int64 ii = 0; ii<cap; ii++) {
            int64 next = (index + 1)&(cap - 1);
            if (!storage->bucket[next].code) {
                // nobody next to us to shift back
                break;
            }
            int64 cost = next - (storage->bucket[next].code&(cap - 1));
            if (cost < 0) cost += cap;
            if (cost == 0) {
                // the next guy is happy where he's at
                break;
            }
            // shuffle the next guy back, and pickup from there
            trade(storage->bucket[index], storage->bucket[next]);
            index = next;
        }

        storage->bucket[index].~keyval<ktype, vtype>();
        --storage->quantity;
    }

    template<class ktype, class vtype>
    void swap(dict<ktype, vtype>& flip, dict<ktype, vtype>& flop) {
        swap(flip.storage, flop.storage);
    }

    //}}}
    //{{{ shared
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

    //{{{ implementation 
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
    //}}}
    //}}}
    //}}}
    //{{{ Special Containers:            tuple, queue 
    //{{{ tuple
    //{{{ tuple 8
    template<
        class type1=none, class type2=none, class type3=none, class type4=none,
        class type5=none, class type6=none, class type7=none, class type8=none
    >
    struct tuple {
        type1 first; type2 second; type3 third; type4 fourth;
        type5 fifth; type6 sixth; type7 seventh; type8 eighth;

        tuple() {}

        tuple(type1 v1, type2 v2, type3 v3, type4 v4, type5 v5, type6 v6, type7 v7, type8 v8) :
            first(v1), second(v2), third(v3), fourth(v4),
            fifth(v5), sixth(v6), seventh(v7), eighth(v8) {
        }

        template<
            class t1, class t2, class t3, class t4, 
            class t5, class t6, class t7, class t8
        >
        tuple(const tuple<t1, t2, t3, t4, t5, t6, t7, t8>& tt) :
            first(tt.first), second(tt.second), third(tt.third), fourth(tt.fourth),
            fifth(tt.fifth), sixth(tt.sixth), seventh(tt.seventh), eighth(tt.eighth) {
        }

        template<
            class t1, class t2, class t3, class t4, 
            class t5, class t6, class t7, class t8
        >
        tuple<type1, type2, type3, type4, type5, type6, type7, type8>&
        operator =(const tuple<t1, t2, t3, t4, t5, t6, t7, t8>& that) {
            first   = that.first;   second = that.second;
            third   = that.third;   fourth = that.fourth;
            fifth   = that.fifth;   sixth  = that.sixth;
            seventh = that.seventh; eighth = that.eighth;
            return *this;
        }
    };

    template<
        class type1, class type2, class type3, class type4,
        class type5, class type6, class type7, class type8
    >
    tuple<type1, type2, type3, type4, type5, type6, type7, type8>
    multival(
        const type1& val1, const type2& val2,
        const type3& val3, const type4& val4,
        const type5& val5, const type6& val6,
        const type7& val7, const type8& val8
    ) {
        return tuple<
            type1, type2, type3, type4,
            type5, type6, type7, type8
        >(val1, val2, val3, val4, val5, val6, val7, val8);
    }

    template<
        class type1, class type2, class type3, class type4,
        class type5, class type6, class type7, class type8
    >
    tuple<type1&, type2&, type3&, type4&, type5&, type6&, type7&, type8&>
    multiref(
        type1& ref1, type2& ref2,
        type3& ref3, type4& ref4,
        type5& ref5, type6& ref6,
        type7& ref7, type8& ref8
    ) {
        return tuple<
            type1&, type2&, type3&, type4&,
            type5&, type6&, type7&, type8&
        >(ref1, ref2, ref3, ref4, ref5, ref6, ref7, ref8);
    }

    template<
        class type1, class type2, class type3, class type4,
        class type5, class type6, class type7, class type8
    >
    size_t hash(
        const tuple<
            type1, type2, type3, type4, 
            type5, type6, type7, type8
        >& tt
    ) {
        return (
            1*hash(tt.first) + 3*hash(tt.second) + 5*hash(tt.third) + 7*hash(tt.fourth) +
            11*hash(tt.fifth) + 13*hash(tt.sixth) + 17*hash(tt.seventh) + 19*hash(tt.eighth)
        );
    }

    template<
        class a1, class a2, class a3, class a4, class a5, class a6, class a7, class a8,
        class b1, class b2, class b3, class b4, class b5, class b6, class b7, class b8
    >
    bool operator ==(
        const tuple<a1, a2, a3, a4, a5, a6, a7, a8>& aa,
        const tuple<b1, b2, b3, b4, b5, b6, b7, b8>& bb
    ) {
        return (
            aa.first == bb.first && aa.second == bb.second && aa.third == bb.third && aa.fourth == bb.fourth &&
            aa.fifth == bb.fifth && aa.sixth == bb.sixth && aa.seventh == bb.seventh && aa.eighth == bb.eighth
        );
    }

    //}}}
    //{{{ tuple 7
    template<
        class type1, class type2, class type3, class type4,
        class type5, class type6, class type7
    > 
    struct tuple<type1, type2, type3, type4, type5, type6, type7> {
        type1 first; type2 second; type3 third; type4 fourth;
        type5 fifth; type6 sixth; type7 seventh;

        tuple() {}

        tuple(type1 v1, type2 v2, type3 v3, type4 v4, type5 v5, type6 v6, type7 v7) :
            first(v1), second(v2), third(v3), fourth(v4),
            fifth(v5), sixth(v6), seventh(v7) {
        }

        template<
            class t1, class t2, class t3, class t4, 
            class t5, class t6, class t7
        >
        tuple(const tuple<t1, t2, t3, t4, t5, t6, t7>& tt) :
            first(tt.first), second(tt.second), third(tt.third), fourth(tt.fourth),
            fifth(tt.fifth), sixth(tt.sixth), seventh(tt.seventh) {
        }

        template<
            class t1, class t2, class t3, class t4, 
            class t5, class t6, class t7
        >
        tuple<type1, type2, type3, type4, type5, type6, type7>&
        operator =(const tuple<t1, t2, t3, t4, t5, t6, t7>& that) {
            first   = that.first;   second = that.second;
            third   = that.third;   fourth = that.fourth;
            fifth   = that.fifth;   sixth  = that.sixth;
            seventh = that.seventh;
            return *this;
        }
    };

    template<
        class type1, class type2, class type3, class type4,
        class type5, class type6, class type7
    >
    tuple<type1, type2, type3, type4, type5, type6, type7>
    multival(
        const type1& val1, const type2& val2,
        const type3& val3, const type4& val4,
        const type5& val5, const type6& val6,
        const type7& val7
    ) {
        return tuple<
            type1, type2, type3, type4,
            type5, type6, type7
        >(val1, val2, val3, val4, val5, val6, val7);
    }

    template<
        class type1, class type2, class type3, class type4,
        class type5, class type6, class type7
    >
    tuple<type1&, type2&, type3&, type4&, type5&, type6&, type7&>
    multiref(
        type1& ref1, type2& ref2,
        type3& ref3, type4& ref4,
        type5& ref5, type6& ref6,
        type7& ref7
    ) {
        return tuple<
            type1&, type2&, type3&, type4&,
            type5&, type6&, type7&
        >(ref1, ref2, ref3, ref4, ref5, ref6, ref7);
    }

    template<
        class type1, class type2, class type3, class type4,
        class type5, class type6, class type7
    >
    size_t hash(
        const tuple<
            type1, type2, type3, type4, 
            type5, type6, type7
        >& tt
    ) {
        return (
            1*hash(tt.first) + 3*hash(tt.second) + 5*hash(tt.third) + 7*hash(tt.fourth) +
            11*hash(tt.fifth) + 13*hash(tt.sixth) + 17*hash(tt.seventh)
        );
    }

    template<
        class a1, class a2, class a3, class a4, class a5, class a6, class a7,
        class b1, class b2, class b3, class b4, class b5, class b6, class b7
    >
    bool operator ==(
        const tuple<a1, a2, a3, a4, a5, a6, a7>& aa,
        const tuple<b1, b2, b3, b4, b5, b6, b7>& bb
    ) {
        return (
            aa.first == bb.first && aa.second == bb.second && aa.third == bb.third && aa.fourth == bb.fourth &&
            aa.fifth == bb.fifth && aa.sixth == bb.sixth && aa.seventh == bb.seventh
        );
    }

    //}}}
    //{{{ tuple 6
    template<
        class type1, class type2, class type3, class type4,
        class type5, class type6
    > 
    struct tuple<type1, type2, type3, type4, type5, type6> {
        type1 first; type2 second; type3 third; type4 fourth;
        type5 fifth; type6 sixth;

        tuple() {}

        tuple(type1 v1, type2 v2, type3 v3, type4 v4, type5 v5, type6 v6) :
            first(v1), second(v2), third(v3), fourth(v4),
            fifth(v5), sixth(v6) {
        }

        template<
            class t1, class t2, class t3, class t4, 
            class t5, class t6
        >
        tuple(const tuple<t1, t2, t3, t4, t5, t6>& tt) :
            first(tt.first), second(tt.second), third(tt.third), fourth(tt.fourth),
            fifth(tt.fifth), sixth(tt.sixth) {
        }

        template<
            class t1, class t2, class t3, class t4, 
            class t5, class t6
        >
        tuple<type1, type2, type3, type4, type5, type6>&
        operator =(const tuple<t1, t2, t3, t4, t5, t6>& that) {
            first   = that.first;   second = that.second;
            third   = that.third;   fourth = that.fourth;
            fifth   = that.fifth;   sixth  = that.sixth;
            return *this;
        }
    };

    template<
        class type1, class type2, class type3, class type4,
        class type5, class type6
    >
    tuple<type1, type2, type3, type4, type5, type6>
    multival(
        const type1& val1, const type2& val2,
        const type3& val3, const type4& val4,
        const type5& val5, const type6& val6
    ) {
        return tuple<
            type1, type2, type3, type4,
            type5, type6
        >(val1, val2, val3, val4, val5, val6);
    }

    template<
        class type1, class type2, class type3, class type4,
        class type5, class type6
    >
    tuple<type1&, type2&, type3&, type4&, type5&, type6&>
    multiref(
        type1& ref1, type2& ref2,
        type3& ref3, type4& ref4,
        type5& ref5, type6& ref6
    ) {
        return tuple<
            type1&, type2&, type3&, type4&,
            type5&, type6&
        >(ref1, ref2, ref3, ref4, ref5, ref6);
    }

    template<
        class type1, class type2, class type3, class type4,
        class type5, class type6
    >
    size_t hash(
        const tuple<
            type1, type2, type3, type4, 
            type5, type6
        >& tt
    ) {
        return (
            1*hash(tt.first) + 3*hash(tt.second) + 5*hash(tt.third) + 7*hash(tt.fourth) +
            11*hash(tt.fifth) + 13*hash(tt.sixth)
        );
    }

    template<
        class a1, class a2, class a3, class a4, class a5, class a6,
        class b1, class b2, class b3, class b4, class b5, class b6
    >
    bool operator ==(
        const tuple<a1, a2, a3, a4, a5, a6>& aa,
        const tuple<b1, b2, b3, b4, b5, b6>& bb
    ) {
        return (
            aa.first == bb.first && aa.second == bb.second && aa.third == bb.third && aa.fourth == bb.fourth &&
            aa.fifth == bb.fifth && aa.sixth == bb.sixth
        );
    }

    //}}}
    //{{{ tuple 5
    template<
        class type1, class type2, class type3, class type4,
        class type5
    > 
    struct tuple<type1, type2, type3, type4, type5> {
        type1 first; type2 second; type3 third; type4 fourth;
        type5 fifth;

        tuple() {}

        tuple(type1 v1, type2 v2, type3 v3, type4 v4, type5 v5) :
            first(v1), second(v2), third(v3), fourth(v4), fifth(v5) {
        }

        template<class t1, class t2, class t3, class t4, class t5>
        tuple(const tuple<t1, t2, t3, t4, t5>& tt) :
            first(tt.first), second(tt.second), third(tt.third), fourth(tt.fourth),
            fifth(tt.fifth) {
        }

        template<
            class t1, class t2, class t3, class t4, 
            class t5
        >
        tuple<type1, type2, type3, type4, type5>&
        operator =(const tuple<t1, t2, t3, t4, t5>& that) {
            first   = that.first;   second = that.second;
            third   = that.third;   fourth = that.fourth;
            fifth   = that.fifth;
            return *this;
        }
    };

    template<
        class type1, class type2, class type3, class type4,
        class type5
    >
    tuple<type1, type2, type3, type4, type5>
    multival(
        const type1& val1, const type2& val2,
        const type3& val3, const type4& val4,
        const type5& val5
    ) {
        return tuple<type1, type2, type3, type4, type5>(
            val1, val2, val3, val4, val5
        );
    }

    template<
        class type1, class type2, class type3, class type4,
        class type5
    >
    tuple<type1&, type2&, type3&, type4&, type5&>
    multiref(
        type1& ref1, type2& ref2,
        type3& ref3, type4& ref4,
        type5& ref5
    ) {
        return tuple<type1&, type2&, type3&, type4&, type5&>(
            ref1, ref2, ref3, ref4, ref5
        );
    }

    template<
        class type1, class type2, class type3, class type4,
        class type5
    >
    size_t hash(const tuple<type1, type2, type3, type4, type5>& tt) {
        return (
            1*hash(tt.first) + 3*hash(tt.second) + 5*hash(tt.third) + 7*hash(tt.fourth) +
            11*hash(tt.fifth)
        );
    }

    template<
        class a1, class a2, class a3, class a4, class a5,
        class b1, class b2, class b3, class b4, class b5
    >
    bool operator ==(
        const tuple<a1, a2, a3, a4, a5>& aa,
        const tuple<b1, b2, b3, b4, b5>& bb
    ) {
        return (
            aa.first == bb.first && aa.second == bb.second && aa.third == bb.third && aa.fourth == bb.fourth &&
            aa.fifth == bb.fifth
        );
    }

    //}}}
    //{{{ tuple 4
    template<class type1, class type2, class type3, class type4> 
    struct tuple<type1, type2, type3, type4> {
        type1 first; type2 second; type3 third; type4 fourth;

        tuple() {}

        tuple(type1 v1, type2 v2, type3 v3, type4 v4) :
            first(v1), second(v2), third(v3), fourth(v4) {
        }

        template<class t1, class t2, class t3, class t4>
        tuple(const tuple<t1, t2, t3, t4>& tt) :
            first(tt.first), second(tt.second), third(tt.third), fourth(tt.fourth) {
        }

        template<class t1, class t2, class t3, class t4>
        tuple<type1, type2, type3, type4>&
        operator =(const tuple<t1, t2, t3, t4>& that) {
            first   = that.first;   second = that.second;
            third   = that.third;   fourth = that.fourth;
            return *this;
        }
    };

    template<class type1, class type2, class type3, class type4>
    tuple<type1, type2, type3, type4>
    multival(
        const type1& val1, const type2& val2,
        const type3& val3, const type4& val4
    ) {
        return tuple<type1, type2, type3, type4>(
            val1, val2, val3, val4
        );
    }

    template<class type1, class type2, class type3, class type4>
    tuple<type1&, type2&, type3&, type4&>
    multiref(
        type1& ref1, type2& ref2,
        type3& ref3, type4& ref4
    ) {
        return tuple<type1&, type2&, type3&, type4&>(
            ref1, ref2, ref3, ref4
        );
    }

    template<class type1, class type2, class type3, class type4>
    size_t hash(const tuple<type1, type2, type3, type4>& tt) {
        return (
            1*hash(tt.first) + 3*hash(tt.second) + 5*hash(tt.third) + 7*hash(tt.fourth)
        );
    }

    template<
        class a1, class a2, class a3, class a4,
        class b1, class b2, class b3, class b4
    >
    bool operator ==(
        const tuple<a1, a2, a3, a4>& aa,
        const tuple<b1, b2, b3, b4>& bb
    ) {
        return (
            aa.first == bb.first && aa.second == bb.second && aa.third == bb.third && aa.fourth == bb.fourth
        );
    }

    //}}}
    //{{{ tuple 3
    template<class type1, class type2, class type3> 
    struct tuple<type1, type2, type3> {
        type1 first; type2 second; type3 third;

        tuple() {}

        tuple(type1 v1, type2 v2, type3 v3) :
            first(v1), second(v2), third(v3) {
        }

        template<class t1, class t2, class t3>
        tuple(const tuple<t1, t2, t3>& tt) :
            first(tt.first), second(tt.second), third(tt.third) {
        }

        template<class t1, class t2, class t3>
        tuple<type1, type2, type3>&
        operator =(const tuple<t1, t2, t3>& that) {
            first   = that.first;   second = that.second;
            third   = that.third;
            return *this;
        }
    };

    template<class type1, class type2, class type3>
    tuple<type1, type2, type3>
    multival(
        const type1& val1, const type2& val2,
        const type3& val3
    ) {
        return tuple<type1, type2, type3>(
            val1, val2, val3
        );
    }

    template<class type1, class type2, class type3>
    tuple<type1&, type2&, type3&>
    multiref(
        type1& ref1, type2& ref2,
        type3& ref3
    ) {
        return tuple<type1&, type2&, type3&>(
            ref1, ref2, ref3
        );
    }

    template<class type1, class type2, class type3>
    size_t hash(const tuple<type1, type2, type3>& tt) {
        return 1*hash(tt.first) + 3*hash(tt.second) + 5*hash(tt.third);
    }

    template<
        class a1, class a2, class a3,
        class b1, class b2, class b3
    >
    bool operator ==(
        const tuple<a1, a2, a3>& aa,
        const tuple<b1, b2, b3>& bb
    ) {
        return (
            aa.first == bb.first && aa.second == bb.second && aa.third == bb.third
        );
    }

    //}}}
    //{{{ tuple 2
    template<class type1, class type2> 
    struct tuple<type1, type2> {
        type1 first; type2 second;

        tuple() {}

        tuple(type1 v1, type2 v2) :
            first(v1), second(v2) {
        }

        template<class t1, class t2>
        tuple(const tuple<t1, t2>& tt) :
            first(tt.first), second(tt.second) {
        }

        template<class t1, class t2>
        tuple<type1, type2>&
        operator =(const tuple<t1, t2>& that) {
            first   = that.first;   second = that.second;
            return *this;
        }
    };

    template<class type1, class type2>
    tuple<type1, type2>
    multival(
        const type1& val1, const type2& val2
    ) {
        return tuple<type1, type2>(val1, val2);
    }

    template<class type1, class type2>
    tuple<type1&, type2&>
    multiref(
        type1& ref1, type2& ref2
    ) {
        return tuple<type1&, type2&>(ref1, ref2);
    }

    template<class type1, class type2>
    size_t hash(const tuple<type1, type2>& tt) {
        return 1*hash(tt.first) + 3*hash(tt.second);
    }

    template<
        class a1, class a2,
        class b1, class b2
    >
    bool operator ==(
        const tuple<a1, a2>& aa,
        const tuple<b1, b2>& bb
    ) {
        return (
            aa.first == bb.first && aa.second == bb.second
        );
    }

    //}}}
    //{{{ tuple 1
    template<class type1> 
    struct tuple<type1> {
        type1 first;

        tuple() {}

        tuple(type1 v1) : first(v1) {
        }

        template<class t1>
        tuple(const tuple<t1>& tt) : first(tt.first) {}

        template<class t1>
        tuple<type1>&
        operator =(const tuple<t1>& that) {
            first   = that.first;
            return *this;
        }
    };

    template<class type1>
    tuple<type1>
    multival(const type1& val1) {
        return tuple<type1>(val1);
    }

    template<class type1>
    tuple<type1&>
    multiref(type1& ref1) {
        return tuple<type1&>(ref1);
    }

    template<class type1>
    size_t hash(const tuple<type1>& tt) {
        return hash(tt.first);
    }

    template<class a1, class b1>
    bool operator ==(const tuple<a1>& aa, const tuple<b1>& bb) {
        return aa.first == bb.first;
    }

    //}}}
    //{{{ tuple 0
    template<> struct tuple<> {};

    tuple<> multival() {
        return tuple<>();
    }

    tuple<> multiref() {
        return tuple<>();
    }

    template<>
    size_t hash(const tuple<>& tt) {
        (void)tt;
        return 0;
    }

    template<>
    bool operator ==(const tuple<>& aa, const tuple<>& bb) {
        (void)aa; (void)bb;
        return true;
    }

    //}}}
    //}}}
    //{{{ queue
    template<class type>
    struct queue {
        queue();
        ~queue();

        void give(type& item);
        void push(const type& item);
        type pull();
        bool pull(type& item, double seconds);

        private:
            queue(const queue&); // deleted
            queue& operator =(const queue&); // deleted

            list<type> data;     
            pthread_mutex_t mutex;
            pthread_cond_t condvar;
    };

    template<class type>
    queue<type>::queue() {
        pthread_condattr_t attr;
        pthread_condattr_init(&attr);
        pthread_condattr_setclock(&attr, CLOCK_MONOTONIC);
        pthread_mutex_init(&mutex, 0);
        pthread_cond_init(&condvar, &attr);
    }

    template<class type>
    queue<type>::~queue() {
        pthread_mutex_destroy(&mutex);
        pthread_cond_destroy(&condvar);
    }

    template<class type>
    void queue<type>::give(type& item) {
        pthread_mutex_lock(&mutex);

        data.append(type());
        swap(data[data.size() - 1], item);

        pthread_cond_signal(&condvar);
        pthread_mutex_unlock(&mutex);
    }

    template<class type>
    void queue<type>::push(const type& item) {
        pthread_mutex_lock(&mutex);

        data.append(item);

        pthread_cond_signal(&condvar);
        pthread_mutex_unlock(&mutex);
    }

    template<class type>
    type queue<type>::pull() {
        type item;
        pthread_mutex_lock(&mutex);

        while (data.size() == 0) {
            pthread_cond_wait(&condvar, &mutex);
        }

        swap(item, data[0]);
        data.remove(0);

        pthread_mutex_unlock(&mutex);

        return item;
    }

    template<class type>
    bool queue<type>::pull(type& item, double seconds) {
        struct timespec tv;
        clock_gettime(CLOCK_MONOTONIC, &tv);
        int64 nanos = tv.tv_sec*1000000000LL + tv.tv_nsec;

        nanos += llrint(seconds*1e9);

        tv.tv_sec  = nanos/1000000000LL;
        tv.tv_nsec = nanos%1000000000LL;

        pthread_mutex_lock(&mutex);

        while (data.size() == 0) {
            int result = pthread_cond_timedwait(&condvar, &mutex, &tv);
            if (result == ETIMEDOUT) {
                pthread_mutex_unlock(&mutex);
                return false;
            }
        }

        swap(item, data[0]);
        data.remove(0);

        pthread_mutex_unlock(&mutex);

        return true;
    }

    //}}}
    //}}}
    //{{{ Sorting and Selection:         introsort, (mergesort), (quickselect) 
    //{{{ comparisons
    template<class type>
    bool compare_lt(const type& aa, const type& bb) { return aa <  bb; }

    template<class type>
    bool compare_le(const type& aa, const type& bb) { return aa <= bb; }

    template<class type>
    bool compare_ge(const type& aa, const type& bb) { return aa >= bb; }

    template<class type>
    bool compare_gt(const type& aa, const type& bb) { return aa >  bb; }

    template<class type>
    bool compare_eq(const type& aa, const type& bb) { return aa == bb; }

    template<class type>
    bool compare_ne(const type& aa, const type& bb) { return aa != bb; }
    //}}}
    //{{{ introsort
    namespace internal {
        template<class type, class compare>
        void dualpivot(type* lo, type* hi, type*& lt, type*& gt, compare& lessthan) {
            // before loop, L and H are chosen as pivots
            //
            //     +---+---+---+---+---+---+---+---+---+---+---+
            //     | L | ? | ? | ? | ? | ? | ? | ? | ? | ? | H |
            //     +---+---+---+---+---+---+---+---+---+---+---+
            //     ^lo ^lt,ii                              ^gt,hi
            //
            int64 third = (hi - lo)/3;
            if (lessthan(lo[+third], hi[-third])) {
                swap(*lo, lo[+third]);
                swap(*hi, hi[-third]);
            } else {
                swap(*lo, hi[-third]);
                swap(*hi, lo[+third]);
            }

            // during loop, unknowns are organized
            //
            //     +---+---+---+---+---+---+---+---+---+---+---+
            //     | L | < | < | ~ | ~ | ? | ? | ? | > | > | H |
            //     +---+---+---+---+---+---+---+---+---+---+---+
            //     ^lo         ^lt     ^ii         ^gt     ^hi
            //
            lt = lo + 1;
            gt = hi;
            type* ii = lt;
            while (ii < gt) {
                if (lessthan(*ii, *lo)) {
                    swap(*lt++, *ii++);
                    continue;
                }
                if (lessthan(*hi, *ii)) {
                    swap(*ii, *--gt);
                    continue;
                }
                ++ii;
            }

            // after loop, no more unknowns
            //
            //     +---+---+---+---+---+---+---+---+---+---+---+
            //     | L | < | < | < | ~ | ~ | ~ | > | > | > | H |
            //     +---+---+---+---+---+---+---+---+---+---+---+
            //     ^lo             ^lt         ^gt         ^hi
            //
            // swapping the pivots, data is partitioned:
            //
            //          lowers < L <= centers <= H < uppers
            //     +---+---+---+---+---+---+---+---+---+---+---+
            //     | < | < | < | L | ~ | ~ | ~ | H | > | > | > |
            //     +---+---+---+---+---+---+---+---+---+---+---+
            //     ^lo         ^lt                 ^gt     ^hi
            //
            swap(*lo, *--lt);
            swap(*hi, *gt++);
        }

        // This heapsifts in the opposite order than you would expect
        // so that heapsort below can remove the LARGEST element first...
        // XXX: Should figure out a way to re-use this for a heap<T> container.
        template<class type, class compare>
        void heapsift(type* ptr, int64 root, int64 count, compare& lessthan) {
            while (root*2 + 1 < count) {
                int64 child = root*2 + 1;
                if (child < count - 1) {
                    if (lessthan(ptr[child], ptr[child + 1])) {
                        child = child + 1;
                    }
                }
                if (lessthan(ptr[root], ptr[child])) {
                    swap(ptr[root], ptr[child]);
                    root = child;
                } else {
                    return;
                }
            }
        }

        template<class type, class compare>
        void heapsort(type* ptr, int64 len, compare& lessthan) {
            int64 start = len/2 - 1;
            int64 end = len - 1;

            while (start >= 0) {
                heapsift(ptr, start, len, lessthan);
                start = start - 1;
            }

            while (end > 0) {
                swap(ptr[end], *ptr);
                heapsift(ptr, 0, end, lessthan);
                end = end - 1;
            }
        }

        template<class type, class compare>
        void recursort(type* lo, type* hi, int64 depth, compare& lessthan) {
            int64 span = hi - lo;
            if (span <= 0) return;
            if (span <= 8) {
                // insertion sort
                for (type* ii = lo + 1; ii <= hi; ++ii) {
                    for (type* jj = ii; jj > lo; --jj) {
                        if (lessthan(*jj, jj[-1])) {
                            swap(*jj, jj[-1]);
                        } else {
                            break;
                        }
                    }
                }
                return;
            }

            if (depth < 0) {
                // recursed too deep, fallback to heapsort
                heapsort(lo, span + 1, lessthan);
                return;
            }

            // quicksort
            type *lt, *gt;
            dualpivot(lo, hi, lt, gt, lessthan);

            // recurse as needed
            recursort(lo, lt - 1, depth - 1, lessthan);
            if (lessthan(*lt, gt[-1])) {
                recursort(lt + 1, gt - 2, depth - 1, lessthan);
            }
            recursort(gt, hi, depth - 1, lessthan);
        }
    }

    template<class type, class compare>
    void introsort(type* data, int64 len, compare& lessthan) {
        using namespace internal;
        // this is a fast integer logarithm base two
        int64 depth = 8*sizeof(long long) - __builtin_clzll(len);
        recursort(data, data + len - 1, depth, lessthan);
    }

    template<class type>
    void introsort(type* data, int64 len) {
        introsort(data, len, compare_lt<type>);
    }

    /* XXX: do these for vector<> and list<> ?
    template<class type, int64 size, class compare>
    void introsort(vector<type, size>& vv, compare& lessthan) {
        introsort(vv.data(), vv.size(), lessthan);
    }

    template<class type, int64 size>
    void introsort(vector<type, size>& vv) {
        introsort(vv.data(), vv.size(), compare_lt<type>);
    }
    */
    //}}}
    //}}}
    //{{{ Unicode Strings (UTF-8):       string 
    struct string;
    static inline void swap(string& flip, string& flop);
    static inline string substr(const string& ss, int64 off, int64 len);
    static inline string join(const list<string>& ll);

    struct string {
        inline ~string();
        inline string();
        inline string(const string& other);
        inline string(const char* ptr);
        inline string& operator =(const string& other);
        inline string& operator =(const char* ptr);

        inline int64 size() const;
        inline int64 codes() const;
        inline const char* data() const;

        private:
            // only friend functions can see these two
            inline string(int64 len);
            inline char* ptr();

            friend string substr(const string& ss, int64 off, int64 len);
            friend string join(const list<string>& ll);

            friend void swap(string& flip, string& flop);
            char* storage;
    };

    //{{{ UTF-8 helpers
    namespace internal {

        static inline bool utf8trail(
            const char*& ptr, int32_t mask, int64 len, int32_t& code
        ) {
            int32_t sum = *ptr++ & mask;
            for (int64 ii = 0; ii<len; ii++) {
                if (*ptr == 0) return false;
                if ((*ptr & 0xC0) != 0x80) {
                    ++ptr;
                    return false;
                }
                sum <<= 6;
                sum |= *ptr++ & 0x3F;
            }
            code = sum;
            return true;
        }

        static inline bool utf8decode(const char*& ptr, int32_t& code) {
            check(*ptr != 0, "can't decode null terminator");
            if ((0x80 & *ptr) == 0) {
                code = *ptr++;
                return true;
            }
            if ((0xE0 & *ptr) == 0xC0) return utf8trail(ptr, 0x1F, 1, code);
            if ((0xF0 & *ptr) == 0xE0) return utf8trail(ptr, 0x0F, 2, code);
            if ((0xF8 & *ptr) == 0xF0) return utf8trail(ptr, 0x07, 3, code);
            if ((0xFC & *ptr) == 0xF8) return utf8trail(ptr, 0x03, 4, code);
            if ((0xFE & *ptr) == 0xFC) return utf8trail(ptr, 0x01, 5, code);
            ++ptr;
            code = 0xFFFD;
            return false;
        }

        static inline int64 utf8bytes(int32_t code) {
            check(code >= 0, "non-negative code point");
            if (code == 0) return 2; // modified UTF-8 with embedded nulls
            if (code < 0x80) return 1;
            if (code < 0x800) return 2;
            if (code < 0x10000) return 3;
            if (code < 0x200000) return 4;
            if (code < 0x4000000) return 5;
            return 6;
        }

        static inline int64 utf8encode(char* ptr, int32_t code) {
            check(code >= 0, "non-negative code point");
            if (code == 0) { // modified UTF-8 with embedded nulls
                ptr[0] = 0xC0;
                ptr[1] = 0x80;
                return 2;
            }
            if (code < 0x80) {
                ptr[0] = code;
                return 1;
            }
            if (code < 0x800) {
                ptr[0] = (code>>6) | 0xC0;
                ptr[1] = (code & 0x3F) | 0x80;
                return 2;
            }
            if (code < 0x10000) {
                ptr[0] =  (code>>12) | 0xE0;
                ptr[1] = ((code>> 6) & 0x3F) | 0x80;
                ptr[2] = ((code    ) & 0x3F) | 0x80;
                return 3;
            }
            if (code < 0x200000) {
                ptr[0] =  (code>>18) | 0xF0;
                ptr[1] = ((code>>12) & 0x3F) | 0x80;
                ptr[2] = ((code>> 6) & 0x3F) | 0x80;
                ptr[3] = ((code    ) & 0x3F) | 0x80;
                return 4;
            }
            if (code < 0x4000000) {
                ptr[0] =  (code>>24) | 0xF8;
                ptr[1] = ((code>>18) & 0x3F) | 0x80;
                ptr[2] = ((code>>12) & 0x3F) | 0x80;
                ptr[3] = ((code>> 6) & 0x3F) | 0x80;
                ptr[4] = ((code    ) & 0x3F) | 0x80;
                return 5;
            }
            ptr[0] =  (code>>30) | 0xFC;
            ptr[1] = ((code>>24) & 0x3F) | 0x80;
            ptr[2] = ((code>>18) & 0x3F) | 0x80;
            ptr[3] = ((code>>12) & 0x3F) | 0x80;
            ptr[4] = ((code>> 6) & 0x3F) | 0x80;
            ptr[5] = ((code    ) & 0x3F) | 0x80;
            return 6;
        }

        static inline int64 utf8sanitize(char* writer, const char* reader) {
            int64 len = 0;
            // We use the "Replacement Character" for all decoding errors
            const int32_t error = 0xFFFD;
            while (*reader) {
                int32_t code = -1;
                if (utf8decode(reader, code)) {
                    // check for the first of a Surrogate Pair
                    if (0xD800 <= code && code < 0xDC00) {
                        // grab the second of a Surrogate Pair
                        int32_t pair = -1;
                        if (*reader && utf8decode(reader, pair)) {
                            if (0xDC00 <= pair && pair < 0xE000) {
                                // it's a valid pair
                                code = ((code-0xD800)<<10) | (pair-0xDC00);
                            } else {
                                code = error;
                            }
                        } else {
                            code = error;
                        }
                    }
                    // check these after surrogate processing
                    // quietly discard any Byte Order Marks
                    if (code == 0xFEFF) continue;
                    if (code == 0xFFFE) continue;
                    // surrogate encoded surrogates become errors
                    if (0xD800 <= code && code < 0xE000) code = error;
                    // this would be pedantic and discard non-unicode
                    // if (code > 0x10FFFF) code = error;
                } else {
                    code = error;
                }
                len += utf8bytes(code);
                if (writer) writer += utf8encode(writer, code);
            }
            if (writer) *writer = 0;
            return len;
        }
    }
    //}}}
    //{{{ string implementation
    string::~string() { if (storage) free(storage); }

    string::string() : storage(0) {}

    string::string(const string& other) : storage(0) {
        if (other.storage) {
            int64 len = ::strlen(other.storage);
            storage = alloc<char>(len*sizeof(char));
            memcpy(storage, other.storage, len + 1);
        }
    }

    string::string(const char* ptr) : storage(0) {
        using namespace internal;
        if (*ptr) {
            int64 len = utf8sanitize(0, ptr);
            storage = alloc<char>(len*sizeof(char));
            utf8sanitize(storage, ptr);
        }
    }

    string::string(int64 len) : storage(alloc<char>(len*sizeof(char))) {
        storage[len] = 0;
    }

    string& string::operator =(const string& other) {
        if (&other == this) return *this;
        if (storage) free(storage);
        if (other.storage) {
            int64 len = ::strlen(other.storage);
            storage = alloc<char>(len*sizeof(char));
            memcpy(storage, other.storage, len + 1);
        } else {
            storage = 0;
        }
        return *this;
    }

    string& string::operator =(const char* ptr) {
        using namespace internal;
        if (storage) free(storage);
        if (*ptr) {
            int64 len = utf8sanitize(0, ptr);
            storage = alloc<char>(len*sizeof(char));
            utf8sanitize(storage, ptr);
        } else {
            storage = 0;
        }
        return *this;
    }

    int64 string::size() const {
        return storage ? ::strlen(storage) : 0;
    }

    int64 string::codes() const {
        using namespace internal;
        if (!storage) return 0;
        const char* ptr = storage;
        int64 len = 0;
        while (*ptr) {
            int32_t code;
            utf8decode(ptr, code);
            ++len;
        }
        return len;
    }

    const char* string::data() const {
        return storage ? storage : "";
    }

    char* string::ptr() {
        return storage;
    }
    //}}}
    //{{{ string operators
    static inline string operator +(const string& aa, const string& bb) {
        list<string> ll;
        ll.append(aa);
        ll.append(bb);
        return join(ll);
    }

    static inline bool operator <(const string& aa, const string& bb) {
        return ::strcmp(aa.data(), bb.data()) < 0;
    }

    static inline bool operator <=(const string& aa, const string& bb) {
        return ::strcmp(aa.data(), bb.data()) <= 0;
    }

    static inline bool operator >=(const string& aa, const string& bb) {
        return ::strcmp(aa.data(), bb.data()) >= 0;
    }

    static inline bool operator >(const string& aa, const string& bb) {
        return ::strcmp(aa.data(), bb.data()) > 0;
    }

    static inline bool operator ==(const string& aa, const string& bb) {
        return ::strcmp(aa.data(), bb.data()) == 0;
    }

    static inline bool operator !=(const string& aa, const string& bb) {
        return ::strcmp(aa.data(), bb.data()) != 0;
    }
    //}}}

    //}}}
    //{{{ String Functions               format, substr, strip, split, join, rfind 
    //{{{ str functions
    __attribute__ ((format (printf, 1, 2))) 
    static inline string format(const char* fmt, ...) {
        char buffer[64];
        va_list args;
        va_start(args, fmt);
        int64 bytes;
        if ((bytes = vsnprintf(buffer, 64, fmt, args)) < 64) {
            va_end(args);
            return string(buffer);
        }
        va_end(args);

        va_list again;
        va_start(again, fmt);
        char* scratch = alloc<char>(bytes*sizeof(char));
        vsnprintf(scratch, bytes + 1, fmt, again);
        va_end(again);
        string result(scratch);
        free(scratch);
        return result;
    }

    static inline string substr(const string& ss, int64 off, int64 len) {
        string result(len);
        memcpy(result.ptr(), ss.data() + off, len);
        return result;
    }

    static inline string strip(const string& text) {
        // XXX: Is this correct for UTF-8 codes?
        const char* ptr = text.data();
        int64 off = 0;
        int64 len = text.size();

        while (len > 0 && ::isspace(ptr[off])) { off++; len--; }
        while (len > 0 && ::isspace(ptr[off + len - 1])) { len--; }

        return substr(text, off, len);
    }

    static inline list<string> split(const string& text) {
        int64 off = 0;
        int64 len = text.size();
        const char* ptr = text.data();

        while (len > 0 && isspace(ptr[off])) { off++; len--; }

        list<string> result;

        while (len > 0) {
            int64 cut = 0;
            while (cut < len && !isspace(ptr[off + cut])) { cut++; }
            if (cut > 0) result.append(substr(text, off, cut));
            off += cut;
            len -= cut;
            while (len > 0 && isspace(ptr[off])) { off++; len--; }
        }

        return result;
    }

    /* XXX: finish this?
    static inline list<string> split(const string& text, const string& regex, int64 max = -1) {
        list<string> result;
        int64 off = 0;

        regex_t pattern;
        check(regcomp(&pattern, regex.data(), REG_EXTENDED) == 0, "compiling regex");
        for (;;) {
            regmatch_t match;
            int rc = regexec(&pattern, text.data() + off, 1, &match, 0);
            if (rc == REG_NOMATCH) break;
            result.append(substr(text, off, match.rm_so));
            off += match.rm_eo;

            if (max >= 0 && result.size() >= max - 1) {
                break;
            }

        }
        regfree(&pattern);

        result.append(substr(text, off)); // XXX: need the length

        return result;
    }
    */

    static inline string join(const list<string>& ll) {
        int64 max = 0;
        for (int64 ii = 0; ii<ll.size(); ii++) {
            max += ll[ii].size();
        }
        string result(max);
        char* ptr = result.ptr();
        for (int64 ii = 0; ii<ll.size(); ii++) {
            int64 len = ll[ii].size();
            ::memcpy(ptr, ll[ii].data(), len);
            ptr += len;
        }
        return result;
    }

    static inline int64 rfind(const string& haystack, const string& needle) {
        int64 hlen = haystack.size();
        int64 nlen = needle.size();
        int64 off = hlen - nlen;
        while (off >= 0) {
            if (memcmp(haystack.data() + off, needle.data(), nlen) == 0) {
                return off;
            }
            --off;
        }
        return -1;
    }

    static inline void swap(string& flip, string& flop) {
        swap(flip.storage, flop.storage);
    }

    static inline size_t hash(const string& ss) {
        return hash(ss.data(), ss.size(), 0);
    }
    //}}}
    //}}}
    //{{{ Arithmetic Containers          complex, vector, matrix 
    //{{{ complex  

    //{{{ struct complex<T> 
    template<class type>
    struct complex {
        ~complex();
        complex();
        template<class tt>
        complex(const tt& other);
        complex(const type& other);
        complex(const type& re, const type& im);

        template<class tt>
        complex(const complex<tt>& other);
        complex(const complex<type>& other);

        template<class tt>
        complex<type>& operator =(const tt& other);
        complex<type>& operator =(const type& other);

        template<class tt>
        complex<type>& operator =(const complex<tt>& other);
        complex<type>& operator =(const complex<type>& other);

        // these are intentionally public and not
        // initialized in the default constructor
        type re, im;
    };

    template<class type>
    complex<type>::~complex() {}

    template<class type>
    complex<type>::complex() {}

    template<class type>
    template<class tt>
    complex<type>::complex(const tt& other) : re(other), im(0) {}

    template<class type>
    complex<type>::complex(const type& other) : re(other), im(0) {}

    template<class type>
    complex<type>::complex(const type& re, const type& im) : re(re), im(im) {}

    template<class type>
    template<class tt>
    complex<type>::complex(const complex<tt>& other) : re(other.re), im(other.im) {}

    template<class type>
    complex<type>::complex(const complex<type>& other) : re(other.re), im(other.im) {}

    template<class type>
    template<class tt>
    complex<type>& complex<type>::operator =(const tt& other) {
        re = other;
        im = 0;
        return *this;
    }

    template<class type>
    complex<type>& complex<type>::operator =(const type& other) {
        re = other;
        im = 0;
        return *this;
    }

    template<class type>
    template<class tt>
    complex<type>& complex<type>::operator =(const complex<tt>& other) {
        re = other.re;
        im = other.im;
        return *this;
    }

    template<class type>
    complex<type>& complex<type>::operator =(const complex<type>& other) {
        re = other.re;
        im = other.im;
        return *this;
    }

    //}}}
    //{{{ complex promotions 

    // Augment the type promotions for complex types
    template<class t0, class t1> struct arithmetic<complex<t0>, complex<t1> > {
        // promote complex and complex
        typedef complex<typename arithmetic<t0, t1>::type> type; 
    };
    template<class t0, class t1> struct arithmetic<complex<t0>, t1> {
        // promote complex and real
        typedef complex<typename arithmetic<t0, t1>::type> type; 
    };
    template<class t0, class t1> struct arithmetic<t0, complex<t1> > {
        // promote real and complex
        typedef complex<typename arithmetic<t0, t1>::type> type; 
    };

    //}}}
    //{{{ complex typedefs 

    // common complex types
    typedef complex<int8_t>  cbyte;
    typedef complex<int16_t> cshort;
    typedef complex<float>   cfloat;
    typedef complex<double>  cdouble;

    //}}}

    //}}}
    //{{{ vector 
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
    //}}}
    //{{{ matrix 
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
    //}}}
    //}}}
    //{{{ Complex Operators:             operator + - * / etc
    //{{{ unary complex ops 
    
    //{{{ prefix - 
    template<class type>
    complex<type> operator -(const complex<type>& zz) {
        return complex<type>(-zz.re, -zz.im);
    }
    //}}}
    //{{{ prefix + 
    template<class type>
    complex<type> operator +(const complex<type>& zz) {
        return zz;
    }
    //}}}

    //}}}
    //{{{ scalar scalar ops 

    //{{{ binary == 
    template<class atype, class btype>
    bool operator ==(const complex<atype>& aa, const complex<btype>& bb) {
        return (aa.re == bb.re) && (aa.im == bb.im);
    }

    template<class atype, class btype>
    bool operator ==(const atype& aa, const complex<btype>& bb) {
        return (aa == bb.re) && (0 == bb.im);
    }

    template<class atype, class btype>
    bool operator ==(const complex<atype>& aa, const btype& bb) {
        return (aa.re == bb) && (aa.im == 0);
    }
    //}}}
    //{{{ binary != 
    template<class atype, class btype>
    bool operator !=(const complex<atype>& aa, const complex<btype>& bb) {
        return (aa.re != bb.re) || (aa.im != bb.im);
    }

    template<class atype, class btype>
    bool operator !=(const atype& aa, const complex<btype>& bb) {
        return (aa != bb.re) || (0 != bb.im);
    }

    template<class atype, class btype>
    bool operator !=(const complex<atype>& aa, const btype& bb) {
        return (aa.re != bb) || (aa.im != 0);
    }
    //}}}
    //{{{ binary + 
    template<class atype, class btype>
    complex<typename arithmetic<atype, btype>::type> operator +(
        complex<atype> aa, complex<btype> bb
    ) {
        return complex<typename arithmetic<atype, btype>::type>(
            aa.re + bb.re, aa.im + bb.im
        );
    }

    template<class atype, class btype>
    complex<typename arithmetic<atype, btype>::type> operator +(
        atype aa, complex<btype> bb
    ) {
        return complex<typename arithmetic<atype, btype>::type>(
            aa + bb.re, bb.im
        );
    }

    template<class atype, class btype>
    complex<typename arithmetic<atype, btype>::type> operator +(
        complex<atype> aa, btype bb
    ) {
        return complex<typename arithmetic<atype, btype>::type>(
            aa.re + bb, aa.im
        );
    }
    //}}}
    //{{{ binary - 
    template<class atype, class btype>
    complex<typename arithmetic<atype, btype>::type> operator -(
        complex<atype> aa, complex<btype> bb
    ) {
        return complex<typename arithmetic<atype, btype>::type>(
            aa.re - bb.re, aa.im - bb.im
        );
    }

    template<class atype, class btype>
    complex<typename arithmetic<atype, btype>::type> operator -(
        atype aa, complex<btype> bb
    ) {
        return complex<typename arithmetic<atype, btype>::type>(
            aa - bb.re, -bb.im
        );
    }

    template<class atype, class btype>
    complex<typename arithmetic<atype, btype>::type> operator -(
        complex<atype> aa, btype bb
    ) {
        return complex<typename arithmetic<atype, btype>::type>(
            aa.re - bb, aa.im
        );
    }
    //}}}
    //{{{ binary * 
    template<class atype, class btype>
    complex<typename arithmetic<atype, btype>::type> operator *(
        complex<atype> aa, complex<btype> bb
    ) {
        return complex<typename arithmetic<atype, btype>::type>(
            aa.re*bb.re - aa.im*bb.im,
            aa.re*bb.im + aa.im*bb.re
        );
    }

    template<class atype, class btype>
    complex<typename arithmetic<atype, btype>::type> operator *(
        atype aa, complex<btype> bb
    ) {
        return complex<typename arithmetic<atype, btype>::type>(
            aa*bb.re, aa*bb.im
        );
    }

    template<class atype, class btype>
    complex<typename arithmetic<atype, btype>::type> operator *(
        complex<atype> aa, btype bb
    ) {
        return complex<typename arithmetic<atype, btype>::type>(
            aa.re*bb, aa.im*bb
        );
    }
    //}}}
    //{{{ binary / 
    namespace internal {
        static inline void smith_division(
            double aa, double bb, double cc, double dd, double& ee, double& ff
        ) {
            // XXX: Switch this to using ilogb() and scalbn() to normalize.
            // See _Cdivd() in http://www.open-std.org/jtc1/sc22/wg14/www/docs/n1124.pdf
            if (fabs(cc) < fabs(dd)) {
                double rr = cc/dd;
                double ll = cc*rr + dd;
                ee = (aa*rr + bb)/ll;
                ff = (bb*rr - aa)/ll;
            } else {
                double rr = dd/cc;
                double ll = cc + dd*rr;
                ee = (aa + bb*rr)/ll;
                ff = (bb - aa*rr)/ll;
            }
        }
    }

    template<class atype, class btype>
    complex<typename arithmetic<atype, btype>::type> operator /(
        complex<atype> aa, complex<btype> bb
    ) {
        using namespace internal;
        double ee, ff;
        smith_division(aa.re, aa.im, bb.re, bb.im, ee, ff);
        return complex<typename arithmetic<atype, btype>::type>(ee, ff);
    }

    template<class atype, class btype>
    complex<typename arithmetic<atype, btype>::type> operator /(
        atype aa, complex<btype> bb
    ) {
        using namespace internal;
        double ee, ff;
        smith_division(aa, 0, bb.re, bb.im, ee, ff);
        return complex<typename arithmetic<atype, btype>::type>(ee, ff);
    }

    template<class atype, class btype>
    complex<typename arithmetic<atype, btype>::type> operator /(
        complex<atype> aa, btype bb
    ) {
        return complex<typename arithmetic<atype, btype>::type>(
            aa.re/bb, aa.im/bb
        );
    }
    //}}}
    //{{{ operator += 
    template<class atype, class btype>
    complex<atype>& operator += (complex<atype>& aa, const complex<btype>& bb) {
        return aa = aa + bb;
    }

    template<class atype, class btype>
    complex<atype>& operator += (complex<atype>& aa, const btype& bb) {
        return aa = aa + bb;
    }
    //}}}
    //{{{ operator -= 
    template<class atype, class btype>
    complex<atype>& operator -= (complex<atype>& aa, const complex<btype>& bb) {
        return aa = aa - bb;
    }

    template<class atype, class btype>
    complex<atype>& operator -= (complex<atype>& aa, const btype& bb) {
        return aa = aa - bb;
    }
    //}}}
    //{{{ operator *= 
    template<class atype, class btype>
    complex<atype>& operator *= (complex<atype>& aa, const complex<btype>& bb) {
        return aa = aa * bb;
    }

    template<class atype, class btype>
    complex<atype>& operator *= (complex<atype>& aa, const btype& bb) {
        return aa = aa * bb;
    }
    //}}}
    //{{{ operator /= 
    template<class atype, class btype>
    complex<atype>& operator /= (complex<atype>& aa, const complex<btype>& bb) {
        return aa = aa / bb;
    }

    template<class atype, class btype>
    complex<atype>& operator /= (complex<atype>& aa, const btype& bb) {
        return aa = aa / bb;
    }
    //}}}

    //}}}
    //}}}
    //{{{ Scalar, Vector, Matrix Ops:    operator + - * / % etc 
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
    //}}}
    //{{{ Complex Functions:             real, imag, conj, mag, mag2, exp, log, sqr 

    template<class type>
    type real(const complex<type>& aa) {
        return aa.re;
    }

    template<class type>
    type real(const type& aa) {
        return aa;
    }

    template<class type>
    type imag(const complex<type>& aa) {
        return aa.im;
    }

    template<class type>
    type imag(const type& aa) {
        (void)aa;
        return 0;
    }

    template<class type>
    complex<type> conj(const complex<type>& aa) {
        return complex<type>(aa.re, -aa.im);
    }

    template<class type>
    type conj(const type& aa) {
        return aa;
    }

    template<class type>
    type mag2(const complex<type>& aa) {
        return aa.re*aa.re + aa.im*aa.im;
    }

    template<class type>
    type mag2(const type& aa) {
        return aa*aa;
    }

    template<class atype, class btype>
    double hypot(const complex<atype>& aa, const complex<btype>& bb) {
        return ::hypot(::hypot(aa.re, aa.im), ::hypot(bb.re, bb.im));
    }

    template<class atype, class btype>
    double hypot(const atype& aa, const complex<btype>& bb) {
        return ::hypot(aa, ::hypot(bb.re, bb.im));
    }

    template<class atype, class btype>
    double hypot(const complex<atype>& aa, const btype& bb) {
        return ::hypot(::hypot(aa.re, aa.im), bb);
    }

    template<class atype, class btype>
    double hypot(const atype& aa, const btype& bb) {
        return ::hypot(aa, bb);
    }

    template<class type>
    type mag(const complex<type>& aa) {
        return ::hypot(aa.re, aa.im);
    }

    template<class type>
    type mag(const type& aa) {
        return aa < 0 ? -aa : aa;
    }

    template<class type>
    type phase(const complex<type>& aa) {
        return ::atan2(aa.im, aa.re);
    }

    template<class type>
    type phase(const type& aa) {
        return aa < 0 ? M_PI : 0;
    }

    template<class type>
    complex<type> expj(type aa) {
        return complex<type>(::cos(aa), ::sin(aa));
    }

    template<class type>
    complex<type> exp(complex<type> zz) {
        double ee = ::exp(zz.re);
        return complex<type>(
            ee*::cos(zz.im),
            ee*::sin(zz.im)
        );
    }

    template<class type>
    type exp(type aa) {
        return ::exp(aa);
    }

    template<class type>
    complex<type> log(const complex<type>& zz) {
        return complex<type>(
            ::log(::hypot(zz.re, zz.im)),
            ::atan2(zz.im, zz.re)
        );
    }

    template<class type>
    type log(type aa) {
        return ::log(aa);
    }

    template<class type>
    static type sqr(type xx) { return xx*xx; }

    //}}}
    //{{{ Vector Functions:              (inner), (outer) 
    // XXX: inner(a, b)
    // XXX: outer(a, b)
    
    /* XXX: rename to setzero?
    template<class type>
    void zero(vector<type>& aa) {
        const int64 size = aa.size();
        for (int64 ii = 0; ii<size; ii++) {
            aa[ii] = 0;
        }
    }
    */

    /* XXX
    template<class type>
    std::ostream& operator <<(std::ostream& os, const vector<type>& vv) {
        for (int64 ii = 0; ii<vv.size(); ii++) {
            os << " " << vv[ii] << std::endl;
        }

        return os;
    }
    */
    //}}}
    //{{{ Matrix Functions:              diag, conj, trans, herm
    
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

    //}}}
    //{{{ Matrix Decompositions:         (qrdecomp), (lqdecomp), (lddecomp) 
    //}}}
    //{{{ Cholesky Functions:            cholesky, chokupdate, cholunitize 
    
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

    //}}}
    //{{{ Linear Solvers:                gensolve, (symsolve) 
    //{{{ gensolve

    namespace internal {

        //
        // "The Complex Householder Transform", Kuo-Liang Chung and Wen-Ming Yan
        // IEEE Transactions on Sig Proc, Vol. 45, No. 9, September 1997
        // 
        // There are two pairs of Householder functions here.  One pair works
        // on columns from the left of the matrix, and the other works on rows
        // form the right.  The first function of each pair builds the
        // Householder vector and modifies the original matrix.  The second
        // function can be used to apply that Householder vector to other
        // matrices.  The second function also has an optional last argument
        // which is used to determine whether you're trying to apply the actual
        // Householder transform, or if you're trying to recover the matrix for
        // a decomposition.  Note that for decompositions, the transforms must
        // be applied in reverse order to recover the combined Householder
        // matrix (Q below).
        //
        // For a least squares problems:
        // 
        //     Assume we start with:              A X = B
        //     householder_col(A, ...) implements:     (H A)
        //     householder_col(A, ,,, B, ...) gives:   (H B)
        //     allowing us to make progress on:  H A X = H B
        //
        // For matrix decompositions:
        //
        //     Assume we start with:     A and I
        //     householder_col(A, ...) implements:     R = (H A)
        //     householder_col(A, ..., Q, ..., true):   Q = (H I)
        //
        // See gensolve(), qrdecomp(), and lqdecomp() for examples.
        //

        // Apply a Householder col from A to cols of B starting at low
        template<class type>
        void householder_col(
            const type* A, int64 arows, int64 acols,
                  type* B, int64 brows, int64 bcols,
            int64 row, int64 col, int64 low=0, bool recover=false
        ) {
            check(arows == brows, "matching rows");
            type pivot = A[row*acols + col];
            if (pivot == 0) return;
            if (recover) pivot = conj(pivot);
            type scale = 1.0/pivot;
            for (int64 jj = low; jj<bcols; jj++) {
                type sum = 0;
                for (int64 ii = row; ii<arows; ii++) {
                    sum += B[ii*bcols + jj] * conj(A[ii*acols + col]);
                }
                sum *= scale;
                for (int64 ii = row; ii<arows; ii++) {
                    B[ii*bcols + jj] += sum * A[ii*acols + col];
                }
            }
        }

        template<class type>
        double householder_col(
            type* H, int64 rows, int64 cols,
            int64 row, int64 col
        ) {
            // calculate the vector magnitude
            double magr = 0;
            for (int64 ii = row; ii<rows; ii++) {
                magr = hypot(magr, H[ii*cols + col]);
            }
            if (magr == 0) return 0; // it's a zero col
            if (real(H[row*cols + col]) > 0) magr = -magr;
            double invr = 1/magr;
            for (int64 ii = row; ii<rows; ii++) {
                H[ii*cols + col] *= invr;
            }
            H[row*cols + col] -= 1;
            householder_col(H, rows, cols, H, rows, cols, row, col, col + 1);
            return magr;
        }

        // Apply a Household row from H to rows of A starting at low
        template<class type>
        void householder_row(
            const type* A, int64 arows, int64 acols,
                  type* B, int64 brows, int64 bcols,
            int64 row, int64 col, int64 low=0, bool recover=false
        ) {
            (void)arows; // not needed, but it looks nicer
            check(acols == bcols, "matching cols");
            type pivot = A[row*acols + col];
            if (pivot == 0) return;
            if (recover) pivot = conj(pivot);
            type scale = 1.0/pivot;
            for (int64 ii = low; ii<brows; ii++) {
                type sum = 0;
                for (int64 jj = col; jj<acols; jj++) {
                    sum += B[ii*bcols + jj] * conj(A[row*acols + jj]);
                }
                sum *= scale;
                for (int64 jj = col; jj<acols; jj++) {
                    B[ii*bcols + jj] += sum * A[row*acols + jj];
                }
            }
        }

        template<class type>
        double householder_row(
            type* H, int64 rows, int64 cols,
            int64 row, int64 col
        ) {
            // calculate the vector magnitude
            double magr = 0;
            for (int64 jj = col; jj<cols; jj++) {
                magr = hypot(magr, H[row*cols + jj]);
            }
            if (magr == 0) return 0; // it's a zero row
            if (real(H[row*cols + col]) > 0) magr = -magr;
            double invr = 1/magr;
            for (int64 jj = col; jj<cols; jj++) {
                H[row*cols + jj] *= invr;
            }
            H[row*cols + col] -= 1;
            householder_row(H, rows, cols, H, rows, cols, row, col, row + 1);
            return magr;
        }

        // Special Householder for the gensolve() function
        template<class type>
        void gensolve_row_col(
            const type* A, int64 arows, int64 acols,
                  type* R, int64 rrows, int64 rcols,
            int64 row, int64 col, int64 low=0
        ) {
            (void)arows; // not needed, but it looks nicer
            check(acols == rrows, "matching sizes");
            type pivot = A[row*acols + col];
            if (pivot == 0) return;
            type scale = 1.0/pivot;
            for (int64 jj = low; jj<rcols; jj++) {
                type sum = 0;
                for (int64 kk = col; kk<acols; kk++) {
                    sum += R[kk*rcols + jj] * A[row*acols + kk];
                }
                sum *= scale;
                for (int64 kk = col; kk<acols; kk++) {
                    R[kk*rcols + jj] += sum * conj(A[row*acols + kk]);
                }
            }
        }

        // Special Householder for upper_hessenber
        template<class type>
        void hessenberg_col_row(
            const type* A, int64 arows, int64 acols,
                  type* L, int64 lrows, int64 lcols,
            int64 row, int64 col, int64 low=0
        ) {
            check(arows == lcols, "matching sizes");
            type pivot = A[row*acols + col];
            if (pivot == 0) return;
            pivot = conj(pivot);
            type scale = 1.0/pivot;
            for (int64 ii = low; ii<lrows; ii++) {
                type sum = 0;
                for (int64 kk = row; kk<arows; kk++) {
                    sum += L[ii*lcols + kk] * A[kk*acols + col];
                }
                sum *= scale;
                for (int64 kk = row; kk<arows; kk++) {
                    L[ii*lcols + kk] += sum * conj(A[kk*acols + col]);
                }
            }
        }
    }

    //
    // Solve: A X = B, Given: A[rows*cols], B[max(rows, cols)*size]
    // 
    // This function finds the least-squares or least-norm solution to the
    // system of linear equations.  Both A and B will be modified with the
    // final result in the top of B.
    //
    // If provided, P is used as scratch and must be P[min(rows, cols)] large.
    // If not provided, the routine will allocate and free temporary space.
    //
    template<class type>
    int64 gensolve(
        type* A, int64 rows, int64 cols, 
        type* B, int64 size, double tol=1e-12,
        int64* P=0
    ) {
        using namespace internal;
        int64 rank = min(rows, cols);
        vector<int64> temp;
        int64* perm = P ? P : (temp.resize(rank), temp.data());

        // zero out the extra part of B, only if rows < cols
        for (int64 ii = rows; ii<cols; ii++) {
            for (int64 jj = 0; jj<size; jj++) {
                B[ii*size + jj] = 0;
            }
        }

        // do a rank-revealing QR decomposition
        for (int64 ij = 0; ij<rank; ij++) {
            // find largest col for pivoting
            perm[ij] = -1;
            double maxsum = -1;
            for (int64 jj = ij; jj<cols; jj++) {
                double sum = 0;
                for (int64 ii = ij; ii<rows; ii++) {
                    sum = hypot(sum, A[ii*cols + jj]);
                }
                if (sum > maxsum) {
                    perm[ij] = jj;
                    maxsum = sum;
                }
            }

            if (ij != perm[ij]) {
                // do the column pivot
                for (int64 ii = 0; ii<rows; ii++) {
                    swap(A[ii*cols + ij], A[ii*cols + perm[ij]]);
                }
            }

            // do the Householder transform to A and B
            double diag = householder_col(A, rows, cols, ij, ij);
            householder_col(A, rows, cols, B, rows, size, ij, ij);

            // remove the Householder vector we dumped into A
            A[ij*cols + ij] = diag;
            for (int64 jj = 0; jj<ij; jj++) {
                A[ij*cols + jj] = 0;
            }

            // Compare to our tolerance to find the rank.  Note: On the first
            // pass, his can only be true if the diagonal is actually zero, in
            // which case our rank really is zero.
            if (::fabs(diag) <= tol*::fabs(real(A[0]))) {
                rank = ij;
                break;
            }
        }

        // do a bi-diagonal decomposition (diagonal and sub-diagonal)
        for (int64 ij = 0; ij<rank - 1; ij++) {
            double diag = householder_row(A, rank, cols, ij, ij);
            double offd = householder_col(A, rank, cols, ij+1, ij);
            householder_col(A, rank, cols, B, rank, size, ij+1, ij);
            // forward ellimation with the diagonal and sub-diagonal
            double scale = 1.0/diag;
            for (int64 jj = 0; jj<size; jj++) {
                B[ij*size + jj] *= scale;
                B[(ij + 1)*size + jj] -= offd*B[ij*size + jj];
            }
        }
        // do the final division for the last diagonal
        double diag = householder_row(A, rows, cols, rank - 1, rank - 1);
        double scale = 1.0/diag;
        for (int64 jj = 0; jj<size; jj++) {
            B[(rank - 1)*size + jj] *= scale;
        }

        // apply our householder rows to X
        for (int64 ij = rank - 1; ij>=0; ij--) {
            inverse_row_col(A, rank, cols, B, cols, size, ij, ij);
        }

        // apply our permutations to X
        for (int64 ii = rank - 1; ii>=0; ii--) {
            if (perm[ii] != ii) {
                for (int64 jj = 0; jj<size; jj++) {
                    swap(B[ii*size + jj], B[perm[ii]*size + jj]);
                }
            }
        }

        return rank;
    }



    //}}}
    //}}}
    //{{{ Eigen and SV Decomps:          svdecomp, (symeigens), (geneigens) 
    //{{{ svdecomp
    namespace internal {

        struct givens_rotation {
            double rr, cc, ss;
            givens_rotation(double r0, double c0, double s0) :
                rr(r0), cc(c0), ss(s0) {}
            givens_rotation(double xx, double yy) {
                rr = hypot(xx, yy);
                if (rr <= 0) {
                    cc = 1;
                    ss = 0;
                } else {
                    cc = xx/rr;
                    ss = yy/rr;
                }
            }
            void rotate(double& xx, double& yy) {
                double uu =  xx*cc + yy*ss;
                double vv = -xx*ss + yy*cc;
                xx = uu;
                yy = vv;
            }
            void rotate(double* aa, int64 ii, int64 size) {
                if (!aa) return;
                double* lo = aa + (ii + 0)*size;
                double* hi = aa + (ii + 1)*size;
                for (int64 jj = 0; jj<size; jj++) {
                    rotate(lo[jj], hi[jj]);
                }
            }
        };

        static void reverse_submatrix(
            double* dd, double* ff, int64 lo, int64 hi,
            double* ll, double* rr, int64 size
        ) {
            for (int64 ii = 0; ii<(hi - lo)/2; ii++) {
                xm::swap(dd[lo + ii], dd[hi - 1 - ii]);
                if (ll) for (int64 jj = 0; jj<size; jj++) {
                    xm::swap(ll[(lo + ii)*size + jj], ll[(hi - 1 - ii)*size + jj]);
                }
                if (rr) for (int64 jj = 0; jj<size; jj++) {
                    xm::swap(rr[(lo + ii)*size + jj], rr[(hi - 1 - ii)*size + jj]);
                }
            }
            for (int64 ii = 0; ii<(hi - 1 - lo)/2; ii++) {
                xm::swap(ff[lo + ii], ff[hi - 2 - ii]);
            }
            for (int64 ii = lo; ii<hi - 1; ii++) {
                givens_rotation gg(dd[ii], ff[ii]);
                dd[ii] = gg.rr; ff[ii] = 0;
                gg.rotate(ff[ii], dd[ii+1]);
                gg.rotate(ll, ii, size);
            }
        }

        // Golub and Van Loan, "Matrix Computations", 4th edition, Section 8.3.4
        static givens_rotation wilkinson_shift(
            double* dd, double* ff, int64 lo, int64 hi
        ) {
            double aa = dd[hi - 2]*dd[hi - 2];
            double bb = dd[hi - 2]*ff[hi - 2];
            double cc = ff[hi - 2]*ff[hi - 2] + dd[hi - 1]*dd[hi - 1];
            double ee = 0.5*(aa - cc);
            double uu = cc + ee - copysign(1, ee)*hypot(ee, bb);
            check(dd[lo] != 0, "no divides by zero");
            return givens_rotation(dd[lo] - uu/dd[lo], ff[lo]);
            //return givens_rotation(dd[lo]*dd[lo] - uu, dd[lo]*ff[lo]);
        }

        //
        // Golub and Van Loan, "Matrix Computations", 4th edition, Section 8.6.1
        //
        //  +----------------------------------+  +----------------------------------+
        //  |                                  |  |                                  |
        //  | bulge chasing:                   |  |  sweep top:   sweep row:         |
        //  |                                  |  |                                  |
        //  |  d0 <-- f0 <-- b1                |  |   d0 <- f0     di   fi   bj+1    |
        //  |   ^      ^      ^                |  |                 ^    ^    ^      |
        //  |   |      |      |                |  |   b0 <- d1      |    |    |      |
        //  |  b0 <-- d1 <-- f1 <-- b3         |  |                bj   di+1 fi+1    |
        //  |          ^      ^      ^         |  |                                  |
        //  |          |      |      |         |  |                                  |
        //  |         b2 <-- d2 <-- f2 <-- b5  |  |  sweep col:                      |
        //  |                 ^      ^      ^  |  |                     sweep end:   |
        //  |                 |      |      |  |  |     fi <-- bj                    |
        //  |                b4 <-- d3 <-- f3  |  |                      d3   f3     |
        //  |                        ^      ^  |  |   di+1 <-- fi+1       ^    ^     |
        //  |                        |      |  |  |                       |    |     |
        //  |                       b6 <-- d4  |  |   bj+1 <-- di+2      b6   d4     |
        //  |                                  |  |                                  |
        //  +----------------------------------+  +----------------------------------+
        //
        static inline void golub_kahan(
            double* dd, double* ff, int64 lo, int64 hi,
            double* ll, double* rr, int64 size
        ) {
            if (3*fabs(dd[lo]) < fabs(dd[hi - 1])) {
                reverse_submatrix(dd, ff, lo, hi, ll, rr, size);
            }

            // sweep top
            double bb = 0;
            givens_rotation gt = wilkinson_shift(dd, ff, lo, hi);
            gt.rotate(dd[lo], ff[lo]);
            gt.rotate(bb, dd[lo + 1]);
            gt.rotate(rr, lo, size);

            for (int64 ii = lo; ii<hi-2; ii++) {
                // sweep row
                givens_rotation gr(dd[ii], bb);
                dd[ii] = gr.rr; bb = 0;
                gr.rotate(ff[ii], dd[ii + 1]);
                gr.rotate(bb, ff[ii + 1]);
                gr.rotate(ll, ii, size);

                // sweep col
                givens_rotation gc(ff[ii], bb);
                ff[ii] = gc.rr; bb = 0;
                gc.rotate(dd[ii + 1], ff[ii + 1]);
                gc.rotate(bb, dd[ii + 2]);
                gc.rotate(rr, ii + 1, size);
            }

            // sweep end
            givens_rotation ge(dd[hi - 2], bb);
            dd[hi - 2] = ge.rr;
            ge.rotate(ff[hi - 2], dd[hi - 1]);
            ge.rotate(ll, hi - 2, size);
        }

        static inline void split_diagonal(
            double* dd, double* ff, int64 lo, int64 hi,
            double* ll, double* rr, int64 size
        ) {
            for (int64 ii = lo; ii<hi - 1; ii++) {
                givens_rotation gg(dd[ii], ff[ii]);
                dd[ii] = gg.rr; ff[ii] = 0;
                gg.rotate(ff[ii], dd[ii+1]);
                gg.rotate(rr, ii, size);
            }
            for (int64 ii = lo; ii<hi - 1; ii++) {
                givens_rotation gg(dd[ii], ff[ii]);
                dd[ii] = gg.rr; ff[ii] = 0;
                gg.rotate(ff[ii], dd[ii+1]);
                gg.rotate(ll, ii, size);
            }
        }

        //
        // Loosely from "Accuracy of two SVD algorithms for 2x2 triangular
        // matrices", by V. Harii and J. Matejas, in Applied Mathematics and
        // Computation 2009.
        //
        static void upper_2x2_svd(
            double* dd, double* ff, int64 lo,
            double* ll, double* rr, int64 size
        ) {
            double aa = dd[lo + 0];
            double bb = ff[lo + 0];
            double cc = dd[lo + 1];

            // Check if it's already diagonal
            if (bb == 0) return;

            if (aa == 0) { // shortcut
                double hh = hypot(bb, cc);
                dd[lo] = ff[lo] = 0;
                dd[lo + 1] = hh;
                if (ll) {
                    givens_rotation gg(0, cc/hh, -bb/hh);
                    gg.rotate(ll, lo, size);
                }
                return;
            }

            if (cc == 0) { // shortcut
                double hh = hypot(aa, bb);
                dd[lo] = hh;
                ff[lo] = dd[lo + 1] = 0;
                if (rr) {
                    givens_rotation gg(0, aa/hh, bb/hh);
                    gg.rotate(rr, lo, size);
                }
                return;
            }

            double a2 = aa*aa;
            double b2 = bb*bb;
            double c2 = cc*cc;

            double Rc, Rs, Lc, Ls;
            if (a2 > c2) {
                double L = .5*atan2(-2*bb*cc, a2 + b2 - c2);
                Lc = cos(L);
                Ls = sin(L);
                double xx, yy;
                if (fabs(Ls*bb + Lc*cc) > fabs(Lc*bb - Ls*cc)) {
                    xx = Ls*bb + Lc*cc;
                    yy = -Ls*aa;
                } else {
                    xx = Lc*aa;
                    yy = Lc*bb - Ls*cc;
                }
                double hh = hypot(xx, yy);
                Rc = xx/hh;
                Rs = yy/hh;
            } else {
                double R = .5*atan2(-2*aa*bb, c2 + b2 - a2);
                Rc = cos(R);
                Rs = sin(R);
                double xx, yy;
                if (fabs(Rc*aa + Rs*bb) > fabs(Rc*bb - Rs*aa)) {
                    xx = Rc*aa + Rs*bb;
                    yy = -Rs*cc;
                } else {
                    xx = Rc*cc;
                    yy = Rc*bb - Rs*aa;
                }
                double hh = hypot(xx, yy);
                Lc = xx/hh;
                Ls = yy/hh;
            }

            dd[lo + 0] =  Rc*Lc*aa + Rs*Lc*bb - Rs*Ls*cc;
            dd[lo + 1] = -Rs*Ls*aa + Rc*Ls*bb + Rc*Lc*cc;
            ff[lo] = 0;

            if (ll) {
                givens_rotation gg(0, Lc, -Ls);
                gg.rotate(ll, lo, size);
            }

            if (rr) {
                givens_rotation gg(0, Rc, Rs);
                gg.rotate(rr, lo, size);
            }
        }

        static inline void block_diag_svd(
            double* dd, double* ff, int64 lo, int64 hi,
            double* ll, double* rr, int64 size
        ) {
            while (hi - lo > 2) {
                /*
                static int64 iteration;
                fprintf(stderr, "iter: %lld\n", ++iteration);
                fprintf(stderr, "     ff: ");
                for (int64 ii = 0; ii<size - 1; ii++) fprintf(stderr, "\t%lf", ff[ii]);
                fprintf(stderr, "\ndd: ");
                for (int64 ii = 0; ii<size; ii++) fprintf(stderr, "\t%lf", dd[ii]);
                fprintf(stderr, "\n");
                getchar();
                */

                const double eps = 1e-20;

                // look for opportunities to recurse from the off diagonal
                for (int64 ii = lo; ii<hi - 1; ii++) {
                    if (fabs(ff[ii]) < eps*(fabs(dd[ii]) + fabs(dd[ii + 1]))) {
                        if ((ii - lo) < (hi - lo)/2) {
                            block_diag_svd(dd, ff, lo, ii + 1, ll, rr, size);
                            lo = ii + 1;
                        } else {
                            block_diag_svd(dd, ff, ii + 1, hi, ll, rr, size);
                            hi = ii + 1;
                            break;
                        }
                    }
                }

                // look for opportunities to recurse from the main diagonal
                for (int64 ii = lo; ii<hi; ii++) {
                    double f0 = (ii == lo) ? 0 : fabs(ff[ii - 1]);
                    double f1 = (ii == hi - 1) ? 0 : fabs(ff[ii]);
                    if (fabs(dd[ii]) < eps*(f0 + f1)) {
                        split_diagonal(dd, ff, lo, hi, ll, rr, size);
                        if ((ii - lo) < (hi - lo)/2) {
                            block_diag_svd(dd, ff, lo, ii, ll, rr, size);
                            lo = ii; hi = hi - 1;
                        } else {
                            block_diag_svd(dd, ff, ii, hi - 1, ll, rr, size);
                            hi = ii;
                            break;
                        }
                    }
                }

                if (hi - lo > 2) golub_kahan(dd, ff, lo, hi, ll, rr, size);
            }
            
            if (hi - lo == 2) upper_2x2_svd(dd, ff, lo, ll, rr, size);
        }

        struct compare_singular_vals {
            const double* pp;
            compare_singular_vals(const double* dd) : pp(dd) {}
            bool operator ()(int64 ii, int64 jj) {
                return pp[ii] > pp[jj];
            }
        };

        static inline void bidiagonal_svd(
            int64* tt, double* dd, double* ff,
            double* ll, double* rr, int64 size
        ) {
            // For numerical precision, we scale the array to near unity
            double frob = 0;
            for (int64 ii = 0; ii<size; ii++)   frob = hypot(frob, dd[ii]);
            for (int64 ii = 0; ii<size-1; ii++) frob = hypot(frob, ff[ii]);
            frob /= size;
            if (frob == 0) return;
            double inv = 1/frob;
            for (int64 ii = 0; ii<size; ii++)   dd[ii] *= inv;
            for (int64 ii = 0; ii<size-1; ii++) ff[ii] *= inv;

            block_diag_svd(dd, ff, 0, size, ll, rr, size);
            
            // negate any negative elements
            for (int64 ii = 0; ii<size; ii++) if (dd[ii] < 0) {
                dd[ii] *= -1;
                if (ll) for (int64 jj = 0; jj<size; jj++) {
                    ll[ii*size + jj] *= -1;
                }
            }

            // Swaps are expensive with large arrays, so we sort the indices first
            // and then do at most N swaps instead of O(N log N) in the sort.
            for (int64 ii = 0; ii<size; ii++) tt[ii] = ii;
            compare_singular_vals decreasing(dd);
            introsort(tt, size, decreasing);

            for (int64 ii = 0; ii<size; ii++) {
                int64 spot = ii;
                int64 want = tt[spot];
                while (spot != want) {
                    xm::swap(dd[spot], dd[want]);
                    if (ll) for (int64 jj = 0; jj<size; jj++) {
                        xm::swap(ll[spot*size + jj], ll[want*size + jj]);
                    }
                    if (rr) for (int64 jj = 0; jj<size; jj++) {
                        xm::swap(rr[spot*size + jj], rr[want*size + jj]);
                    }
                    tt[spot] = spot;
                    spot = want;
                    want = tt[spot];
                    if (want == ii) {
                        tt[spot] = spot;
                        break;
                    }
                }
            }

            // Correct for the scaling we did above
            for (int64 ii = 0; ii<size; ii++)   dd[ii] *= frob;
            for (int64 ii = 0; ii<size-1; ii++) ff[ii] *= frob;
        }
    }

    template<class type>
    void svdecomp(
        type* A, int64 rows, int64 cols,
        type* L, double* D, type* R,
        double* U=0, double* V=0,
        double* F=0, int64* T=0
    ) {
        using namespace internal;
        matrix<double> utmp, vtmp;
        vector<double> dtmp, ftmp;
        vector<int64> ttmp;

        const int64 size = min(rows, cols);
        double* uu = (L && !U) ? (utmp.resize(size, size), utmp.data()) : U;
        double* vv = (R && !V) ? (vtmp.resize(size, size), vtmp.data()) : V;
        double* dd = D ? D : (dtmp.resize(size), dtmp.data());
        double* ff = F ? F : (ftmp.resize(size), ftmp.data());
        int64*  tt = T ? T : (ttmp.resize(size), ttmp.data());

        if (L) {
            for (int64 ii = 0; ii<size; ii++) {
                for (int64 jj = 0; jj<size; jj++) {
                    uu[ii*size + jj] = (ii == jj);
                }
            }
        }
        if (R) {
            for (int64 ii = 0; ii<size; ii++) {
                for (int64 jj = 0; jj<size; jj++) {
                    vv[ii*size + jj] = (ii == jj);
                }
            }
        }

        if (rows >= cols) {
            // build it upper diagonal
            for (int64 ij = 0; ij<size; ij++) {
                dd[ij] = householder_col(A, rows, cols, ij, ij);
                ff[ij] = householder_row(A, rows, cols, ij, ij + 1);
            }
        } else {
            // build it lower diagonal
            for (int64 ij = 0; ij<size; ij++) {
                dd[ij] = householder_row(A, rows, cols, ij, ij);
                ff[ij] = householder_col(A, rows, cols, ij + 1, ij);
            }
            // givens rotate to upper diagonal
            for (int64 ij = 0; ij<size - 1; ij++) {
                givens_rotation gg(dd[ij], ff[ij]);
                dd[ij] = gg.rr; ff[ij] = 0;
                gg.rotate(ff[ij], dd[ij+1]);
                gg.rotate(L ? uu : 0, ij, size);
            }
        }

        bidiagonal_svd(tt, dd, ff, L ? uu : 0, R ? vv : 0, size);

        if (L) {
            for (int64 ii = 0; ii<size; ii++) {
                for (int64 jj = 0; jj<size; jj++) {
                    // this is intentionally transposed
                    L[ii*size + jj] = uu[jj*size + ii];
                }
            }
            for (int64 ii = size; ii<rows; ii++) {
                for (int64 jj = 0; jj<size; jj++) {
                    L[ii*size + jj] = 0;
                }
            }
            if (rows >= cols) {
                for (int64 ij = size-1; ij>=0; ij--) {
                    householder_col(
                        A, rows, cols, 
                        L, rows, size,
                        ij, ij, 0, true
                    );
                }
            } else {
                for (int64 ij = size-1; ij>=0; ij--) {
                    householder_col(
                        A, rows, cols,
                        L, rows, size,
                        ij + 1, ij, 0, true
                    );
                }
            }
        }

        if (R) {
            for (int64 ii = 0; ii<size; ii++) {
                for (int64 jj = 0; jj<size; jj++) {
                    R[ii*cols + jj] = vv[ii*size + jj];
                }
                for (int64 jj = size; jj<cols; jj++) {
                    R[ii*cols + jj] = 0;
                }
            }
            if (rows >= cols) {
                for (int64 ij = size-1; ij>=0; ij--) {
                    householder_row(
                        A, rows, cols,
                        R, size, cols,
                        ij, ij + 1, 0, true
                    );
                }
            } else {
                for (int64 ij = size-1; ij>=0; ij--) {
                    householder_row(
                        A, rows, cols,
                        R, size, cols,
                        ij, ij, 0, true
                    );
                }
            }
        }
    }
        
    template<class type>
    void svdecomp(
        const matrix<type>& A, matrix<type>* L,
        vector<double>* D, matrix<type>* R
    ) {
        const int64 size = min(A.rows(), A.cols());
        if (L) L->resize(A.rows(), size);
        if (D) D->resize(size);
        if (R) R->resize(size, A.cols());

        matrix<type> copy = A;
        svdecomp(
            copy.data(), A.rows(), A.cols(),
            L ? L->data() : 0,
            D ? D->data() : 0,
            R ? R->data() : 0
        );
    }

    //}}}
    //}}}
    //{{{ Root and Peak Finding:         findzero, quadroots, quadpeak, (polyroots) 
    namespace internal {
        static inline bool diffsign(double aa, double bb) {
            return (bool)signbit(aa) != (bool)signbit(bb);
        }
    }

    template<class callable>
    double findzero(callable func, double xlo, double xhi, double tol=0) {
        using namespace internal;
        if (xhi < xlo) swap(xlo, xhi);
        double ylo = func(xlo); if (ylo == 0) return xlo;
        double yhi = func(xhi); if (yhi == 0) return xhi;
        check(diffsign(yhi, ylo), "must have opposite signs");

        while (xhi - xlo > tol) {
            double half = .5*(xhi - xlo);
            double xmid = xlo + half;
            if (xmid == xlo || xmid == xhi) return xmid;
            double ymid = func(xmid); if (ymid == 0) return xmid;

            // We're using a variation of Ridder's method to find an
            // interpolated point, slightly modified to avoid underflow.
            double denom = ::hypot(ymid, ::sqrt(::fabs(ylo))*::sqrt(::fabs(yhi)));
            double xexp = xmid + half*::copysign(1, ylo - yhi)*ymid/denom;
            check(xlo <= xexp && xexp <= xhi, "in bounds");
            double yexp = func(xexp); if (yexp == 0) return xexp;

            double xx[4] = { xlo, xmid, xexp, xhi };
            double yy[4] = { ylo, ymid, yexp, yhi };
            if (xexp < xmid) { swap(xx[1], xx[2]); swap(yy[1], yy[2]); }

            double best = xhi - xlo;
            for (int ii = 0; ii<3; ii++) {
                if (xx[ii+1] - xx[ii+0] < best && diffsign(yy[ii+0], yy[ii+1])) {
                    xlo = xx[ii+0]; ylo = yy[ii+0];
                    xhi = xx[ii+1]; yhi = yy[ii+1];
                    best = xx[ii+1] - xx[ii+0];
                }
            }
        }
        return xhi + .5*(xhi - xlo);
    }

    struct quadroots {
        double lo, hi;
    };

    // Find the real roots of: a*x^2 + b*x + c (if possible).  It returns NaNs
    // in bad cases.  XXX: Numerically stable, but there is room for
    // improvement in special cases and error handling....
    static inline quadroots quadsolve(double aa, double bb, double cc) {
        double rad = sqrt(bb*bb - 4*aa*cc);
        quadroots results;
        if (bb < 0) {
            double r0 = (2*cc)/(-bb + rad);
            double r1 = (-bb + rad)/(2*aa);
            results.lo = fmin(r0, r1);
            results.hi = fmax(r0, r1);
        } else {
            double r0 = (-bb - rad)/(2*aa);
            double r1 = (2*cc)/(-bb - rad);
            results.lo = fmin(r0, r1);
            results.hi = fmax(r0, r1);
        }
        return results;
    }

    // Given 3 consecutive values, find the place where the derivative is zero
    static inline double quadpeak(double before, double center, double after) {
        double two_a = (before - 2*center + after);
        double neg_b = .5*(before - after);
        return neg_b/two_a;
    }

    //}}}
    //{{{ Numerical Optimization:        (adaptmin), covarmin, (quasimin) 
    //}}}
    //{{{ Automatic Differentiation:     (autodiff) 
    //}}}
    //{{{ Random Number Generation:      prng, shiftreg, gpsgold 
    //{{{ prng 
    struct prng {
        inline ~prng();
        inline prng();
        inline prng(const prng& other);
        inline prng(uint64_t seed);
        inline prng& operator =(const prng& other);
        inline void reseed(uint64_t seed);

        inline uint64_t uint64();
        inline double uniform();
        inline double uniform(double lo, double hi);
        inline double normal();
        inline cdouble cxnormal();

        private:
            uint64_t state[16];
            int64 index;
    };

    prng::~prng() {}

    prng::prng() {
        reseed(0);
    }

    prng::prng(const prng& other) : index(other.index) {
        for (int64 ii = 0; ii<16; ii++) {
            state[ii] = other.state[ii];
        }
    }

    prng::prng(uint64_t seed) {
        reseed(seed);
    }

    prng& prng::operator =(const prng& other) {
        index = other.index;
        for (int64 ii = 0; ii<16; ii++) {
            state[ii] = other.state[ii];
        }
        return *this;
    }

    void prng::reseed(uint64_t seed) {
        while (seed == 0) {
            struct timeval tv;
            check(gettimeofday(&tv, 0) == 0, "gettimeofday");
            seed = (tv.tv_sec*1000000ULL) ^ tv.tv_usec;
        }
        // This is 'xorshift64*' by Sebastiano Vigna (public domain).
        // We're just using it to seed the other generator.
        for (int64 ii = 0; ii<16; ii++) {
            seed ^= seed >> 12;
            seed ^= seed << 25;
            seed ^= seed >> 27;
            state[ii] = seed * 2685821657736338717LL;
        }
        index = 0;
    }

    uint64_t prng::uint64() {
        // This is 'xorshift1024*' by Sebastiano Vigna (public domain).  It's
        // an improved version of one from George Marsaglia, and should have a
        // period of 2^1024 - 1, which is long enough for many things.  It does
        // well on the "BigCrush" suite of tests (better than Mersenne Twister).
        uint64_t state0 = state[index];
        index = (index + 1)%16;
        uint64_t state1 = state[index];
        state1 ^= state1 << 31;
        state1 ^= state1 >> 11;
        state0 ^= state0 >> 30;
        state[index] = state0 ^ state1;
        return state[index] * 1181783497276652981LL; 
    }

    double prng::uniform() {
        const double scale = pow(2, -63);
        int64_t value = (int64_t)uint64();
        return value*scale;
    }

    double prng::uniform(double lo, double hi) {
        const double scale = pow(2, -64);
        double value = scale*uint64();
        return lo + (hi - lo)*value;
    }

    double prng::normal() {
        cdouble zz = cxnormal();
        return zz.re + zz.im;
    }

    cdouble prng::cxnormal() {
        // u0 is in (0, 1], and u1 is in [0, 2pi)
        const double s0 = 5.42101086242752217e-20;
        const double u0 = s0*uint64() + s0;
        const double s1 = 3.40612158008655459e-19;
        const double u1 = s1*uint64();
        // Box-Muller transform
        const double len = ::sqrt(-::log(u0));
        return cdouble(::cos(u1)*len, ::sin(u1)*len);
    }
    //}}}
    //{{{ shiftreg
    struct shiftreg {
        inline shiftreg(
            uint64_t poly = (1L<<61) | (1L<<60) | (1L<<45) | (1L<<1) | 1,
            uint64_t seed = 0x196e6a4b093L, bool fibonacci=false
        );

        inline void shl();
        inline void shr();

        inline uint64_t val() const;
        inline bool bit() const;

        private:
            static uint64_t popcnt(uint64_t poly);
            static uint64_t gethi(uint64_t poly);

            bool fib;
            uint64_t poly, hibit, reg;
    };

    shiftreg::shiftreg(uint64_t poly, uint64_t seed, bool fibonacci) :
        fib(fibonacci), poly(poly), hibit(gethi(poly)), reg(seed&(hibit - 1))
    {}

    uint64_t shiftreg::popcnt(uint64_t bits) {
        return __builtin_popcountl(bits);
    }

    uint64_t shiftreg::gethi(uint64_t poly) {
        check(poly != 0, "can't have a zero polynomial");
        uint64_t hibit = 1;
        poly >>= 1;
        while (poly) {
            hibit <<= 1;
            poly >>= 1;
        }
        return hibit;
    }

    void shiftreg::shl() {
        reg <<= 1;
        if (fib) {
            if (popcnt(reg&poly)%2) {
                reg |= 1;
            }
            reg &= ~hibit;
        } else {
            if (reg&hibit) {
                reg ^= poly;
            }
        }
    }

    void shiftreg::shr() {
        if (fib) {
            if (popcnt(reg&poly)%2) {
                reg |= hibit;
            }
        } else {
            if (reg&1) {
                reg ^= poly;
            }
        }
        reg >>= 1;
    }

    uint64_t shiftreg::val() const {
        return reg;
    }

    bool shiftreg::bit() const {
        return (reg&1) ? true : false;
    }

    //}}}
    //{{{ gpsgold
    struct gpsgold {
        inline gpsgold(int32_t prn);

        inline void next();

        inline bool bit() const;
        inline int8_t chip() const;

        private:
            shiftreg g1;
            shiftreg g2;
    };

    gpsgold::gpsgold(int32_t prn) :
        // See ICD GPS 200, pages 29-30
        g1((1<<10) | (1<<3) | 1, 0x3FF, true),
        g2((1<<10) | (1<<9) | (1<<8) | (1<<6) | (1<<3) | (1<<2) | 1, 0x3ff, true)
    {
        check(1 <= prn && prn <= 37, "prn must be in range");
        // see ICD GPS 200, pages 8-9
        const static int32_t table[38] = {
            // 0    1    2    3    4    5    6    7    8    9
               0,   5,   6,   7,   8,  17,  18, 139, 140, 141,
             251, 252, 254, 255, 256, 257, 258, 469, 470, 471,
             472, 473, 474, 509, 512, 513, 514, 515, 516, 859,
             860, 861, 862, 863, 950, 947, 948, 950
        };
        const int32_t delay = table[prn];
        for (int32_t ii = 0; ii<delay; ii++) {
            g2.shr();
        }
    }

    void gpsgold::next() {
        g1.shl();
        g2.shl();
    }

    bool gpsgold::bit() const {
        return ((g1.val() ^ g2.val()) & 0x200) >> 9;
    }

    int8_t gpsgold::chip() const {
        return bit() ? +1 : -1;
    }

    //}}}
    //}}}
    //{{{ Statistics Functions:          ricepdf, ricecdf, mednoise 
    namespace internal {
        // XXX: verify this
        static inline double chebyshev(double xx, const double* cc, int nn) {
            check(nn > 0, "sanity");
            double b0 = cc[0];
            double b1 = 0;
            double b2 = 0;
            for (int ii = 1; ii < nn; ++ii) {
                b2 = b1;
                b1 = b0;
                b0 = xx*b1 - b2 + cc[ii];
            }
            
            return 0.5*(b0 - b2);
        }

        // XXX: verify this
        static inline double besseli0(double xx) {
            // Coefficients for fabs(xx) < 8.0
            static const double aa[30] = {
                -4.41534164647933937950e-18, +3.33079451882223809783e-17,
                -2.43127984654795469359e-16, +1.71539128555513303061e-15,
                -1.16853328779934516808e-14, +7.67618549860493561688e-14,
                -4.85644678311192946090e-13, +2.95505266312963983461e-12,
                -1.72682629144155570723e-11, +9.67580903537323691224e-11,
                -5.18979560163526290666e-10, +2.65982372468238665035e-9,
                -1.30002500998624804212e-8,  +6.04699502254191894932e-8,
                -2.67079385394061173391e-7,  +1.11738753912010371815e-6,
                -4.41673835845875056359e-6,  +1.64484480707288970893e-5,
                -5.75419501008210370398e-5,  +1.88502885095841655729e-4,
                -5.76375574538582365885e-4,  +1.63947561694133579842e-3,
                -4.32430999505057594430e-3,  +1.05464603945949983183e-2,
                -2.37374148058994688156e-2,  +4.93052842396707084878e-2,
                -9.49010970480476444210e-2,  +1.71620901522208775349e-1,
                -3.04682672343198398683e-1,  +6.76795274409476084995e-1
            };

            // Coefficients for fabs(xx) >= 8.0
            static const double bb[25] = {
                -7.23318048787475395456e-18, -4.83050448594418207126e-18,
                +4.46562142029675999901e-17, +3.46122286769746109310e-17,
                -2.82762398051658348494e-16, -3.42548561967721913462e-16,
                +1.77256013305652638360e-15, +3.81168066935262242075e-15,
                -9.55484669882830764870e-15, -4.15056934728722208663e-14,
                +1.54008621752140982691e-14, +3.85277838274214270114e-13,
                +7.18012445138366623367e-13, -1.79417853150680611778e-12,
                -1.32158118404477131188e-11, -3.14991652796324136454e-11,
                +1.18891471078464383424e-11, +4.94060238822496958910e-10,
                +3.39623202570838634515e-9, +2.26666899049817806459e-8,
                +2.04891858946906374183e-7, +2.89137052083475648297e-6,
                +6.88975834691682398426e-5, +3.36911647825569408990e-3,
                +8.04490411014108831608e-1
            };

            if (xx < 0) xx = -xx;

            return (xx < 8 ?
                chebyshev(.5*xx - 2, aa, 30)*exp(xx) :
                chebyshev(32/xx - 2, bb, 25)*exp(xx)/sqrt(xx)
            );
        }
    }

    static inline double ricepdf(double vv, double ss, double xx) {
        using namespace internal;
        check(vv >= 0, "sanity");
        check(ss > 0, "sanity");
        if (xx < 0) return 0;
        return (
            xx*exp(-(xx*xx + vv*vv) / (2 * ss*ss)) *
            besseli0(xx*vv/(ss*ss)) / (ss*ss)
        );
    }

    static inline double ricecdf(double vv, double ss, double xx) {
        check(vv >= 0, "sanity");
        check(ss > 0, "sanity");
        if (xx < 0) return 0;
        // This is magic to evaluate the Marcum Q function
        double aa = vv/ss;
        double bb = xx/ss;
        // aa and bb below should be real and non-negative
        // Should we check for this?
        aa = aa*aa/2;
        bb = bb*bb/2;
        double dd = exp(-aa);
        double hh = dd;
        double ff = bb*exp(-bb);
        double delta = ff*hh;
        double sum = delta;
        double kk = 1;
        const double eps = 1e-60;
        while (delta > eps*sum) {
            dd *= aa/kk;
            hh += dd;
            ff *= bb/(kk + 1);
            delta = ff*hh;
            sum += delta;
            kk = kk + 1;
        }
        return sum;
    }

    static inline double mednoise(const cfloat* ptr, ssize_t len) {
        // XXX: consider doing this with quickselect instead of a histogram

        // Floating point values can have a max exponent of about 38, so ignoring
        // denormals and infinities, the magsquare exponent can go from about -76
        // to +76.  That's -760 to +760 dB.  We're doing a histogram of tenths of a
        // dB, so -8000 to +8000 should cover the range more than adequately.
        vector<ssize_t> histogram(16000, 0);
        ssize_t ii;

        for (ii = 0; ii<len; ii++) {
            double m2 = mag2(ptr[ii]);
            double db = 10.0*::log10(m2 + 1e-300);
            double bin = floor(8000.0 + 10.0*db);
            if (bin < 0) bin = 0;
            if (bin > 15999) bin = 15999;
            histogram[(ssize_t)bin]++;
        }

        ssize_t count = 0;
        for (ii = 0; ii<16000; ii++) {
            count += histogram[ii];
            if (count >= len/2) {
                double db = (ii + .5 - 8000)/10.0;
                double median = ::pow(10.0, db/10);

                // Returns linear power after converting the median to mean.
                // Assumes the original data was complex normal, and so the
                // the mag squared data is Chi-square with 2 degrees of freedom.
                return median/::log(2.0);
            }
        }

        check(false, "shouldn't get here");
        return -1;
    }
    //}}}
    //{{{ Rotational Shifting:           (fftshift), (ifftshift), hshift, vshift, shift2d 
    namespace internal {
        static inline int64 shiftgcd(int64 rot, int64 len) {
            int64 gcd = len;
            int64 tmp = rot;
            while (tmp) {
                int64 mod = gcd%tmp;
                gcd = tmp;
                tmp = mod;
            }
            return gcd;
        }
    }

    template<class type>
    static inline void hshift(type* data, int64 rows, int64 cols, int64 horz) {
        using namespace internal;
        horz = horz%cols;
        if (horz == 0) return;
        if (horz < 0) horz += cols;
        int64 gcd = shiftgcd(horz, cols);
        int64 cyc = cols/gcd - 1;

        for (int64 ii = 0; ii<rows; ii++) {
            type* ptr = data + ii*cols;
            for (int64 jj = 0; jj<gcd; jj++) {
                long lo = jj;
                long hi = jj + horz;
                for (int64 kk = 0; kk<cyc; kk++) {
                    swap(ptr[lo], ptr[hi]);
                    lo = hi; hi += horz;
                    if (hi >= cols) hi -= cols;
                }
            }
        }
    }

    template<class type>
    static inline void vshift(type* data, int64 rows, int64 cols, int64 vert) {
        using namespace internal;
        vert = vert%rows;
        if (vert == 0) return;
        if (vert < 0) vert += rows;
        int64 gcd = shiftgcd(vert, rows);
        int64 cyc = rows/gcd - 1;

        for (int64 ii = 0; ii<gcd; ii++) {
            long lo = ii;
            long hi = ii + vert;
            for (int64 kk = 0; kk<cyc; kk++) {
                for (int64 jj = 0; jj<cols; jj++) {
                    swap(data[lo*cols + jj], data[hi*cols + jj]);
                }
                lo = hi; hi += vert;
                if (hi >= rows) hi -= rows;
            }
        }
    }

    // Circularly shifts such that the offset at (ii, jj) will be at (0, 0)
    template<class type>
    static inline void shift2d(type* data, int64 rows, int64 cols, int64 ii, int64 jj) {
        vshift(data, rows, cols, ii);
        hshift(data, rows, cols, jj);
    }
    //}}}
    //{{{ Window Functions:              sinc, firwin, kaiswin, chebwin
    //{{{ sinc
    static inline double sinc(double x) {
        if (x == 0.0) return 1;
        return sin(M_PI*x)/(M_PI*x);
    }
    //}}}
    //{{{ firwin
    static inline double firwin(int window, double offset, double length) {
        if (offset > +.5*length) return 0;
        if (offset < -.5*length) return 0;

        //
        // These windows are based on two papers.  The first is from Albert H.
        // Nuttall, called "Some Windows with Very Good Sidelobe Behaviour".
        // In it, he explains that windows that sum to zero at the edge have
        // -18dB of roll-off per octave, which is convenient because we want
        // that property for other reasons too.  Nuttall optimized for
        // sidelobes on the window, but here we optimize for the sidelobes of a
        // half-band filter built from the window.
        //
        // With the exception of the rectangular window, all of these windows
        // equal 1.0 in the middle and 0.0 at the edge and have continuous
        // first derivatives.  We don't include Hamming or Blackman-Harris
        // windows in here because both of those are not zero at the end
        // points.
        //
        // The second paper is by Hans H. Albrecht, called "A family of cosine-
        // sum windows for high-resolution measurements".  We used some of his
        // coefficients as starting points and then ran them through a
        // numerical optimizer to meet Nuttall's criteria for -18dB roll-off.
        //
        double xx = 2*M_PI*offset/length;
        switch (window) {
            case 0: // rectangular
                return 1.0;
            case 1: // Hann window (-43.94dB, -18dB per octave)
                return 0.5 + 0.5*cos(xx);
            case 2: // Nuttall 3-term (-80.41dB, -18dB per octave)
                return 0.41 + 0.5*cos(xx) + 0.09*cos(2*xx);
            case 3: // Nuttall 4-term (-114.38dB, -18dB per octave)
                return (
                    0.353478834 + 0.486608654*cos(xx) +
                    0.146521166*cos(2*xx) + 0.013391346*cos(3*xx)
                );
            case 4: // Nuttall, Albrecht (-134.96dB, -18dB per octave)
                return (
                    0.323497331 + 0.471838046*cos(xx) +
                    0.175295953*cos(2*xx) + 0.028161954*cos(3*xx) +
                    0.001206716*cos(4*xx)
                );
            case 5: // Nuttall, Albrecht (-163.69dB, -18dB per octave)
                return (
                    0.293169361571 + 0.451670371292*cos(xx) +
                    0.201735249850*cos(2*xx) + 0.048184602083*cos(3*xx) +
                    0.005095388579*cos(4*xx) + 0.000145026625*cos(5*xx)
                );
            default:
                check(false, "unhandled window %d", window);
        }

        return 0;
    }
    // This is intended to approximate the effective decrease in
    // bandwidth when using a window before performing an FFT.
    static inline double apodize(int window) {
        switch (window) {
            case 0:  return 1.0;
            case 1:  return 2.0409;
            case 2:  return 2.4914;
            case 3:  return 2.8927;
            case 4:  return 3.1616;
            case 5:  return 3.4894;
        }
        check(false, "invalid window");
        return 0;
    }

    //}}}
    //{{{ kaiswin
    // Kaiser-Bessel window, centered at zero. 
    // These windows have discontinuities at the end points.
    static inline double kaiswin(double alpha, double offset, double length) {
        if (offset > +.5*length) return 0;
        if (offset < -.5*length) return 0;
        offset += .5*length;

        return (
            internal::besseli0(
                M_PI*alpha*::sqrt(1 - sqr(2*offset/length - 1))
            ) / internal::besseli0(M_PI*alpha)
        );
    }
    //}}}
    //{{{ chebwin
    namespace internal {

        //
        // Time domain Dolph-Chebyshev window.  It has a very flat stop band.
        //
        // The equation is from the link here, but we did not use their code:
        //
        //       http://practicalcryptography.com/miscellaneous/ ...
        //       ... machine-learning/implementing-dolph-chebyshev-window/
        //
        // Refers to: Antoniou, A., "Digital Filters", McGraw-Hill, 2000
        //
        static double chebwin(double atten, double offset, double length) {
            if (offset > +.5*length) return 0;
            if (offset < -.5*length) return 0;

            double invr = ::pow(10, -atten/20);
            double xnot = ::cosh(::acosh(invr)/length);
            double sum = 0;
            for (int64 ii = 1; ii<length/2; ii++) {
                double arg = xnot*::cos(ii*M_PI/(length + 1));
                double cheb = (
                    ::fabs(arg) <= 1.0 ? 
                    ::cos(length*acos(arg)) :
                    ::cosh(length*acosh(arg))
                );
                sum += ::cos(2*offset*ii*M_PI/(length + 1))*cheb;
            }
            return (invr + 2*sum)/(length + 1);
        }
    }

    // The atten parameter is negative dB, for instance atten=-60.0
    // See notes above in the internal implementatation.
    static inline double chebwin(double atten, double offset, double length) {
        const double scale = internal::chebwin(atten, 0, length);
        return internal::chebwin(atten, offset, length)/scale;
    }
    //}}}
    //}}}
    //{{{ IIR and FIR Filters:           (firparks), (halfpass) 
    //}}}
    //{{{ Signal Processing:             blocktuner, singleton, polyphase 
    //{{{ blocktuner

    // blocktuner uses a 64 bit integer to track phase.  This has more
    // precision than a 64 bit double, and it does not lose accuracy as the
    // sequence of data gets large.  A block of 1024 precomputed phasors is
    // stored so there are no trig calls in the inner loop of the apply method.
    struct blocktuner {
        //~blocktuner() = default;
        //blocktuner() = default;
        //blocktuner(const blocktuner&) = default;
        //blocktuner& operator =(const blocktuner&) = default;

        // arguments are digital frequency (cycles per sample),
        // and phase at the first sample (cycles)
        inline blocktuner(double dfreq, double phase0=0.0);

        // off - indicates the absolute offset where ptr is in
        // the tuned data stream.  len is the number of samples
        inline void apply(cfloat* ptr, int64 off, int64 len);

        private:
            cfloat block[1024];
            uint64_t delta;
    };


    blocktuner::blocktuner(double dfreq, double phase0) {
        dfreq = fmodl(dfreq, 1.0);
        if (dfreq < 0) dfreq += 1.0;
        delta = (uint64_t)roundl(dfreq*powl(2.0, +64));
        for (uint64_t ii = 0; ii<1024; ii++) {
            uint64_t phase = delta * ii;
            double angle = 2*M_PI*(phase0 + phase*pow(2.0, -64));
            block[ii] = cfloat(cos(angle), sin(angle));
        }
    }

    void blocktuner::apply(cfloat* ptr, int64 off, int64 len) {
        // this assumes 2's complement, which should always be true
        uint64_t phase = delta*(uint64_t)off;
        while (len > 0) {
            double angle = 2*M_PI*phase*pow(2.0, -64);
            cfloat extra = cfloat(cos(angle), sin(angle));

            uint64_t amt = len;
            if (amt > 1024) amt = 1024;
            for (uint64_t ii = 0; ii<amt; ii++) {
                ptr[ii] = ptr[ii] * block[ii] * extra;
            }

            len -= amt;
            ptr += amt;
            phase += amt*delta;
        }
    }

    //}}}
    //{{{ singleton

    template<class atype, class btype>
    static inline void singleton(
        complex<atype>* dst, const complex<btype>* src, int64 len,
        double cycles_lo, double cycles_hi
    ) {
        // We're going through a lot of pains here to avoid making
        // expensive sine and cosine calls in the inner loop.  The
        // naive trig recurrence is numerically unstable and incurs
        // O(N) rounding error as it runs, so we don't do that.
        // Instead, we're using a recurrence from Richard Singleton
        // that incurs O(sqrt N) error.  For large lengths, that
        // could still get too large, so we restart the recurrence
        // after every 1024 samples as necessary.  We also subtract
        // out integer cycles before converting to radians so that
        // we aren't evaluating cosines and sines of large numbers.
        double invlen = 1.0/len;
        for (int64 jj = 0; jj<len; jj += 1024) {
            int64 index_lo = jj;
            int64 index_hi = min(jj + 1024, len);
            double fract_lo = index_lo*invlen;
            double fract_hi = index_hi*invlen;
            double cyc0 = (1.0 - fract_lo)*cycles_lo + fract_lo*cycles_hi;
            double cyc1 = (1.0 - fract_hi)*cycles_lo + fract_hi*cycles_hi;

            double mid = round(.5*cyc0 + .5*cyc1);
            double rad0 = 2*M_PI*(cyc0 - mid);
            double rad1 = 2*M_PI*(cyc1 - mid);
            double dph = (rad1 - rad0)/(index_hi - index_lo);
            double cc = cos(rad0);
            double ss = sin(rad0);
            double aa = 2*sin(dph/2)*sin(dph/2);
            double bb = sin(dph);

            for (int64 ii = index_lo; ii<index_hi; ii++) {
                double re = src[ii].re;
                double im = src[ii].im;
                dst[ii].re = re*cc - im*ss;
                dst[ii].im = im*cc + re*ss;
                // Singleton's trig recurrence
                double tc = cc - (aa*cc + bb*ss);
                double ts = ss + (bb*cc - aa*ss);
                cc = tc;
                ss = ts;
            }
        }
    }
    //XXX: This is the old one - delete it...
#if 0
    static inline void singleton_tune(cfloat* dst, const cfloat* src, int64 len, double ph0, double ph1) {
        // We're going through a lot of pains here to avoid making
        // expensive sine and cosine calls in the inner loop.  The
        // naive trig recurrence is numerically unstable and incurs
        // O(N) rounding error as it runs, so we don't do that.
        // Instead, we're using a recurrence from Richard Singleton
        // that incurs O(sqrt N) error.  For large lengths, that
        // could still get too large, so we restart the recurrence
        // after every 1024 samples as necessary.  We also subtract
        // out integer cycles before converting to radians so that
        // we aren't evaluating cosines and sines of large numbers.
        double invlen = 1.0/len;
        for (int64 jj = 0; jj<len; jj += 1024) {
            int64 index_lo = jj;
            int64 index_hi = min(jj + 1024, len);
            double fract_lo = index_lo*invlen;
            double fract_hi = index_hi*invlen;
            double cyc0 = (1.0 - fract_lo)*ph0 + fract_lo*ph1;
            double cyc1 = (1.0 - fract_hi)*ph0 + fract_hi*ph1;

            double mid = round(.5*cyc0 + .5*cyc1);
            double rad0 = 2*M_PI*(cyc0 - mid);
            double rad1 = 2*M_PI*(cyc1 - mid);
            double dph = (rad1 - rad0)/(index_hi - index_lo);
            double cc = cos(rad0);
            double ss = sin(rad0);
            double aa = 2*sin(dph/2)*sin(dph/2);
            double bb = sin(dph);

            for (int64 ii = index_lo; ii<index_hi; ii++) {
                dst[ii] = cfloat(cc, ss)*src[ii];
                /*
                double re = src[ii].re;
                double im = src[ii].im;
                dst[ii].re = re*cc - im*ss;
                dst[ii].im = im*cc + re*ss;
                */
                // Singleton's trig recurrence
                double tc = cc - (aa*cc + bb*ss);
                double ts = ss + (bb*cc - aa*ss);
                cc = tc;
                ss = ts;
            }
        }
    }
#endif
    //}}}
    //{{{ polyphase
    struct polyphase {

        //~polyphase() = default;
        //polyphase() = default;
        //polyphase(const polyphase&) = default;
        //polyphase& operator =(const polyphase&) = default;

        inline polyphase(int window, double dwidth, int64 taps);

        inline void resample(
            cfloat* dst_ptr, int64 dst_len,
            const cfloat* src_ptr, double src_lo, double src_hi
        ) const;

        private:
            enum { COUNT = 1024 };
            int64 taps;
            vector<float> bank;

            inline void makefir(float* ptr, double fract, int window, double dwidth);
            inline cfloat interp(const cfloat* src, double where) const;
    };

    polyphase::polyphase(int window, double dwidth, int64 taps) : taps(taps), bank(taps*COUNT) {
        for (int64 ii = 0; ii<COUNT; ii++) {
            double fract = ii/(double)COUNT;
            makefir(bank.data() + ii*taps, fract, window, dwidth);
        }
    }

    void polyphase::makefir(float* ptr, double fract, int window, double dwidth) {
        double sum = 0.0;
        for (int64 jj = 0; jj<taps; jj++) {
            double xx = jj + 1 - taps/2 - fract;
            double tap = firwin(window, xx, taps)*sinc(xx*dwidth);
            ptr[jj] = tap;
            sum += tap;
        }
        double inv = 1.0/sum;
        for (int64 jj = 0; jj<taps; jj++) {
            ptr[jj] *= inv;
        }
    }

    cfloat polyphase::interp(const cfloat* src, double where) const {
        double re = 0;
        double im = 0;
        int64 fixed = llrint(where*COUNT);
        int64 index = fixed/COUNT;
        int64 which = fixed%COUNT;
        const cfloat* ptr = src + index + 1 - taps/2;
        const float* filt = bank.data() + taps*which;
        for (int64 ii = 0; ii<taps; ii++) {
            re += ptr[ii].re*filt[ii];
            im += ptr[ii].im*filt[ii];
        }
        return cfloat(re, im);
    }

    void polyphase::resample(
        cfloat* dst_ptr, int64 dst_len,
        const cfloat* src_ptr, double src_lo, double src_hi
    ) const {
        double df = 1.0/dst_len;
        for (int64 ii = 0; ii<dst_len; ii++) {
            double ff = ii*df;
            dst_ptr[ii] = interp(src_ptr, (1.0 - ff)*src_lo + ff*src_hi);
        }
    }

    //}}}
    //}}}
    //{{{ Coordinate Systems:            cartesian, transform, geodetic 
    //{{{ cartesian
    struct cartesian {
        double x, y, z;
    };

    static inline cartesian operator +(cartesian aa, cartesian bb) {
        return (cartesian){ aa.x + bb.x, aa.y + bb.y, aa.z + bb.z };
    }

    static inline cartesian operator -(cartesian aa, cartesian bb) {
        return (cartesian){ aa.x - bb.x, aa.y - bb.y, aa.z - bb.z };
    }

    static inline cartesian operator *(double aa, cartesian bb) {
        return (cartesian){ aa*bb.x, aa*bb.y, aa*bb.z };
    }

    static inline cartesian operator *(cartesian aa, double bb) {
        return (cartesian){ aa.x*bb, aa.y*bb, aa.z*bb };
    }

    static inline cartesian operator /(cartesian aa, double bb) {
        return aa * (1.0/bb);
    }

    static inline cartesian operator +=(cartesian& aa, cartesian bb) {
        return aa = aa + bb;
    }

    static inline cartesian operator -=(cartesian& aa, cartesian bb) {
        return aa = aa - bb;
    }

    static inline cartesian operator *=(cartesian& aa, double bb) {
        return aa = aa * bb;
    }

    static inline cartesian operator /=(cartesian& aa, double bb) {
        return aa = aa / bb;
    }

    static inline cartesian operator -(cartesian aa) {
        return (cartesian){ -aa.x, -aa.y, -aa.z };
    }

    static inline double len(cartesian aa) {
        return hypot(aa.x, hypot(aa.y, aa.z));
    }

    static inline double fast_len(cartesian aa) {
        // This one is less numerically stable, and it will have problems
        // if the squares underflow to zero or overflow to infinity, but
        // it is a little faster than the safe len(...) function above.
        return sqrt(aa.x*aa.x + aa.y*aa.y + aa.z*aa.z);
    }

    static inline double dist(cartesian aa, cartesian bb) {
        return len(aa - bb);
    }

    static inline cartesian unit(cartesian aa) {
        return (1.0/len(aa))*aa;
    }

    static inline double dot(cartesian aa, cartesian bb) {
        return aa.x*bb.x + aa.y*bb.y + aa.z*bb.z;
    }

    static inline cartesian cross(cartesian aa, cartesian bb) {
        return (cartesian){
            aa.y*bb.z - aa.z*bb.y,
            aa.z*bb.x - aa.x*bb.z,
            aa.x*bb.y - aa.y*bb.x
        };
    }

    /*
    static inline std::ostream& operator <<(std::ostream& os, const cartesian& xyz) {
        os << "cartesian(x: " << xyz.x << ", y: " << xyz.y << ", z: " << xyz.z << ")";
        return os;
    }
    */

    //}}}
    //{{{ transform
    struct transform {
        // Transform Matrix:
        //
        //   u.x   u.y   u.z   t.x
        //   v.x   v.y   v.z   t.y
        //   w.x   w.y   w.z   t.z
        //    0     0     0     1
        //
        cartesian u, v, w, t;

        cartesian operator ()(cartesian aa) const {
            return t + (cartesian){
                dot(u, aa), dot(v, aa), dot(w, aa)
            };
        }
    };

    static inline transform translate(cartesian tt) {
        return (transform){
            (cartesian){ 1, 0, 0 },
            (cartesian){ 0, 1, 0 },
            (cartesian){ 0, 0, 1 },
            tt
        };
    }

    static inline transform operator *(transform aa, transform bb) {
        return (transform){
            (cartesian){
                aa.u.x*bb.u.x + aa.u.y*bb.v.x + aa.u.z*bb.w.x,
                aa.u.x*bb.u.y + aa.u.y*bb.v.y + aa.u.z*bb.w.y,
                aa.u.x*bb.u.z + aa.u.y*bb.v.z + aa.u.z*bb.w.z
            },
            (cartesian){
                aa.v.x*bb.u.x + aa.v.y*bb.v.x + aa.v.z*bb.w.x,
                aa.v.x*bb.u.y + aa.v.y*bb.v.y + aa.v.z*bb.w.y,
                aa.v.x*bb.u.z + aa.v.y*bb.v.z + aa.v.z*bb.w.z
            },
            (cartesian){
                aa.w.x*bb.u.x + aa.w.y*bb.v.x + aa.w.z*bb.w.x,
                aa.w.x*bb.u.y + aa.w.y*bb.v.y + aa.w.z*bb.w.y,
                aa.w.x*bb.u.z + aa.w.y*bb.v.z + aa.w.z*bb.w.z
            },
            (cartesian){
                aa.u.x*bb.t.x + aa.u.y*bb.t.y + aa.u.z*bb.t.z + aa.t.x,
                aa.v.x*bb.t.x + aa.v.y*bb.t.y + aa.v.z*bb.t.z + aa.t.y,
                aa.w.x*bb.t.x + aa.w.y*bb.t.y + aa.w.z*bb.t.z + aa.t.z
            }
        };
    }


    //}}}
    //{{{ geodetic
    struct geodetic {
        double lat, lon, alt;
    };

    namespace internal {
        static inline double curve_pvert(double lat) {
            const double EARTH_SMAJOR  = 6378137.0;     // meters, semi-major axis
            const double EARTH_ECC_SQ  = .006694379990141316996137;
            return EARTH_SMAJOR/sqrt(1.0 - EARTH_ECC_SQ*pow(sin(lat),2.0));
        }
    }

    static inline cartesian lla_to_ecf(geodetic lla) {
        using namespace internal;
        const double RAD_PER_DEG   = .017453292519943295769237;
        const double EARTH_ECC_SQ  = .006694379990141316996137;
        lla.lat *= RAD_PER_DEG; lla.lon *= RAD_PER_DEG;

        double clat  = cos(lla.lat), slat = sin(lla.lat);
        double clon  = cos(lla.lon), slon = sin(lla.lon);

        double N     = curve_pvert(lla.lat);
        double G     = (1.0-EARTH_ECC_SQ)*N;

        return (cartesian){
            (N+lla.alt)*clat*clon,
            (N+lla.alt)*clat*slon,
            (G+lla.alt)*slat
        };
    }

    static inline cartesian lla_to_ecf_vel(geodetic pos, geodetic vel) {
        using namespace internal;
        const double RAD_PER_DEG   = .017453292519943295769237;
        const double EARTH_ECC_SQ  = .006694379990141316996137;
        pos.lat *= RAD_PER_DEG; pos.lon *= RAD_PER_DEG;
        vel.lat *= RAD_PER_DEG; vel.lon *= RAD_PER_DEG;

        double clat = cos(pos.lat); double slat = sin(pos.lat);
        double clon = cos(pos.lon); double slon = sin(pos.lon);
        double N    = curve_pvert(pos.lat);
        double Ndot = (N*EARTH_ECC_SQ*slat*clat / (1 - EARTH_ECC_SQ*slat*slat))*vel.lat;

        double xx = -(N + pos.alt)*clat*slon*vel.lon -
            (N + pos.alt)*slat*vel.lat*clon + (Ndot + vel.alt)*clat*clon;

        double yy = (N + pos.alt)*clat*clon*vel.lon -
            (N + pos.alt)*slat*vel.lat*slon + (Ndot + vel.alt)*clat*slon;

        double zz = (N*(1 - EARTH_ECC_SQ) + pos.alt)*clat*vel.lat +
            (Ndot*(1 - EARTH_ECC_SQ) + vel.alt)*slat;

        return (cartesian){xx, yy, zz};
    }

    static inline cartesian lla_to_ecf_acc(geodetic pos, geodetic vel, geodetic acc) {
        using namespace internal;
        const double RAD_PER_DEG   = .017453292519943295769237;
        const double EARTH_ECC_SQ  = .006694379990141316996137;
        pos.lat *= RAD_PER_DEG; pos.lon *= RAD_PER_DEG;
        vel.lat *= RAD_PER_DEG; vel.lon *= RAD_PER_DEG;

        double clat  = cos(pos.lat); double slat = sin(pos.lat);
        double clon  = cos(pos.lon); double slon = sin(pos.lon);
        double denom = 1/(1 - EARTH_ECC_SQ*slat*slat);
        double N     = curve_pvert(pos.lat);
        double Nh = N + pos.alt;
        double Ndot  = (N*EARTH_ECC_SQ*slat*clat*denom)*vel.lat;
        double Nddot = (N*EARTH_ECC_SQ*denom) *
            (
                3*EARTH_ECC_SQ*slat*slat*clat*clat*vel.lat*vel.lat*denom -
                slat*slat*vel.lat*vel.lat + slat*clat*acc.lat + clat*clat*vel.lat*vel.lat
            );

        double xx =
            -Nh*clat*slon*acc.lon - Nh*clat*clon*vel.lon*vel.lon
            +Nh*slat*vel.lat*slon*vel.lon - (Ndot+vel.alt)*clat*slon*vel.lon
            +Nh*slat*vel.lat*slon*vel.lon - Nh*slat*acc.lat*clon
            -Nh*clat*vel.lat*vel.lat*clon - (Ndot+vel.alt)*slat*vel.lat*clon
            -(Ndot+vel.alt)*clat*slon*vel.lon - (Ndot+vel.alt)*slat*vel.lat*clon
            +(Nddot+acc.alt)*clat*clon;

        double yy =
            +Nh*clat*clon*acc.lon - Nh*clat*slon*vel.lon*vel.lon
            -Nh*slat*vel.lat*clon*vel.lon + (Ndot+vel.alt)*clat*clon*vel.lon
            -Nh*slat*vel.lat*clon*vel.lon - Nh*slat*acc.lat*slon
            -Nh*clat*vel.lat*vel.lat*slon - (Ndot+vel.alt)*slat*vel.lat*slon
            +(Ndot+vel.alt)*clat*clon*vel.lon - (Ndot+vel.alt)*slat*vel.lat*slon
            +(Nddot+acc.alt)*clat*slon;

        double zz =
            +(N*(1-EARTH_ECC_SQ)+pos.alt)*clat*acc.lat
            -(N*(1-EARTH_ECC_SQ)+pos.alt)*slat*vel.lat*vel.lat
            +2*(Ndot*(1-EARTH_ECC_SQ)+vel.alt)*clat*vel.lat
            +(Nddot*(1-EARTH_ECC_SQ)+acc.alt)*slat;

        return (cartesian){xx, yy, zz};
    }

    static inline geodetic ecf_to_lla(cartesian xyz) {
        using namespace internal;
        const double EARTH_SMINOR  = 6356752.314245179497564;  // meters, semi-minor axis
        const double EARTH_ECC_SQ  = .006694379990141316996137;
        double x = xyz.x, y = xyz.y, z = xyz.z;

        double p   = hypot(x,y); // Equatorial projection
        double lon = atan2(y,x); // Right ascension
        double lat = atan2(z,p); // Use geocentric latitude as initial guess
        double alt;

        int cnt = 0;
        double lat_old = lat - 10.0;
        double Rn;
        while ((fabs(lat - lat_old) > 1e-12) && (cnt++ < 10)) {
            lat_old = lat;

            Rn  = curve_pvert(lat);
            //alt = p/cosl(lat) - Rn;
            lat = atan2(z+Rn*EARTH_ECC_SQ*sin(lat), p);
        }

        // Near the poles geocentric and geodetic become the same
        if (fabs((fabs(lat) - M_PI/2.0)) < 1e-12) {
            alt = fabs(z) - EARTH_SMINOR;
        } else {
            alt = p/cos(lat) - curve_pvert(lat);
        }

        return (geodetic){ lat*180.0/M_PI, lon*180.0/M_PI, alt };
    }

    static inline transform ecf_to_enu(geodetic lla) {
        cartesian enu_center = lla_to_ecf(lla);

        double lon = M_PI/2.0 + M_PI*lla.lon/180.0; // rotate around z-axis first
        double lat = M_PI/2.0 - M_PI*lla.lat/180.0; // rotate around x-axis second

        transform lonrot = {
            {  cos(lon),  sin(lon),       0.0 },
            { -sin(lon),  cos(lon),       0.0 },
            {       0.0,       0.0,       1.0 },
            {       0.0,       0.0,       0.0 }
        };

        transform latrot = {
            {       1.0,        0.0,      0.0 },
            {       0.0,   cos(lat), sin(lat) },
            {       0.0,  -sin(lat), cos(lat) },
            {       0.0,        0.0,      0.0 }
        };

        return latrot * lonrot * translate(-enu_center);
    }

    static inline transform enu_to_ecf(geodetic lla) {
        cartesian enu_center = lla_to_ecf(lla);

        double lon = M_PI/2.0 + M_PI*lla.lon/180.0; // rotate around z-axis first
        double lat = M_PI/2.0 - M_PI*lla.lat/180.0; // rotate around x-axis second

        transform lonrot = {
            {  cos(lon),  -sin(lon),      0.0 },
            {  sin(lon),   cos(lon),      0.0 },
            {       0.0,        0.0,      1.0 },
            {       0.0,        0.0,      0.0 }
        };

        transform latrot = {
            {       1.0,        0.0,       0.0  },
            {       0.0,    cos(lat), -sin(lat) },
            {       0.0,    sin(lat),  cos(lat) },
            {       0.0,        0.0,       0.0  }
        };

        return translate(enu_center) * lonrot * latrot;
    }

    /*
    static inline std::ostream& operator <<(std::ostream& os, const geodetic& lla) {
        os << "geodetic(lat: " << lla.lat << ", lon: " << lla.lon << ", alt: " << lla.alt << ")";
        return os;
    }
    */


    //}}}
    //}}}
    //{{{ Precision Time and Date:       timecode, datetime 

    struct timecode {
        // J1950 Epoch
        int64 whole;
        double fract;

        static inline timecode parse(const string& ss);
    };

    // returns "seconds since midnight" before the timecode
    static inline double ssm(timecode tc) {
        return fmod(tc.whole%86400 + tc.fract, 86400.0);
    }

    struct datetime {
        int32_t year, month, day;
        int32_t hour, min;
        double secs;
    };

    static inline timecode normalize(timecode tc) {
        double lower = floor(tc.fract);
        tc.whole += (int64)lower;
        tc.fract -= lower;
        return tc;
    }

    static inline timecode gettime(datetime dt) {
        check(1950 <= dt.year  && dt.year  <  2100, "year in bounds: %d", dt.year);
        check(1    <= dt.month && dt.month <=   12, "month in bounds: %d", dt.month);
        check(0.0  <= dt.secs  && dt.secs  <= 60.0, "seconds in bounds: %lf", dt.secs);
        // XXX: could check day, hour, min as well

        static const int32_t moffset[] = {
            0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334
        };

        int64 whole = (dt.year - 1950) * 365;  // add days in first
        whole += (dt.year - 1949)/4;     // leap days since epoch
        whole += moffset[dt.month - 1];  // days in current year

        if ((dt.year % 4 == 0) && (dt.month > 2)) {
            whole++;                  // possible current leap day
        }

        whole += dt.day-1;  whole *= 24; // day of the month, now at curent day
        whole += dt.hour;   whole *= 60; // hours
        whole += dt.min;    whole *= 60; // minutes
        return normalize((timecode){ whole, dt.secs});
    }

    static inline datetime getdate(timecode tc, int32_t places = -1) {
        tc = normalize(tc);
        if (places >= 0) {
            double scale = pow(10, places);
            tc.fract = round(tc.fract*scale)/scale;
            if (tc.fract == 1.0) {
                tc.whole += 1;
                tc.fract = 0.0;
            }
        }

        static const int32_t mdays[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

        int32_t sec  = tc.whole % 60; tc.whole /= 60;
        int32_t min  = tc.whole % 60; tc.whole /= 60;
        int32_t hour = tc.whole % 24; tc.whole /= 24;

        // tc.whole is now days since 1950, switch to 1948 to make math easier
        tc.whole += 365+366;
        int32_t year = 1948 + tc.whole/(365*4+1)*4; tc.whole %= (365*4+1);

        // handle remainder of years
        int32_t curleap=1;
        if (tc.whole > 365) {
            year += 1;            tc.whole -= 366;
            year += tc.whole/365; tc.whole %= 365;
            curleap=0;
        }

        size_t ii;
        for (ii=0; ii < 12; ii++) {
            int32_t days = mdays[ii] + (((ii == 1) && curleap) ? 1 : 0);
            if (tc.whole < days) {
                break;
            }
            tc.whole -= days;
        }

        int32_t day   = tc.whole+1;
        int32_t month = ii + 1;
        return (datetime){ year, month, day, hour, min, sec + tc.fract };
    }

    static inline timecode operator +(timecode tc, double dd) {
        return normalize((timecode){ tc.whole, tc.fract + dd});
    }

    static inline timecode operator +(double dd, timecode tc) {
        return normalize((timecode){ tc.whole, tc.fract + dd});
    }

    static inline timecode operator -(timecode tc, double dd) {
        return normalize((timecode){ tc.whole, tc.fract - dd});
    }

    static inline double operator -(timecode aa, timecode bb) {
        return (double)(aa.whole - bb.whole) + (aa.fract - bb.fract);
    }

    static inline timecode& operator +=(timecode& tc, double dd) {
        return tc = tc + dd;
    }

    static inline timecode& operator -=(timecode& tc, double dd) {
        return tc = tc - dd;
    }

    static inline bool operator <(timecode aa, timecode bb) {
        return (aa - bb) < 0;
    }

    static inline bool operator >(timecode aa, timecode bb) {
        return (aa - bb) > 0;
    }

    static inline bool operator <=(timecode aa, timecode bb) {
        return (aa - bb) <= 0;
    }

    static inline bool operator >=(timecode aa, timecode bb) {
        return (aa - bb) >= 0;
    }

    timecode timecode::parse(const string& ss) {
        datetime dt;
        int res = sscanf(
            // XXX: This will silently ignore any timezone at the end.
            // It should work with Z for UTC or none (defaulting to UTC).
            ss.data(), " %u%*[/:-]%u%*[/:-]%u%*[/:T -]%u:%u:%lf ",
            &dt.year, &dt.month, &dt.day,
            &dt.hour, &dt.min, &dt.secs
        );
        check(res == 6, "expected valid timecode string '%s'", ss.data());

        return gettime(dt);
    }

    static inline string format(timecode tc, int32_t places=12) {
        datetime dt = getdate(tc, places);
        if (places <= 0) {
            return xm::format(
                "%04d-%02d-%02dT%02d:%02d:%02.0f",
                dt.year, dt.month, dt.day, dt.hour, dt.min, dt.secs
            );
        }
        return xm::format(
            "%04d-%02d-%02dT%02d:%02d:%0*.*lf",
            dt.year, dt.month, dt.day, dt.hour, dt.min,
            places + 3, places, dt.secs
        );
    }

    /*
    static inline std::ostream& operator <<(std::ostream& os, const timecode& tc) {
        os << "timecode(whole: " << tc.whole << ", fract: " << tc.fract << ", text: " << format(tc) << ")";
        return os;
    }
    */

    //}}}
    //{{{ Raw Files and Memory Mapping:  rawfile 
    //{{{ rawfile
    namespace internal {

        // C++ doesn't provide a good way to work directly with file descriptors.
        // This class implements a container for the descriptor and a mmap ptr.
        //
        // This class returns errors rather than throwing exceptions, and we're
        // hiding it in an internal namespace for now.  Don't rely on it staying.
        //
        struct rawfile;
        static inline void swap(rawfile& flip, rawfile& flop);

        struct rawfile {
            ~rawfile() { clear(); }

            // default constructor or with file descriptor
            rawfile(int fd=-1) : fd(fd), ptr(0), len(0) {}

            /* XXX:
            // move constructor steals from the other object
            rawfile(rawfile&& other) : fd(other.fd), ptr(other.ptr), len(other.len) {
                other.fd = -1;
                other.ptr = 0;
                other.len = 0;
            }
            */

            /* XXX:
            rawfile& operator =(rawfile&& other) {
                clear();
                fd  = other.fd; other.fd  = -1;
                ptr = other.ptr; other.ptr = 0;
                len = other.len; other.len = 0;
                return *this;
            }
            */


            inline bool read(void* data, int64 bytes);
            inline bool write(const void* data, int64 bytes);
            inline bool seek(int64 offset);
            inline bool skip(int64 bytes);
            inline int64 size() const;
            inline const void* mmap();

            inline bool isfile() const;

            private:
                // no copies or defaults
                void operator =(rawfile& other);// = delete;
                rawfile(const rawfile&);// = delete;

                void clear() {
                    if (ptr) {
                        msync(ptr, len, MS_SYNC);
                        munmap(ptr, len);
                    }
                    if (fd >= 0) close(fd);
                }
                friend void swap(rawfile& flip, rawfile& flop);

                // file descriptor
                int fd;
                // mmap fields
                void* ptr;
                int64 len;
        };

        bool rawfile::read(void* ptr, int64 len) {
            char* buf = (char*)ptr;
            int64 want = len;
            while (want) {
                int64 got = ::read(fd, buf, want);
                if (got <= 0) {
                    break;
                }
                want -= got;
                buf += got;
            }
            return want == 0;
        }

        bool rawfile::write(const void* ptr, int64 len) {
            const char* buf = (const char*)ptr;
            while (len) {
                int64 put = ::write(fd, buf, len);
                if (put < 0) {
                    break;
                }
                len -= put;
                buf += put;
            }
            return len == 0;
        }

        bool rawfile::seek(int64 offset) {
            off_t result = ::lseek(fd, offset, SEEK_SET);
            return result != (off_t)-1;
        }

        bool rawfile::skip(int64 bytes) {
            if (bytes == 0) {
                return true;
            }
            off_t result = ::lseek(fd, bytes, SEEK_CUR);
            if (result == (off_t)-1) {
                if (errno != ESPIPE) {
                    return false;
                }
                // Can't quickly seek, so we'll manually dump data
                static char ignored[8192];
                while (bytes) {
                    int64 amt = bytes;
                    if (amt > 8192) {
                        amt = 8192;
                    }
                    int64 got = ::read(fd, ignored, amt);
                    if (got != amt) {
                        return false;
                    }
                    bytes -= got;
                }
            }
            return true;
        }

        int64 rawfile::size() const {
            struct stat st;
            check(fstat(fd, &st) == 0, "fstat");
            return st.st_size;
        }

        const void* rawfile::mmap() {
            if (ptr) return ptr;
            len = size();
            return ptr = (char*)::mmap(0, len, PROT_READ, MAP_SHARED, fd, 0);
        }

        bool rawfile::isfile() const {
            struct stat st;
            check(fstat(fd, &st) == 0, "fstat");
            return st.st_mode & S_IFREG;
        }

        static inline void swap(rawfile& flip, rawfile& flop) {
            xm::swap(flip.fd, flop.fd);
            xm::swap(flip.ptr, flop.ptr);
            xm::swap(flip.len, flop.len);
        }
    }

    //}}}
    //}}}
    //{{{ Unique Identifier              uniqueid 
    //{{{ uniqueid
    struct uniqueid {
        uint8_t bytes[16];

        static inline uniqueid parse(const string& text);
    };

    static inline size_t hash(const uniqueid& uuid) {
        return hash(uuid.bytes, 16, 0);
    }

    static inline bool operator == (const uniqueid& aa, const uniqueid& bb) {
        return memcmp(aa.bytes, bb.bytes, 16) == 0;
    }

    static inline bool operator != (const uniqueid& aa, const uniqueid& bb) {
        return memcmp(aa.bytes, bb.bytes, 16) != 0;
    }

    static inline uniqueid genuuid() {
        using namespace internal;
        int fd = open("/dev/urandom", O_RDONLY);
        check(fd >= 0, "opening '/dev/urandom' for reading");
        rawfile file(fd);

        uniqueid result;
        check(file.read(result.bytes, 16), "reading /dev/urandom");
        // this is the mask for randomly generated uuids
        // ffffffff-ffff-4fff-bfff-ffffffffffff
        result.bytes[6] &= 0x0f;
        result.bytes[6] |= 0x40;
        result.bytes[8] &= 0x0f;
        result.bytes[8] |= 0xb0;
        return result;
    }

    static inline string format(const uniqueid& uuid) {
        return format(
            "%02x%02x%02x%02x-%02x%02x-%02x%02x-%02x%02x-%02x%02x%02x%02x%02x%02x",
            uuid.bytes[ 0], uuid.bytes[ 1], uuid.bytes[ 2], uuid.bytes[ 3],
            uuid.bytes[ 4], uuid.bytes[ 5], uuid.bytes[ 6], uuid.bytes[ 7],
            uuid.bytes[ 8], uuid.bytes[ 9], uuid.bytes[10], uuid.bytes[11],
            uuid.bytes[12], uuid.bytes[13], uuid.bytes[14], uuid.bytes[15]
        );
    }

    inline uniqueid uniqueid::parse(const string &text) {
        uniqueid uuid;
        if(text.size() == 32) {
            check(sscanf(
                text.data(), "%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx"
                "%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx",
                uuid.bytes +  0, uuid.bytes +  1, uuid.bytes +  2, uuid.bytes +  3,
                uuid.bytes +  4, uuid.bytes +  5, uuid.bytes +  6, uuid.bytes +  7,
                uuid.bytes +  8, uuid.bytes +  9, uuid.bytes + 10, uuid.bytes + 11,
                uuid.bytes + 12, uuid.bytes + 13, uuid.bytes + 14, uuid.bytes + 15
            ) == 16, "expected 16 pairs of hex chars with no hyphens");
        }
        else {
            check(text.size() == 36, "expecting a 36 byte string");
            check(sscanf(
                text.data(), "%2hhx%2hhx%2hhx%2hhx-%2hhx%2hhx-"
                "%2hhx%2hhx-%2hhx%2hhx-%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx",
                uuid.bytes +  0, uuid.bytes +  1, uuid.bytes +  2, uuid.bytes +  3,
                uuid.bytes +  4, uuid.bytes +  5, uuid.bytes +  6, uuid.bytes +  7,
                uuid.bytes +  8, uuid.bytes +  9, uuid.bytes + 10, uuid.bytes + 11,
                uuid.bytes + 12, uuid.bytes + 13, uuid.bytes + 14, uuid.bytes + 15
            ) == 16, "expected 16 pairs of hex chars with hyphens in the right places");
        }
        return uuid;
    }

    /* XXX
    static inline std::ostream& operator <<(std::ostream& os, const uniqueid& uuid) {
        os << format(uuid);
        return os;
    }
    */

    //}}}
    //}}}
    //{{{ Blue File Metadata:            bluekeyword, bluefield
    //}}}
    //{{{ Blue File Readers:             bluereader, load1000, load2000 
    //}}}
    //{{{ Blue File Writers:             bluewriter, dump1000, dump2000 
    //}}}
    //{{{ State Vectors:                 statevec, timestate, ephemeris, ephcache 
    //{{{ statevec
    struct statevec {
        cartesian pos, vel, acc;
    };

    static inline statevec operator +(statevec aa, statevec bb) {
    	return (statevec){ aa.pos + bb.pos, aa.vel + bb.vel, aa.acc + bb.acc};
    }

    /*
    static inline std::ostream& operator <<(std::ostream& os, const statevec& sv) {
        os << "statevec(";
        os << "pos: { " << sv.pos.x << ", " << sv.pos.y << ", " << sv.pos.z << " }, ";
        os << "vel: { " << sv.vel.x << ", " << sv.vel.y << ", " << sv.vel.z << " }, ";
        os << "acc: { " << sv.acc.x << ", " << sv.acc.y << ", " << sv.acc.z << " }";
        os << ")";
        return os;
    }
    */

    //}}}
    //{{{ timestate
    struct timestate {
        timecode tc;
        statevec sv;
    };

    static inline bool operator <(const timestate& aa, const timestate& bb) {
        return (aa.tc - bb.tc) < 0.0;
    }

    namespace internal {

        // Evaluates an interpolating polynomial:
        //
        //      P(t) = a + b*t + c*t^2 + d*t^3 + e*t^4 + f*t^5
        //
        //  Such that:
        //
        //      P(-1) = plo      P(+1) = phi
        //     P'(-1) = vlo     P'(+1) = vhi
        //     P"(-1) = alo     P"(+1) = ahi
        //
        static double hermite5(
            double plo, double vlo, double alo,
            double phi, double vhi, double ahi,
            double tt
        ) {
            plo *= .0625; vlo *= .0625; alo *= .0625;
            phi *= .0625; vhi *= .0625; ahi *= .0625;

            double aa =   8*phi +  8*plo -  5*vhi +  5*vlo + 1*ahi + 1*alo;
            double bb =  15*phi - 15*plo -  7*vhi -  7*vlo + 1*ahi - 1*alo;
            double cc =                     6*vhi -  6*vlo - 2*ahi - 2*alo;
            double dd = -10*phi + 10*plo + 10*vhi + 10*vlo - 2*ahi + 2*alo;
            double ee =                  -  1*vhi +  1*vlo + 1*ahi + 1*alo;
            double ff =   3*phi -  3*plo -  3*vhi -  3*vlo + 1*ahi - 1*alo;

            return aa + (bb + (cc + (dd + (ee + ff*tt)*tt)*tt)*tt)*tt;
        }

        // Evaluates an interpolating polynomial:
        //
        //      P(t) = a + b*t + c*t^2 + d*t^3
        //
        //  Such that:
        //
        //      P(-1) = plo      P(+1) = phi
        //     P'(-1) = vlo     P'(+1) = vhi
        //
        static double hermite3(
            double plo, double vlo,
            double phi, double vhi,
            double tt
        ) {
            plo *= .25; vlo *= .25;
            phi *= .25; vhi *= .25;

            double aa =  2*phi + 2*plo - 1*vhi + 1*vlo;
            double bb =  3*phi - 3*plo - 1*vhi - 1*vlo;
            double cc =                  1*vhi - 1*vlo;
            double dd = -1*phi + 1*plo + 1*vhi + 1*vlo;

            return aa + (bb + (cc + dd*tt)*tt)*tt;
        }

        // This is really just linear interpolation with t in [-1,+1]
        static double hermite1(double plo, double phi, double tt) {
            return (plo*(1.0 - tt) + phi*(1.0 + tt))*0.5;
        }

    }

    static inline statevec interp(
        const timestate& ts0, const timestate& ts1, const timecode& tc
    ) {
        using namespace internal;
        double range = ts1.tc - ts0.tc;
        double half = range*.5;
        double tt = ((tc - ts1.tc) + (tc - ts0.tc)) / range;

        return (statevec){
            (cartesian){
                hermite5(
                    ts0.sv.pos.x, ts0.sv.vel.x*half, ts0.sv.acc.x*half*half,
                    ts1.sv.pos.x, ts1.sv.vel.x*half, ts1.sv.acc.x*half*half, tt
                ),
                hermite5(
                    ts0.sv.pos.y, ts0.sv.vel.y*half, ts0.sv.acc.y*half*half,
                    ts1.sv.pos.y, ts1.sv.vel.y*half, ts1.sv.acc.y*half*half, tt
                ),
                hermite5(
                    ts0.sv.pos.z, ts0.sv.vel.z*half, ts0.sv.acc.z*half*half,
                    ts1.sv.pos.z, ts1.sv.vel.z*half, ts1.sv.acc.z*half*half, tt
                )
            },
            (cartesian){
                hermite3(
                    ts0.sv.vel.x, ts0.sv.acc.x*half,
                    ts1.sv.vel.x, ts1.sv.acc.x*half, tt
                ),
                hermite3(
                    ts0.sv.vel.y, ts0.sv.acc.y*half,
                    ts1.sv.vel.y, ts1.sv.acc.y*half, tt
                ),
                hermite3(
                    ts0.sv.vel.z, ts0.sv.acc.z*half,
                    ts1.sv.vel.z, ts1.sv.acc.z*half, tt
                )
            },
            (cartesian){
                hermite1(ts0.sv.acc.x, ts1.sv.acc.x, tt),
                hermite1(ts0.sv.acc.y, ts1.sv.acc.y, tt),
                hermite1(ts0.sv.acc.z, ts1.sv.acc.z, tt)
            }
        };
    }
    //}}}
    //}}}
    //{{{ Light Time:                    lightfwd, lightrev 
    //}}}
    //{{{ DTED Processing:               dtedtile, dtedcache 
    //{{{ dtedtile
    namespace internal {
        static int16_t dtedsignmag(uint16_t xx) {
            xx = be16toh(xx);
            int16_t signbit = xx&0x8000;
            int16_t magbits = xx&0x7FFF;
            if (signbit) magbits *= -1;
            return magbits;
        }

        static int16_t dtedgetbits(const char* data, int64 off, int64 len) {
            int64 byte = off/8;
            int64 bit = off%8;
            int32_t result = 0;
            memcpy(&result, data + byte, sizeof(int32_t));
            result = be32toh(result);
            result <<= bit;
            result >>= 32 - len;
            return result;
        }
    }

    struct dtedtile {
        inline dtedtile(const string& path);

        //inline ~dtedtile() = default;
        inline dtedtile() : lats(0), lons(0) {}
        //inline dtedtile(const dtedtile&) = default;
        //inline dtedtile& operator =(const dtedtile&) = default;

        inline double lookup(double fractlat, double fractlon);
        inline bool isvalid();

        private:
            // data is stored column major, starting from the lower
            // left through each latitude, then increasing longitude.
            int64 lats, lons;
            vector<int16_t> data;
    };

    dtedtile::dtedtile(const string& path) {
        using namespace internal;

        // An empty path is used to indicate the absence of a tile.
        // This is used by dtedcache to indicate it should use MSL lookup.
        if (path == "") return;

        int fd = open(path.data(), O_RDONLY);
        check(fd >= 0, "opening '%s' for reading", path.data());
        rawfile file(fd);
        const char* ptr = (const char*)file.mmap();

        if (memcmp(ptr, "UHL", 3) == 0) {
            // uncompressed data
            char temp[5] = {};
            memcpy(temp, ptr + 47, 4);
            lons = strtol(temp, 0, 10);
            memcpy(temp, ptr + 51, 4);
            lats = strtol(temp, 0, 10);
            data.resize(lats*lons);
            for (int64 ii = 0; ii<lons; ii++) {
                const int64 skip = 80 + 648 + 2700 + 8;
                const int64 offset = skip + ii*(12 + lats*sizeof(int16_t));
                memcpy(&data[ii*lats], ptr + offset, lats*sizeof(int16_t));
                for (int64 jj = 0; jj<lats; jj++) {
                    data[ii*lats + jj] = dtedsignmag(data[ii*lats + jj]);
                }
            }
            return;
        }

        if (memcmp(ptr, "DTC", 3) == 0) {
            // compressed data
            char temp[5] = {};
            memcpy(temp, ptr + 51, 4);
            lons = strtol(temp, 0, 10);
            memcpy(temp, ptr + 55, 4);
            lats = strtol(temp, 0, 10);
            data.resize(lats*lons);

            const uint32_t* offset = (const uint32_t*)(ptr + 80 + 648 + 23);

            const int64 blockstart = 80 + 648 + 23 + 400;
            for (int64 ii = 0; ii<10; ii++) {
                for (int64 jj = 0; jj<10; jj++) {

                    const char* blockptr = ptr + offset[ii*10 + jj] + blockstart + 4;
                    const int64 blocklats = lats / 10 + (ii == 9 ? lats%10 : 0);
                    const int64 blocklons = lons / 10 + (jj == 9 ? lons%10 : 0);

                    int8_t nchunks = *(int8_t*)blockptr;
                    blockptr += 1;

                    for (int64 kk = 0; kk<blocklons; kk++) {
                        int16_t* line = &data[jj*lats*(lons/10) + ii*(lats/10) + kk*lats];

                        int64 total = 0;

                        for (int64 nn = 0; nn<nchunks; nn++) {
                            int64 samples = blocklats/nchunks + (nn == nchunks-1 ? blocklats%nchunks : 0);
                            uint8_t nbits = *(uint8_t*)(blockptr);
                            blockptr += 1;
                            check(nbits == 0xFF || nbits <= 16, "in bounds");

                            if (nbits == 0xFF) {
                                memcpy(line, blockptr, samples*2);
                                blockptr += samples*2;
                            } else {
                                int16_t accum = *(int16_t*)(blockptr);
                                blockptr += 2;
                                int64 bitoff = 0;
                                for (int64 ss = 0; ss<samples; ss++) {
                                    line[ss] = accum;
                                    if (ss != samples-1 && nbits > 0) {
                                        accum += dtedgetbits(blockptr, bitoff, nbits);
                                        bitoff += nbits;
                                    }
                                }
                                blockptr += (bitoff + 7)/8;
                            }
                            line += samples;
                            total += samples;
                        }

                        check(total == blocklats, "sanity %lld %lld", total, blocklats);
                    }
                }
            }

            return;
        }

        check(false, "unrecognized DTED file format");
    }

    double dtedtile::lookup(double fractlat, double fractlon) {
        check(fractlat >= 0.0, "fract lat in bounds: %lf", fractlat);
        check(fractlon >= 0.0, "fract lon in bounds: %lf", fractlon);
        check(fractlat <  1.0, "fract lat in bounds: %lf", fractlat);
        check(fractlon <  1.0, "fract lon in bounds: %lf", fractlon);

        double floatrow = (lats - 1)*fractlat;
        double floatcol = (lons - 1)*fractlon;
        int64 wholerow = floor(floatrow);
        int64 wholecol = floor(floatcol);
        double fractrow = floatrow - wholerow;
        double fractcol = floatcol - wholecol;

        //check(wholerow >= 0, "in bounds wholerow %lld < 0", wholerow);
        //check(wholecol >= 0, "in bounds wholecol %lld < 0", wholecol);
        //check(wholerow < lats - 1, "in bounds wholerow %lld < %lld", wholerow, lats - 1);
        //check(wholecol < lons - 1, "in bounds wholecol %lld < %lld", wholecol, lons - 1);

        double bl = data[(wholerow + 0) + lats*(wholecol + 0)];
        double br = data[(wholerow + 0) + lats*(wholecol + 1)];
        double tl = data[(wholerow + 1) + lats*(wholecol + 0)];
        double tr = data[(wholerow + 1) + lats*(wholecol + 1)];

        return (
            bl*(1.0 - fractrow)*(1.0 - fractcol) +
            br*(1.0 - fractrow)*(0.0 + fractcol) +
            tl*(0.0 + fractrow)*(1.0 - fractcol) +
            tr*(0.0 + fractrow)*(0.0 + fractcol)
        );
    }

    bool dtedtile::isvalid() {
        return data.size() > 0;
    }

    //}}}
    //{{{ dtedcache
    namespace internal {
        struct latlon {
            int32_t lat;
            int32_t lon;
            bool operator ==(const latlon& other) const {
                return lat == other.lat && lon == other.lon;
            }
        };

        static inline size_t hash(const latlon& ll) {
            return xm::hash(ll.lat) ^ 5*xm::hash(ll.lon);
        }
    }

    struct dtedcache {
        //inline ~dtedcache() = default;
        //inline dtedcache(const dtedcache&) = default;
        //inline dtedcache& operator =(const dtedcache&) = default;
        inline dtedcache(int64 maxtiles=64);

        inline double lookup(double lat, double lon);

        private:
            inline string trypath(int32_t lat, int32_t lon);
            int64 maxsize;
            int64 counter;
            struct countdted {
                int64 count;
                dtedtile tile;
            };
            dict<internal::latlon, countdted> cache;

            inline double getmsl(double lat, double lon);

            // MSL data is stored row-major starting from
            // 0-lon at the north pole, moving southward per row
            enum { MSL_LATS = 180*4 + 1, MSL_LONS = 360*4 + 1 };
            vector<float> msl;
    };

    dtedcache::dtedcache(int64 maxtiles) : maxsize(maxtiles), counter(0) {
        using namespace internal;

        // This should be: /full/path/to/WW15MGH.BIN
        char* dtedmsl = getenv("DTED_MSL");
        if (!dtedmsl) return;

        int fd = open(dtedmsl, O_RDONLY);
        check(fd >= 0, "opening '%s' for reading", dtedmsl);
        rawfile file(fd);
        check(
            file.size() >= int64((MSL_LATS*MSL_LONS + 6)*sizeof(float)),
            "expected size for MSL"
        );
        const float* ptr = (const float*)file.mmap();
        msl.resize(MSL_LATS*MSL_LONS + 6);
        memcpy(msl.data(), ptr, (MSL_LATS*MSL_LONS + 6)*sizeof(float));

#if __BYTE_ORDER == __BIG_ENDIAN
        // XXX: reimplement this without C++11
        for (auto ii = msl.begin(); ii != msl.end(); ++ii) {
            byteswap4(&*ii);
        }
#endif
        check(msl[0] == -90, "expected 0");
        check(msl[1] == +90, "expected 1");
        check(msl[2] ==   0, "expected 2");
        check(msl[3] == 360, "expected 3");
        check(msl[4] == .25, "expected 4");
        check(msl[5] == .25, "expected 5");
    }

    string dtedcache::trypath(int32_t lat, int32_t lon) {
        const char* dteddir = getenv("DTED_DIR");
        check(dteddir != 0, "DTED_DIR must be set");

        char ns = 'n';
        char ew = 'e';
        if (lat < 0) {
            ns = 's';
            lat *= -1;
        }
        if (lon < 0) {
            ew = 'w';
            lon *= -1;
        }

        list<string> paths;
        paths.append(format("%s/%c%03d/%c%02d.cdt2", dteddir, ew, lon, ns, lat));
        paths.append(format("%s/%c%03d/%c%02d.cdt1", dteddir, ew, lon, ns, lat));
        paths.append(format("%s/%c%03d/%c%02d.dt0",  dteddir, ew, lon, ns, lat));

        for (int64 ii = 0; ii<paths.size(); ii++) {
            const string& path = paths[ii];
            struct stat buf;
            if (stat(path.data(), &buf) == 0) {
                return path;
            }
        }

        return "";
    }

    inline double dtedcache::lookup(double lat, double lon) {
        using namespace internal;

        int32_t wholelat = (int32_t)floor(lat);
        int32_t wholelon = (int32_t)floor(lon);
        double fractlat = lat - wholelat;
        double fractlon = lon - wholelon;
        latlon ll = { wholelat, wholelon };

        if (!cache.haskey(ll)) {
            if ((int64)cache.size() >= maxsize) {
                // the oldest will have the lowest count
                latlon oldest = (latlon){ -1, -1 };
                int64 lowest = INT64_MAX;
                for (int64 ii = 0; ii<cache.bins(); ii++) {
                    if (cache.skip(ii)) continue;
                    if (cache.val(ii).count < lowest) {
                        oldest = cache.key(ii);
                        lowest = cache.val(ii).count;
                    }
                }
                cache.remove(oldest);
            }
            cache[ll] = (countdted){ counter, dtedtile(trypath(wholelat, wholelon)) };
        }

        countdted& dt = cache[ll];
        dt.count = ++counter;
        if (dt.tile.isvalid()) {
            return dt.tile.lookup(fractlat, fractlon);
        }

        return getmsl(lat, lon);
    }

    double dtedcache::getmsl(double lat, double lon) {
        if (msl.size() == 0) return 0.0;
        if (lat >= +90) return msl[0];
        if (lat <= -90) return msl[MSL_LATS*MSL_LONS - 1];

        double rowcoord = 90.0 - lat;
        double colcoord = fmod(lon + 360.0, 360.0);
        double rowfloat = rowcoord*((MSL_LATS - 1)/180.0);
        double colfloat = colcoord*((MSL_LONS - 1)/360.0);
        int64 rowwhole = floor(rowfloat);
        int64 colwhole = floor(colfloat);
        check(rowwhole >= 0, "lat in bounds: %lf >= 0", lat);
        check(colwhole >= 0, "lon in bounds: %lf >= 0", lon);
        check(rowwhole < MSL_LATS - 1, "lat in bounds: %lf (%lld)", lat, rowwhole);
        check(colwhole < MSL_LONS - 1, "lon in bounds: %lf (%lld)", lon, colwhole);
        double rowfract = rowfloat - rowwhole;
        double colfract = colfloat - colwhole;

        float tl = msl[(rowwhole + 0)*MSL_LONS + (colwhole + 0) + 6];
        float tr = msl[(rowwhole + 0)*MSL_LONS + (colwhole + 1) + 6];
        float bl = msl[(rowwhole + 1)*MSL_LONS + (colwhole + 0) + 6];
        float br = msl[(rowwhole + 1)*MSL_LONS + (colwhole + 1) + 6];

        return (
            tl*(1.0 - rowfract)*(1.0 - colfract) +
            tr*(1.0 - rowfract)*(0.0 + colfract) +
            bl*(0.0 + rowfract)*(1.0 - colfract) +
            br*(0.0 + rowfract)*(0.0 + colfract)
        );
    }


    //}}}
    //}}}
    //{{{ Graphical Windows:             graphics, pixel, message, runwin 
    //}}}
    //{{{ Drawing Functions:             drawline, fillrect, drawtext 
    //}}}
    //{{{ Plotting Functions:            plotframe, plotseries, plotpoints 
    //}}}
    //{{{ Command Line Parsing:          cmdline 

    struct cmdline {
        inline ~cmdline();
        inline cmdline(int argc, char const * const * const argv, string doc);

        inline string         getstring(    string name, string    def, string doc);
        inline double         getdouble(    string name, double    def, string doc);
        inline int64          getint64(     string name, int64     def, string doc);
        inline timecode       gettimecode(  string name, timecode  def, string doc);
        inline cartesian      getcartesian( string name, cartesian def, string doc);
        inline geodetic       getgeodetic(  string name, geodetic  def, string doc);

        inline string         getstring(    string name, string doc);
        inline double         getdouble(    string name, string doc);
        inline int64          getint64(     string name, string doc);
        inline timecode       gettimecode(  string name, string doc);
        inline cartesian      getcartesian( string name, string doc);
        inline geodetic       getgeodetic(  string name, string doc);

        inline bool           getswitch(    string name, string doc);

        inline string         getinput(     string name, string doc);
        inline string         getoutput(    string name, string doc);
        inline list<string>   getinputs(    string name, string doc);

        inline void done();

        private:
            cmdline();// = delete;
            cmdline(const cmdline&);// = delete;
            void operator =(const cmdline&);// = delete;

            inline const char* find_argument(const string& name);

            __attribute__ ((format (scanf, 7, 8)))
            inline bool scan_argument(
                string name, bool required, const char* type,
                const string& doc, int count, const char* format, ...
            );

            list<const char*> args;
            bool switches_done;
            bool need_help;
            bool done_called;
    };

    cmdline::~cmdline() { check(done_called, "must call cmdline .done()"); }

    cmdline::cmdline(int argc, char const * const * const argv, string doc)
        : switches_done(false), need_help(false), done_called(false)
    {
        for (int ii = 1; ii<argc; ii++) {
            args.append(argv[ii]);
            if (::strcmp(argv[ii], "-h") == 0 || ::strcmp(argv[ii], "-help") == 0) {
                need_help = true;
            }
        }
        if (need_help) {
            fprintf(stderr, "\n");
            fprintf(stderr, "Usage: %s [arguments ...]\n", argv[0]);
            fprintf(stderr, "    %s\n\n", doc.data());
            fprintf(stderr, "Arguments:\n");
        }
    }

    string cmdline::getstring(string name, string def, string doc) {
        check(switches_done == false, "switches must be processed first");
        name = format("-%s", name.data()); //name = "-" + name;
        if (need_help) {
            fprintf(
                stderr, "    %s <string, default='%s'>\n\t%s\n",
                name.data(), def.data(), doc.data()
            );
            return "";
        }

        const char* val = find_argument(name);
        return val ? val : def;
    }

    double cmdline::getdouble(string name, double def, string doc) {
        double val = 0;
        doc = format("<double, default=%lg>\n\t%s", def, doc.data());
        return scan_argument(name, false, "double", doc, 1, " %lf ", &val) ? val : def;
    }

    int64 cmdline::getint64(string name, int64 def, string doc) {
        int64 val = 0;
        doc = format("<int64, default=%lld>\n\t%s", def, doc.data());
        return scan_argument(name, false, "int64", doc, 1, " %lld ", &val) ? val : def;
    }

    timecode cmdline::gettimecode(string name, timecode def, string doc) {
        check(switches_done == false, "switches must be processed first");
        name = format("-%s", name.data()); //name = "-" + name;
        if (need_help) {
            string ts = format(def, 12);
            fprintf(
                stderr, "    %s <timecode, default=%s>\n\t%s\n",
                name.data(), ts.data(), doc.data()
            );
            return timecode();
        }

        const char* val = find_argument(name);
        return val ? timecode::parse(val) : def;
    }

    cartesian cmdline::getcartesian(string name, cartesian def, string doc) {
        cartesian val = cartesian();
        doc = format("<cartesian, default=%lg,%lg,%lg>\n\t%s", def.x, def.y, def.z, doc.data());
        return scan_argument(
            name, false, "cartesian", doc, 3, " %lf , %lf , %lf ", &val.x, &val.y, &val.z
        ) ? val : def;
    }

    geodetic cmdline::getgeodetic(string name, geodetic def, string doc) {
        geodetic val = geodetic();
        doc = format("<geodetic, default=%lg,%lg,%lg>\n\t%s", def.lat, def.lon, def.alt, doc.data());
        return scan_argument(
            name, false, "geodetic", doc, 3, " %lf , %lf , %lf ", &val.lat, &val.lon, &val.alt
        ) ? val : def;
    }

    string cmdline::getstring(string name, string doc) {
        check(switches_done == false, "switches must be processed first");
        name = format("-%s", name.data()); //name = "-" + name;
        if (need_help) {
            fprintf(
                stderr, "    %s <string, required>\n\t%s\n",
                name.data(), doc.data()
            );
            return "";
        }

        const char* val = find_argument(name);
        if (val == 0) {
            fprintf(stderr, "Error: switch '%s' is required\n", name.data());
            exit(-1);
        }
        return val;

    }

    double cmdline::getdouble(string name, string doc) {
        double val = 0;
        doc = format("<double, required>\n\t%s", doc.data());
        scan_argument(name, true, "double", doc, 1, " %lf ", &val);
        return val;
    }

    int64 cmdline::getint64(string name, string doc) {
        int64 val = 0;
        doc = format("<int64, required>\n\t%s", doc.data());
        scan_argument(name, true, "int64", doc, 1, " %lld ", &val);
        return val;
    }

    timecode cmdline::gettimecode(string name, string doc) {
        check(switches_done == false, "switches must be processed first");
        name = format("-%s", name.data()); //name = "-" + name;
        if (need_help) {
            fprintf(
                stderr, "    %s <timecode, required>\n\t%s\n",
                name.data(), doc.data()
            );
            return timecode();
        }

        const char* val = find_argument(name);
        if (!val) {
            fprintf(stderr, "Error: switch '%s' is required\n", name.data());
            exit(-1);
        }
        return timecode::parse(val);
    }

    cartesian cmdline::getcartesian(string name, string doc) {
        cartesian val = cartesian();
        doc = format("<cartesian, required>\n\t%s", doc.data());
        scan_argument(name, true, "cartesian", doc, 1, " %lf , %lf , %lf ", &val.x, &val.y, &val.z);
        return val;
    }

    geodetic cmdline::getgeodetic(string name, string doc) {
        geodetic val = geodetic();
        doc = format("<geodetic, required>\n\t%s", doc.data());
        scan_argument(name, true, "geodetic", doc, 1, " %lf , %lf , %lf ", &val.lat, &val.lon, &val.alt);
        return val;
    }

    bool cmdline::getswitch(string name, string doc) {
        check(switches_done == false, "switches must be processed first");
        name = format("-%s", name.data()); //name = "-" + name;
        if (need_help) {
            fprintf(stderr, "    %s\n\t%s\n", name.data(), doc.data());
            return false;
        }
        for (int64 ii = 0; ii<args.size(); ii++) {
            if (args[ii] && args[ii] == name) {
                args[ii] = 0;
                return true;
            }
        }
        return false;
    }

    string cmdline::getinput(string name, string doc) {
        switches_done = true;
        if (need_help) {
            fprintf(
                stderr, "    <%s, required input>\n\t%s\n",
                name.data(), doc.data()
            );
            return "";
        }
        for (int64 ii = 0; ii<args.size(); ii++) {
            if (args[ii]) {
                if (args[ii][0] == '-' && ::strcmp(args[ii], "-") != 0) {
                    fprintf(
                        stderr, "Error: found '%s' instead of input file name", args[ii]
                    );
                    exit(-1);
                }
                string result = args[ii];
                args[ii] = 0;
                return result;
            }
        }
        fprintf(
            stderr, "Error: expected an input file name\n"
            "Use -help for a complete list of arguments\n"
        );
        exit(-1);
        return "";
    }

    string cmdline::getoutput(string name, string doc) {
        switches_done = true;
        if (need_help) {
            fprintf(
                stderr, "    <%s, required output>\n\t%s\n",
                name.data(), doc.data()
            );
            return "";
        }
        for (int64 ii = 0; ii<args.size(); ii++) {
            if (args[ii]) {
                if (args[ii][0] == '-' && ::strcmp(args[ii], "-") != 0) {
                    fprintf(
                        stderr, "Error: found '%s' instead of output file name\n"
                        "Use -help for a complete list of arguments\n", args[ii]
                    );
                    exit(-1);
                }
                string result = args[ii];
                args[ii] = 0;
                return result;
            }
        }
        fprintf(
            stderr, "Error: expected an output file name\n"
            "Use -help for a complete list of arguments\n"
        );
        exit(-1);
        return "";
    }

    list<string> cmdline::getinputs(string name, string doc) {
        switches_done = true;
        if (need_help) {
            fprintf(
                stderr, "    <%s, multiple inputs> ...\n\t%s\n",
                name.data(), doc.data()
            );
            return list<string>();
        }
        list<string> results;
        for (int64 ii = 0; ii<args.size(); ii++) {
            if (args[ii]) {
                if (args[ii][0] == '-' && ::strcmp(args[ii], "-") != 0) {
                    fprintf(
                        stderr, "Error: found '%s' instead of input file name\n"
                        "Use -help for a complete list of arguments\n", args[ii]
                    );
                    exit(-1);
                }
                results.append(args[ii]);
                args[ii] = (char*)0;
            }
        }
        return results;
    }

    void cmdline::done() {
        bool die = false;

        if (need_help) {
            fprintf(stderr, "\n");
            exit(0);
        }

        for (int64 ii = 0; ii<args.size(); ii++) {
            if (args[ii] != 0) {
                fprintf(
                    stderr, "Error: Unrecognized or unused argument '%s'\n"
                    "Use -help for a complete list of arguments\n", args[ii]
                );
                die = true;
            }
        }

        if (die) exit(-1);
        done_called = true;
    }

    const char* cmdline::find_argument(const string& name) {
        for (int64 ii = 0; ii<args.size(); ii++) {
            if (args[ii] && args[ii] == name) {
                if (ii + 1 == args.size() || args[ii + 1] == 0) {
                    fprintf(
                        stderr, "switch '%s' requires an argument\n"
                        "Use -help for a complete list of arguments\n", name.data()
                    );
                    exit(-1);
                }
                const char* result = args[ii + 1];
                args[ii + 0] = (char*)0;
                args[ii + 1] = (char*)0;
                return result;
            }
        }
        return 0;
    }

    bool cmdline::scan_argument(
        string name, bool required, const char* type, const string& doc,
        int count, const char* format, ...
    ) {
        check(switches_done == false, "switches must be processed first");
        name = xm::format("-%s", name.data()); //name = "-" + name;

        if (need_help) {
            fprintf(stderr, "    %s %s\n", name.data(), doc.data());
            return false;
        }

        const char* val = find_argument(name);
        if (!val) {
            if (required) {
                fprintf(
                    stderr, "Error: %s switch %s is required\n"
                    "Use -help for a complete list of arguments\n", type, name.data()
                );
                exit(-1);
            }
            return false;
        }

        va_list ap;
        va_start(ap, format);
        int got = vsscanf(val, format, ap);
        va_end(ap);
        if (got < count) {
            fprintf(
                stderr, "Error: %s requires %d arguments\n"
                "Use -help for a complete list of arguments\n", name.data(), count
            );
            exit(-1);
        }

        return true;
    }

    //}}}
    //{{{ Deprecated Functions and Types 
    namespace deprecated {
        //{{{ qrsolve
        //
        // Solve A*X = B for X using QR decomposition
        //
        //    double A[rows * cols]
        //    double B[rows * count]
        //
        // Returns false is A is singular
        //
        // A will be modified during the decomposition
        //
        // If successful, the result will be placed into the top
        // 'cols' rows of B.  B will also be modified on failure
        //
        static inline bool qrsolve (
            double* A, int64 rows, int64 cols,
            double* B, int64 count
        ) {
            check(rows >= cols, "rows %lld must be >= cols %lld", rows, cols);

            for (int64 jj = 0; jj<cols; jj++) {
                double rr = 0;
                for (int64 ii = jj; ii<rows; ii++) {
                    rr = hypot(rr, A[ii*cols + jj]);
                }
                // check if A is singular
                if (rr == 0) return false;

                // for numerical stability
                if (A[jj*cols + jj] > 0) rr = -rr;

                // normalize the Householder vector
                double invrr = 1.0/rr;
                for (int64 ii = jj; ii<rows; ii++) {
                    A[ii*cols + jj] *= invrr;
                }
                double pp = A[jj*cols + jj] -= 1;
                double invpp = 1.0/pp;

                // apply the Householder to the rest of A
                for (int64 kk = jj + 1; kk<cols; kk++) {
                    double sum = 0;
                    for (int64 ii = jj; ii<rows; ii++) {
                        sum += A[ii*cols + jj]*A[ii*cols + kk];
                    }
                    sum *= invpp;
                    for (int64 ii = jj; ii<rows; ii++) {
                        A[ii*cols + kk] += sum*A[ii*cols + jj];
                    }
                }

                // apply the Householder to all of B
                for (int64 kk = 0; kk<count; kk++) {
                    double sum = 0;
                    for (int64 ii = jj; ii<rows; ii++) {
                        sum += A[ii*cols + jj]*B[ii*count + kk];
                    }
                    sum *= invpp;
                    for (int64 ii = jj; ii<rows; ii++) {
                        B[ii*count + kk] += sum*A[ii*cols + jj];
                    }
                }

                A[jj*cols + jj] = rr;
            }

            // back substitute R into B
            for (int64 jj = cols - 1; jj >= 0; jj--) {
                // divide by diagonal element
                double dd = A[jj*cols + jj];
                double invdd = 1.0/dd;
                for (int64 kk = 0; kk<count; kk++) {
                    B[jj*count + kk] *= invdd;
                }

                // subtract from other rows
                for (int64 ii = jj - 1; ii >= 0; ii--) {
                    double ss = A[ii*cols + jj];
                    for (int64 kk = 0; kk<count; kk++) {
                        B[ii*count + kk] -= ss*B[jj*count + kk];
                    }
                }
            }

            return true;
        }

        // Note for the next several wrappers: we intentionally take A and B by value
        // so that copies will be made.  The worker routines will modify these copies.
        static inline matrix<double> qrsolve(matrix<double> A, matrix<double> B) {
            check(A.rows() >= A.cols(), "can't be underdetermined");
            check(A.rows() == B.rows(), "need compatible sizes");
            check(
                qrsolve(A.data(), A.rows(), A.cols(), B.data(), B.cols()),
                "matrix must not be singular"
            );
            matrix<double> X(A.cols(), B.cols());
            for (int64 ii = 0; ii<X.rows(); ii++) {
                for (int64 jj = 0; jj<X.cols(); jj++) {
                    X(ii, jj) = B(ii, jj);
                }
            }
            return X;
        }

        static inline vector<double> qrsolve(matrix<double> A, vector<double> b) {
            check(A.rows() >= A.cols(), "can't be underdetermined");
            check(A.rows() == b.size(), "need compatible sizes");
            check(
                qrsolve(A.data(), A.rows(), A.cols(), b.data(), 1),
                "matrix must not be singular"
            );
            vector<double> x(A.cols());
            for (int64 ii = 0; ii<x.size(); ii++) {
                x[ii] = b[ii];
            }
            return x;
        }
        //}}}
        //{{{ ldsolve
        //
        // Factor A using LDL^t decomposition.
        //
        // The matrix must be symmetric, but this is not checked.  The decomp is
        // placed in the lower half of the A matrix.  This decomp is similar to a
        // Cholesky decomp, but it does not require the matrix to be positive
        // definite, only symmetric.  This function always succeeds, but the decomp
        // is only unique when the matrix is non-singular.
        //
        static inline void ldfactor(double* A, int64 size) {
            for (int64 jj = 0; jj < size; jj++) {
                // Solve for Diagonal
                double ajj = A[jj*size + jj];
                for (int64 kk = 0; kk < jj; kk++) {
                    double dkk = A[kk*size + kk];
                    double ljk = A[jj*size + kk];
                    ajj -= ljk*(ljk)*dkk;
                }
                A[jj*size + jj] = ajj;
                if (ajj == 0) ajj = 1;

                // Solve for Lowers
                for (int64 ii = jj + 1; ii < size; ii++) {
                    double aij = A[ii*size + jj];
                    for (int64 kk = 0; kk < jj; kk++) {
                        double lik = A[ii*size + kk];
                        double ljk = A[jj*size + kk];
                        double dkk = A[kk*size + kk];
                        aij -= lik*(ljk)*dkk;
                    }
                    A[ii*size + jj] = aij/ajj;
                }
            }
        }

        //
        // Check if the LD decomposition is positive definite.
        //
        static inline bool ldposdef(const double* LD, int64 size) {
            for (int64 ii = 0; ii<size; ii++) {
                if (LD[ii*size + ii] <= 0) return false;
            }
            return true;
        }

        //
        // Use the LDL^t decomposition in A to solve A*X = B.  X = A\B.
        //
        //     double LD[size * size]
        //     double B[size * count]
        //
        // This function fails when the decomposition is singular (zero on diagonal)
        //
        static inline bool lddivide(const double* LD, int64 size, double* B, int64 count) {
            for (int64 ii = 0; ii<size; ii++) {
                if (LD[ii*size + ii] == 0) return false;
            }
            // Invert the Lower
            for (int64 jj = 0; jj < size - 1; jj++) {
                for (int64 ii = jj + 1; ii < size; ii++) {
                    for (int64 kk = 0; kk<count; kk++) {
                        B[ii*count + kk] -= LD[ii*size + jj]*B[jj*count + kk];
                    }
                }
            }
            // Invert the Diagonal
            for (int64 ii = 0; ii < size; ii++) {
                for (int64 kk = 0; kk<count; kk++) {
                    B[ii*count + kk] /= LD[ii*size + ii];
                }
            }
            // Invert the Lower Transpose
            for (int64 ii = size - 1; ii > 0; ii--) {
                for (int64 jj = 0; jj < ii; jj++) {
                    for (int64 kk = 0; kk<count; kk++) {
                        B[jj*count + kk] -= LD[ii*size + jj]*B[ii*count + kk];
                    }
                }
            }
            return true;
        }

        //
        // Solve A*X = B for X using LDL^t decomposition
        //
        //     double A[size * size]
        //     double B[size * count]
        //
        // Modifies A, places the result in B.  Returns false if A is singular.
        //
        static inline bool ldsolve(double* A, int64 size, double* B, int64 count) {
            ldfactor(A, size);
            return lddivide(A, size, B, count);
        }

        static inline matrix<double> ldsolve(matrix<double> A, matrix<double> B) {
            check(A.rows() == A.cols(), "must be a square matrix");
            check(A.rows() == B.rows(), "need compatible sizes");
            check(
                ldsolve(A.data(), A.rows(), B.data(), B.cols()),
                "matrix must not be singular"
            );
            matrix<double> X(A.cols(), B.cols());
            for (int64 ii = 0; ii<X.rows(); ii++) {
                for (int64 jj = 0; jj<X.cols(); jj++) {
                    X(ii, jj) = B(ii, jj);
                }
            }
            return X;
        }

        static inline vector<double> ldsolve(matrix<double> A, vector<double> b) {
            check(A.rows() == A.cols(), "must be a square matrix");
            check(A.rows() == b.size(), "need compatible sizes");
            check(
                ldsolve(A.data(), A.rows(), b.data(), 1),
                "matrix must not be singular"
            );
            vector<double> x(A.cols());
            for (int64 ii = 0; ii<x.size(); ii++) {
                x[ii] = b[ii];
            }
            return x;
        }

        //}}}
    //{{{ SVD 2x2

    template<class type>
    struct svd2x2t {
        type L[2][2];
        double S[2];
        type R[2][2];
    };

    typedef svd2x2t<double> rsvd2x2;
    typedef svd2x2t<cdouble> csvd2x2;

    namespace internal {
        static inline rsvd2x2 diagonalize (double ff, double gg, double hh) {
            // Diagonalize a 2x2 real valued lower bi-diagonal matrix.
            //
            // We can always factor an arbitrary 2x2 real or complex matrix
            // into this special case by using Householder (or Givens)
            // rotations as shown in the functions below.
            //
            // This algorithm is loosely taken from an article titled,
            // "Accuracy of two SVD algorithms for 2x2 triangular matrices"
            // published in Applied Mathematic and Computation in 2009.
            //
            // We want to find two rotation matrices such that:
            //
            //    [ff  0] = [Lc -Ls] [S0 0] [Rc -Rs]
            //    [gg hh]   [Ls  Lc] [0 S1] [Rs  Rc]
            //
            //    Where Lc = cos(L), Ls = sin(L), Rc = cos(R), Rs = sin(R)
            //
            // Transposing to invert and rearranging:
            //
            //    [ Lc Ls] [ff  0] [ Rc Rs] = [S0 0]
            //    [-Ls Lc] [gg hh] [-Rs Rc]   [0 S1]
            //
            // Expanding the left:
            //
            //    [( Lc*ff + Ls*gg) Ls*hh] [ Rc Rs] = [S0 0]
            //    [(-Ls*ff + Lc*gg) Lc*hh] [-Rs Rc]   [0 S1]
            //
            // Expanding and separating:
            //
            //    S0 =  Lc*Rc*ff + Ls*Rc*gg - Ls*Rs*hh
            //     0 =  Lc*Rs*ff + Ls*Rs*gg + Ls*Rc*hh
            //     0 = -Ls*Rc*ff + Lc*Rc*gg - Lc*Rs*hh
            //    S1 = -Ls*Rs*ff + Lc*Rs*gg + Lc*Rc*hh
            //
            ///////////////// First Method //////////////////////////
            //
            // Factoring the two zero equations for left cos and sin:
            //
            //    [Rs*ff  (Rs*gg + Rc*hh)] [Lc] = [0]
            //    [(Rc*gg - Rs*hh) -Rc*ff] [Ls]   [0]
            //
            // Since [Lc Ls] can't be zero, the determinant must be zero:
            //
            //    0 = -Rc*Rs*ff^2 - (Rc*Rs*gg^2 - Rs^2*gg*hh + Rc^2*gg*hh - Rc*Rs*hh^2)
            //      = Rc*Rs*ff^2 + Rc*Rs*gg^2 - Rs^2*gg*hh + Rc^2*gg*hh - Rc*Rs*hh^2
            //      = Rc*Rs*(ff^2 + gg^2 - hh^2) + (Rc^2 - Rs^2)*gg*hh
            //
            // We're going to use the following double-angle formulas:
            //
            //    sin(2x) = 2*cos(x)*sin(x)
            //    cos(2x) = cos(x) - sin(x)
            //
            // In:
            //
            //    0 = 2*Rs*CS*(ff^2 + gg^2 - hh^2) + 2*(Rc^2 - Rs^2)*gg*hh
            //      = sin(2R)*(ff^2 + gg^2 - hh^2) + 2*cos(2R)*gg*hh
            //
            // Rearranging, we get our first method which solves for the right angle:
            //
            //    sin(2R)*(ff^2 + gg^2 - hh^2) = -2*cos(2R)*gg*hh
            //    tan(2R) = (-2*gg*hh) / (ff^2 + gg^2 - hh^2)
            //
            // Now that we have a value for R, we can solve for L in two ways:
            //
            //    0 =  Lc*Rs*ff + Ls*Rs*gg + Ls*Rc*hh
            //    Ls*(Rs*gg + Rc*hh) = -Lc*(Rs*ff)
            //    tan(L) = -(Rs*ff) / (Rs*gg + Rc*hh)
            //
            // Or:
            //
            //    0 = -Ls*Rc*ff + Lc*Rc*gg - Lc*Rs*hh
            //    Ls*(Rc*ff) = Lc*(Rc*gg - Rs*hh)
            //    tan(L) = (Rc*gg - Rs*hh) / (Rc*ff)
            //
            ////////////////// Second Method ////////////////////////
            //
            // We can also factor those two equations for the right cos and sin:
            //
            //    0 =  Lc*Rs*ff + Ls*Rs*gg + Ls*Rc*hh
            //    0 = -Ls*Rc*ff + Lc*Rc*gg - Lc*Rs*hh
            //
            //    [Ls*hh   (Lc*ff + Ls*gg)] [Rc] = [0]
            //    [(-Ls*ff + Lc*gg) -Lc*hh] [Rs]   [0]
            //
            // Again, the determinant must be zero and using the double angle formulas:
            //
            //    0 = -Lc*Ls*hh^2 - (-Lc*Ls*ff^2 + Lc^2*ff*gg - Ls^2*ff*gg + Lc*Ls*gg^2)
            //      = Lc*Ls*hh^2 - Lc*Ls*ff^2 + Lc^2*ff*gg - Ls^2*ff*gg + Lc*Ls*gg^2
            //      = Lc*Ls*(-ff^2 + gg^2 + hh^2) + (Lc^2 - Ls^2)*ff*gg
            //      = sin(2*L)*(-ff^2 + gg^2 + hh^2) + cos(2L)*ff*gg
            //
            // Again rearranging, we get our second method for the left angle:
            //
            //    2*sin(2L)*(-ff^2 + gg^2 + hh^2) = -2*cos(2L)*ff*gg
            //    tan(2L) = (-2*ff*gg) / (-ff^2 + gg^2 + hh^2)
            //
            // This time we have a value for L, so we can solve for R in two ways:
            //
            //    0 =  Lc*Rs*ff + Ls*Rs*gg + Ls*Rc*hh
            //    Rs*(Lc*ff + Ls*gg) = -Rc*(Ls*hh)
            //    tan(R) = -Ls*hh / (Lc*ff + Ls*gg)
            //
            //    0 = -Ls*Rc*ff + Lc*Rc*gg - Lc*Rs*hh
            //    Rs*(Lc*hh) = Rc*(-Ls*ff + Lc*gg)
            //    tan(R) = (Lc*gg - Ls*ff) / Lc*hh
            //
            //////////////////////////////////////////////////////////
            //
            // So we have two methods - one which solves for L and one for R.
            // Each of those two methods has two sub-methods to solve for the
            // missing piece.  We choose which ones to use based on trying to
            // avoid catastrophic cancellation in the operands.
            //

            // Check if it's already diagonal
            if (gg == 0) return (rsvd2x2){
                {{ 1, 0 }, { 0, 1 }}, { ff, hh }, {{ 1, 0 }, { 0, 1 }}
            };

            if (ff == 0) { // shortcut
                double rr = hypot(gg, hh);
                double cc = hh/rr;
                double ss = gg/rr;
                return (rsvd2x2){
                    {{ 1, 0 }, { 0, 1 }}, { 0, rr }, {{ cc, -ss }, { ss, cc }}
                };
            }

            if (hh == 0) { // shortcut
                double rr = hypot(ff, gg);
                double cc = ff/rr;
                double ss = gg/rr;
                return (rsvd2x2){
                    {{ cc, -ss }, { ss, cc }}, { rr, 0 }, {{ 1, 0 }, { 0, 1 }}
                };
            }

            double f2 = ff*ff;
            double g2 = gg*gg;
            double h2 = hh*hh;

            double Lc, Ls, Rc, Rs;
            if (f2 > h2) {
                double R = .5*atan2(-2*gg*hh, f2 + g2 - h2);
                Rc = cos(R);
                Rs = sin(R);
                double L;
                if (fabs(Rs*gg + Rc*hh) > fabs(Rc*gg - Rs*hh)) {
                    L = atan2(-Rs*ff, Rs*gg + Rc*hh);
                } else {
                    L = atan2(Rc*gg - Rs*hh, Rc*ff);
                }
                Lc = cos(L);
                Ls = sin(L);
            } else {
                double L = .5*atan2(-2*ff*gg, h2 + g2 - f2);
                Lc = cos(L);
                Ls = sin(L);
                double R;
                if (fabs(Lc*ff + Ls*gg) > fabs(Lc*gg - Ls*ff)) {
                    R = atan2(-Ls*hh, Lc*ff + Ls*gg);
                } else {
                    R = atan2(Lc*gg - Ls*ff, Lc*hh);
                }
                Rc = cos(R);
                Rs = sin(R);
            }

            double S0 =  Lc*Rc*ff + Ls*Rc*gg - Ls*Rs*hh;
            double S1 = -Ls*Rs*ff + Lc*Rs*gg + Lc*Rc*hh;

            return (rsvd2x2) {
                {{ Lc, -Ls }, { Ls, Lc }}, { S0, S1 }, {{ Rc, -Rs }, { Rs, Rc }}
            };
        }
    }

    /* XXX
    template<class type>
    static inline std::ostream& operator <<(std::ostream& os, const svd2x2t<type>& s) {
        os << std::setprecision(5);
        os << "L: [[" << s.L[0][0] << ", " << s.L[0][1] << "][" << s.L[1][0] << ", " << s.L[1][1] << "]]" << std::endl;
        os << "S: [" << s.S[0] << "; " << s.S[1] << "]" << std::endl;
        os << "R: [[" << s.R[0][0] << ", " << s.R[0][1] << "][" << s.R[1][0] << ", " << s.R[1][1] << "]]" << std::endl;

        return os;
    }
    */

    static inline rsvd2x2 bsvd2x2 (double ff, double gg, double hh) {
        using namespace internal;

        // Calculate the SVD of a 2x2 real lower bidiagonal matrix
        // Permute and negate the bidiagonal result if needed
        rsvd2x2 svd = diagonalize(ff, gg, hh);
        if (svd.S[0] < 0) {
            svd.L[0][0] *= -1;
            svd.L[1][0] *= -1;
            svd.S[0] *= -1;
        }

        if (svd.S[1] < 0) {
            svd.L[0][1] *= -1;
            svd.L[1][1] *= -1;
            svd.S[1] *= -1;
        }

        if (svd.S[0] < svd.S[1]) {
            swap(svd.L[0][0], svd.L[0][1]);
            swap(svd.L[1][0], svd.L[1][1]);
            swap(svd.S[0], svd.S[1]);
            swap(svd.R[0][0], svd.R[1][0]);
            swap(svd.R[0][1], svd.R[1][1]);
        }

        return svd;
    }


    static inline rsvd2x2 svd2x2 (
        double tt, double uu,
        double vv, double ww
    ) {
        if (uu) {
            double ff = hypot(tt, uu);
            double cc = +tt/ff;
            double ss = -uu/ff;
            double gg = vv*cc - ww*ss;
            double hh = vv*ss + ww*cc;
            rsvd2x2 svd = bsvd2x2(ff, gg, hh);
            double r00 = +svd.R[0][0]*cc + svd.R[0][1]*ss;
            double r01 = -svd.R[0][0]*ss + svd.R[0][1]*cc;
            double r10 = +svd.R[1][0]*cc + svd.R[1][1]*ss;
            double r11 = -svd.R[1][0]*ss + svd.R[1][1]*cc;
            svd.R[0][0] = r00; svd.R[0][1] = r01;
            svd.R[1][0] = r10; svd.R[1][1] = r11;
            return svd;
        }
        // it was already bidiagonal
        return bsvd2x2(tt, vv, ww);
    }

    static inline csvd2x2 svd2x2 (
        cdouble tt, cdouble uu,
        cdouble vv, cdouble ww
    ) {
        double magt = hypot(tt.re, tt.im);
        double magu = hypot(uu.re, uu.im);
        double magx = hypot(magt, magu);
        double angt = atan2(tt.im, tt.re);
        double angu = atan2(uu.im, uu.re);
        double angx = atan2(magu, magt);
        double cosx = cos(angx);
        double cost = cos(angt);
        double cosu = cos(angu);
        double sinx = sin(angx);
        double sint = sin(angt);
        double sinu = sin(angu);

        cdouble R00 = +cosx*cdouble(cost, sint);
        cdouble R01 = +sinx*cdouble(cosu, sinu);
        cdouble R10 = -sinx*cdouble(cost, sint);
        cdouble R11 = +cosx*cdouble(cosu, sinu);

        cdouble C10 = vv*conj(R00) + ww*conj(R01);
        cdouble C11 = vv*conj(R10) + ww*conj(R11);

        double angl = atan2(C10.im, C10.re);
        cdouble zl = cdouble(cos(angl), sin(angl));
        cdouble L00 = cdouble(1, 0);
        cdouble L01 = cdouble(0, 0);
        cdouble L10 = cdouble(0, 0);
        cdouble L11 = zl;//cdouble(1, 0);
        C10 = C10*conj(zl);
        C11 = C11*conj(zl);

        double angr = atan2(C11.im, C11.re);
        cdouble zr = cdouble(cos(angr), sin(angr));
        R10 = R10*zr;
        R11 = R11*zr;

        double ff = magx;
        double gg = hypot(C10.re, C10.im);
        double hh = hypot(C11.re, C11.im);
        rsvd2x2 svd= bsvd2x2(ff, gg, hh);

        return (csvd2x2){
            {{svd.L[0][0]*L00 + svd.L[1][0]*L01, svd.L[0][1]*L00 + svd.L[1][1]*L01},
             {
                (svd.L[0][0])*L10 + (svd.L[1][0])*L11,
                (svd.L[0][1])*L10 + (svd.L[1][1])*L11
            }},
            { svd.S[0], svd.S[1] },
            {{
                (svd.R[0][0])*R00 + (svd.R[0][1])*R10,
                (svd.R[0][0])*R01 + (svd.R[0][1])*R11
            },{
                (svd.R[1][0])*R00 + (svd.R[1][1])*R10,
                (svd.R[1][0])*R01 + (svd.R[1][1])*R11
            }}
        };
    }
    
    //}}}
    //{{{ SIMD
 
#define define_simdtype(NAME, BASE, COUNT) \
    typedef BASE NAME __attribute__(       \
        (vector_size (COUNT*sizeof(BASE))) \
    )

    // 64 bit SIMD (MMX)
    define_simdtype( i8x8,    int8_t,  8);
    define_simdtype(i16x4,   int16_t,  4);
    define_simdtype(i32x2,   int32_t,  2);
    define_simdtype( u8x8,   uint8_t,  8);
    define_simdtype(u16x4,  uint16_t,  4);
    define_simdtype(u32x2,  uint32_t,  2);
    define_simdtype(f32x2,     float,  2);
 
    // 128 bit SIMD (SSE)
    define_simdtype(i8x16,    int8_t, 16);
    define_simdtype(i16x8,   int16_t,  8);
    define_simdtype(i32x4,   int32_t,  4);
    define_simdtype(i64x2,   int64_t,  2);
    define_simdtype(u8x16,   uint8_t, 16);
    define_simdtype(u16x8,  uint16_t,  8);
    define_simdtype(u32x4,  uint32_t,  4);
    define_simdtype(u64x2,  uint64_t,  2);
    define_simdtype(f32x4,     float,  4);
    define_simdtype(f64x2,    double,  2);

    // 256 bit SIMD (AVX)
    define_simdtype( i8x32,   int8_t, 32);
    define_simdtype(i16x16,  int16_t, 16);
    define_simdtype( i32x8,  int32_t,  8);
    define_simdtype( i64x4,  int64_t,  4);
    define_simdtype( u8x32,  uint8_t, 32);
    define_simdtype(u16x16, uint16_t, 16);
    define_simdtype( u32x8, uint32_t,  8);
    define_simdtype( u64x4, uint64_t,  4);
    define_simdtype( f32x8,    float,  8);
    define_simdtype( f64x4,   double,  4);

    // 512 bit SIMD (AVX-512)
    define_simdtype( i8x64,   int8_t, 64);
    define_simdtype(i16x32,  int16_t, 32);
    define_simdtype(i32x16,  int32_t, 16);
    define_simdtype( i64x8,  int64_t,  8);
    define_simdtype( u8x64,  uint8_t, 64);
    define_simdtype(u16x32, uint16_t, 32);
    define_simdtype(u32x16, uint32_t, 16);
    define_simdtype( u64x8, uint64_t,  8);
    define_simdtype(f32x16,    float, 16);
    define_simdtype( f64x8,   double,  8);

    // 1024 bit SIMD (Future??)
    define_simdtype(i8x128,   int8_t, 128);
    define_simdtype(i16x64,  int16_t,  64);
    define_simdtype(i32x32,  int32_t,  32);
    define_simdtype(i64x16,  int64_t,  16);
    define_simdtype(u8x128,  uint8_t, 128);
    define_simdtype(u16x64, uint16_t,  64);
    define_simdtype(u32x32, uint32_t,  32);
    define_simdtype(u64x16, uint64_t,  16);
    define_simdtype(f32x32,    float,  32);
    define_simdtype(f64x16,   double,  16);

#undef define_simdtype

    //}}}
    }
    //}}}

////////////////////////////////////////////////

    //{{{ xmheader
    namespace internal {
        // We put all of this stuff in an internal namespace just
        // because it's not typically needed directly.

        static int64 xmsuffix (char suffix) {
            switch (suffix) {
                case 'S': return sizeof(char);
                case 'A': return sizeof(char);
                case 'B': return sizeof(int8_t);
                case 'I': return sizeof(int16_t);
                case 'L': return sizeof(int32_t);
                case 'X': return sizeof(int64_t);
                case 'F': return sizeof(float);
                case 'D': return sizeof(double);
                case 'O': return sizeof(uint8_t);
                case 'U': return sizeof(uint16_t);
                case 'V': return sizeof(uint32_t);
            }
            return 0;
        }

        static int64 xmprefix(char prefix) {
            switch (prefix) {
                case 'S': return  1;
                case 'C': return  2;
                case 'V': return  3;
                case 'Q': return  4;
                case 'M': return  9;
                case 'T': return 16;
                case '1': return  1;
                case '2': return  2;
                case '3': return  3;
                case '4': return  4;
                case '5': return  5;
                case '6': return  6;
                case '7': return  7;
                case '8': return  8;
                case '9': return  9;
                case 'X': return 10;
                case 'A': return 32;
            }
            return 0;
        }

        static int64 xmbytesize(const string& format) {
            //if (memcmp(format, "NH", 2) == 0)  // record files
            //if (memcmp(format, "KW", 2) == 0)  // keyword files
            check(format.size() == 2, "expected two characters '%s'", format.data());
            return xmprefix(format.data()[0]) * xmsuffix(format.data()[1]);
        }

        struct xmheader {
            char     version[4];    // "BLUE"
            char     head_rep[4];   // BigEndian ? "IEEE" : "EEEI"
            char     data_rep[4];   // BigEndian ? "IEEE" : "EEEI"
            int32_t  detached;      // 1 for detached, 0 otherwise
            char     ignore0_[8];
            uint32_t ext_start;     // Offset to extended header /512
            uint32_t ext_size;      // Size of extended header in bytes
            double   data_start;    // Offset to data in bytes
            double   data_size;     // Size of data in bytes
            int32_t  type;          // 1000, 2000, 3000, ...
            char     format[2];     // "SI", "CF", ...
            char     ignore1_[2];
            double   timecode;      // J1950 accurate to microseconds
            char     ignore2_[96];
            int32_t  keylength;     // keyword length in bytes */
            char     keywords[92];  // "KEY1=val1\0KEY2=val2\0"

            double  xstart;        // seconds since the timecode
            double  xdelta;        // delta increments in seconds
            int32_t xunits;
            int32_t subsize;       // frame size for Type 2000 files or
                                   // field count for Type 3000 files
            double  ystart;        // seconds since the timecode
            double  ydelta;        // delta increments in seconds
            int32_t yunits;        // 'xm help units' for more info
            int32_t recsize;       // record size for Type 3000 files

            union {
                struct {
                    /// Record descriptors for Type 3000 files
                    struct {
                        char name[4];
                        char format[2];
                        int16_t offset;
                    } fields[26];
                } std;

                struct {
                    // Record descriptor for Type 5000 statevector files
                    struct {
                        char name[4];
                        char format[2];
                        int8_t type;
                        int8_t units;
                    } fields[14];
                    char frame[8];
                    double quads[11];
                } eph;
            } u;
        };

        static void byteswap8 (void* ptr) {
            char* dst = (char*)ptr;
            char src[8];
            memcpy(src, ptr, 8);
            dst[0] = src[7];
            dst[1] = src[6];
            dst[2] = src[5];
            dst[3] = src[4];
            dst[4] = src[3];
            dst[5] = src[2];
            dst[6] = src[1];
            dst[7] = src[0];
        }

        static void byteswap4(void* ptr) {
            char* dst = (char*)ptr;
            char src[4];
            memcpy(src, ptr, 4);
            dst[0] = src[3];
            dst[1] = src[2];
            dst[2] = src[1];
            dst[3] = src[0];
        }

        static void byteswap2(void* ptr) {
            char* dst = (char*)ptr;
            char src[2];
            memcpy(src, ptr, 2);
            dst[0] = src[1];
            dst[1] = src[0];
        }

#if   __BYTE_ORDER == __LITTLE_ENDIAN
            static const char* xmnative = "EEEI";
#elif __BYTE_ORDER == __BIG_ENDIAN
            static const char* xmnative = "IEEE";
#else
#  error "can't determine the endian"
#endif

        // returns true if the header had to be byteswapped - this means
        // the extended header keywords will have to be byteswapped too.
        static bool fix_header(xmheader* blue) {
            check(memcmp(blue->version, "BLUE", 4) == 0, "expected 'BLUE' header");
            check(
                memcmp(blue->head_rep, "EEEI", 4) == 0 || memcmp(blue->head_rep, "IEEE", 4) == 0,
                "expected 'EEEI' or 'IEEE' for header representation"
            );
            check(
                memcmp(blue->data_rep, "EEEI", 4) == 0 || memcmp(blue->data_rep, "IEEE", 4) == 0,
                "expected 'EEEI' or 'IEEE' for data representation"
            );

            bool byteswap = memcmp(blue->head_rep, xmnative, 4) != 0;
            if (byteswap) {
                // only swap the fields we use
                byteswap4(&blue->ext_start);
                byteswap4(&blue->ext_size);
                byteswap8(&blue->data_start);
                byteswap8(&blue->data_size);
                byteswap4(&blue->type);
                byteswap8(&blue->timecode);
                byteswap4(&blue->keylength);
                // these are common to all BLUE types
                byteswap8(&blue->xstart);
                byteswap8(&blue->xdelta);
                byteswap4(&blue->xunits);
                byteswap4(&blue->subsize);
                byteswap8(&blue->ystart);
                byteswap8(&blue->ydelta);
                byteswap4(&blue->yunits);
                byteswap4(&blue->recsize);

                int64 ii;
                switch (blue->type/1000) {
                    case 1: break; // nothing needed
                    case 2: break; // nothing needed
                    case 3: // fix the fields
                        check(blue->subsize <= 26, "at most 26 fields for Type 3000 (got %d)", blue->subsize);
                        for (ii = 0; ii<blue->subsize; ii++) {
                            byteswap2(&blue->u.std.fields[ii].offset);
                        }
                        break;
                    case 5: // check field count
                        check(blue->subsize <= 14, "at most 14 fields for Type 5000 (got %d)", blue->subsize);
                        for (ii = 0; ii<11; ii++) {
                            byteswap8(&blue->u.eph.quads[ii]);
                        }
                        break;
                    default:
                        check(false, "type %d files are not currently supported", blue->type);
                        break;
                }
            }

            return byteswap;
        }

        /*
        static inline bluefile_t* bluefile_defaults (void) {
            bluefile_t* blue = (bluefile_t*)xalloc(sizeof(bluefile_t));
            memset(blue, 0, sizeof(bluefile_t));
            memcpy(blue->version, "BLUE", 4);
            const char* native = bluefile_native_();
            memcpy(blue->head_rep, native, 4);
            memcpy(blue->data_rep, native, 4);
            blue->type = 1000;
            memcpy(blue->format, "CF", 2);
            blue->xdelta = 1.0;
            blue->xunits = 1;
            blue->ydelta = 1.0;
            blue->yunits = 1;
            return blue;
        }
        */

        static double xmtcprec(const xmheader* blue) {
            const char* ptr = blue->keywords;
            int64 len = blue->keylength;
            for (;;) {
                if (len <= 0 || *ptr == 0) break;
                double tcprec = 0;
                if (sscanf(ptr, " TC_PREC = %lf", &tcprec) == 1) {
                    return tcprec;
                }
                ptr += ::strlen(ptr) + 1;
                len -= ::strlen(ptr) + 1;
            }
            return 0;
        }

        static timecode xmgettime(const xmheader* blue) {
            double tcode = blue->timecode;
            double tcprec = xmtcprec(blue);
            double whole, fract = modf(tcode, &whole);
            fract = round(fract*1e6)*1e-6 + tcprec;
            return normalize((timecode){ (int64)whole, fract });
        }

        static void xmsettime(xmheader* blue, timecode time) {
            double whole, fract = modf(time.fract*1e6, &whole);
            blue->timecode = time.whole + whole*1e-6;
            sprintf(blue->keywords, "TC_PREC=%.6le", fract*1e-6);
            blue->keylength = ::strlen(blue->keywords) + 1;
        }

        static int64 xmxcount(const xmheader* blue) {
            switch (blue->type/1000) {
                case 1: return (int64)floor(
                    blue->data_size / xmbytesize(blue->format)
                );
                case 2: return blue->subsize;
                case 3: // fall through
                case 5: return (int64)floor(
                    blue->data_size / blue->recsize
                );
            }

            return 0;
        }

        static int64 xmycount(const xmheader* blue) {
            if (blue->type/1000 == 2) {
                return (int64)floor(
                    blue->data_size / (
                        blue->subsize * xmbytesize(blue->format)
                    )
                );
            }

            return 1;
        }

        static string terminated(const char* ptr, int64 len) {
            vector<char> temp(len + 1);
            memcpy(temp.data(), ptr, len);
            temp[len] = 0;
            return string(temp.data());
        }

        // Field names are fixed length, but a space or a null means
        // that the string is shorter than the total length.
        static string fortranstr(const char* ptr, int64 len) {
            vector<char> temp(len + 1);
            memcpy(temp.data(), ptr, len);
            temp[len] = 0;
            for (int64 ii = 0; ii<len; ii++) {
                if (temp[ii] == ' ') {
                    temp[ii] = 0;
                    break;
                }
            }
            return string(temp.data());
        }

    }
    //}}}
    //{{{ bluekeyword

    //
    // Keywords only support a single character represenation.  For example,
    // there are no 'CD' (complex double) keywords, just 'D' (double).  All
    // keywords are stored as an array of the type, but frequently there is
    // only one item in the array.  The following codes are understood by this
    // library:
    //
    //  'A' - ascii string
    //  'S' - UTF-8 string
    //
    //  'B' - int8_t
    //  'I' - int16_t
    //  'L' - int32_t
    //  'X' - int64_t
    //
    //  'O' - uint8_t
    //  'U' - uint16_t
    //  'V' - uint32_t
    //
    //  'F' - float
    //  'D' - double
    //
    // 'A', 'L', and 'D' are the most common
    //

    struct bluekeyword {
        string name;
        char code;
        vector<char> bytes;

        inline string       getstr() const;
        inline vector<double>  getflt() const;
        inline vector<int64>   getint() const;
    };

    namespace internal {
        template<class type>
        string intkwdtostr(const bluekeyword& kwd) {
            int64 len = kwd.bytes.size() / sizeof(type);
            int64 show = min(len, (int64)3);
            list<string> out;
            for (int64 ii = 0; ii<show; ++ii) {
                int64_t value = *(type*)&kwd.bytes[ii*sizeof(type)];
                out.append(format("%ld", value));
                if (ii != show - 1) {
                    out.append(",");
                }
            }
            if (len > show) {
                out.append(", ...");
            }
            return join(out);
        }

        template<class type>
        string unskwdtostr(const bluekeyword& kwd) {
            int64 len = kwd.bytes.size() / sizeof(type);
            int64 show = min(len, (int64)3);
            list<string> out;
            for (int64 ii = 0; ii<show; ++ii) {
                uint64_t value = *(type*)&kwd.bytes[ii*sizeof(type)];
                out.append(format("%lu", value));
                if (ii != show - 1) {
                    out.append(",");
                }
            }
            if (len > show) {
                out.append(", ...");
            }
            return join(out);
        }

        template<class type>
        string fltkwdtostr(const bluekeyword& kwd) {
            int64 len = kwd.bytes.size() / sizeof(type);
            int64 show = min(len, (int64)3);
            list<string> out;
            for (int64 ii = 0; ii<show; ++ii) {
                double value = *(type*)&kwd.bytes[ii*sizeof(type)];
                out.append(format("%.18lf", value));
                if (ii != show - 1) {
                    out.append(",");
                }
            }
            if (len > show) {
                out.append(", ...");
            }
            return join(out);
        }

        template<class type>
        vector<double> kwdtoflt(const bluekeyword& kwd) {
            int64 len = kwd.bytes.size() / sizeof(type);
            vector<double> result(len);

            for (int64 ii = 0; ii<len; ++ii) {
                type value = *(type*)&kwd.bytes[ii*sizeof(type)];
                result[ii] = value;
            }
            return result;
        }

        template<class type>
        vector<int64> intkwdtoint(const bluekeyword& kwd) {
            int64 len = kwd.bytes.size() / sizeof(type);
            vector<int64_t> result(len);

            for (int64 ii = 0; ii<len; ++ii) {
                type value = *(type*)&kwd.bytes[ii*sizeof(type)];
                result[ii] = value;
            }
            return result;
        }

        template<class type>
        vector<int64> fltkwdtoint(const bluekeyword& kwd) {
            int64 len = kwd.bytes.size() / sizeof(type);
            vector<int64_t> result(len);

            for (int64 ii = 0; ii<len; ++ii) {
                type value = *(type*)&kwd.bytes[ii*sizeof(type)];
                result[ii] = llrint(value);
            }
            return result;
        }
    }

    string bluekeyword::getstr() const {
        using namespace internal;
        switch (code) {
            case 'S': case 's':
            case 'A': case 'a': {
                int64 len = bytes.size();
                return terminated(bytes.data(), len);
            }
            case 'B': case 'b': return intkwdtostr<int8_t>(*this);
            case 'I': case 'i': return intkwdtostr<int16_t>(*this);
            case 'L': case 'l': return intkwdtostr<int32_t>(*this);
            case 'X': case 'x': return intkwdtostr<int64_t>(*this);
            case 'O': case 'o': return unskwdtostr<uint8_t>(*this);
            case 'U': case 'u': return unskwdtostr<uint16_t>(*this);
            case 'V': case 'v': return unskwdtostr<uint32_t>(*this);
            case 'F': case 'f': return fltkwdtostr<float>(*this);
            case 'D': case 'd': return fltkwdtostr<double>(*this);
        }
        // XXX: Is this the right thing to do?
        return string();
    }

    vector<double> bluekeyword::getflt() const {
        using namespace internal;
        switch (code) {
            case 'S': case 's':
            case 'A': case 'a': {
                string str = getstr();
                char* end = 0;
                double val = ::strtod(str.data(), &end);
                check(end != 0 && (end == str.data() + str.size()), "convert '%s' to double", str.data());
                vector<double> result(1);
                result[0] = val;
                return result;
            }
            case 'B': case 'b': return kwdtoflt<int8_t>(*this);
            case 'I': case 'i': return kwdtoflt<int16_t>(*this);
            case 'L': case 'l': return kwdtoflt<int32_t>(*this);
            case 'X': case 'x': return kwdtoflt<int64_t>(*this);
            case 'O': case 'o': return kwdtoflt<uint8_t>(*this);
            case 'U': case 'u': return kwdtoflt<uint16_t>(*this);
            case 'V': case 'v': return kwdtoflt<uint32_t>(*this);
            case 'F': case 'f': return kwdtoflt<float>(*this);
            case 'D': case 'd': return kwdtoflt<double>(*this);
        }
        return vector<double>();
    }

    vector<int64> bluekeyword::getint() const {
        using namespace internal;
        switch (code) {
            case 'S': case 's':
            case 'A': case 'a': {
                string str = getstr();
                char* end = 0;
                int64 val = ::strtoll(str.data(), &end, 0);
                if (end != str.data() + str.size()) {
                    double flt = strtod(str.data(), &end);
                    check(end != 0 && end == str.data() + str.size(), "convert '%s' to double", str.data());
                    val = llrint(flt);
                }
                vector<int64_t> result(1);
                result[0] = val;
                return result;
            }
            case 'B': case 'b': return intkwdtoint<int8_t>(*this);
            case 'I': case 'i': return intkwdtoint<int16_t>(*this);
            case 'L': case 'l': return intkwdtoint<int32_t>(*this);
            case 'X': case 'x': return intkwdtoint<int64_t>(*this);
            case 'O': case 'o': return intkwdtoint<uint8_t>(*this);
            case 'U': case 'u': return intkwdtoint<uint16_t>(*this);
            case 'V': case 'v': return intkwdtoint<uint32_t>(*this);
            case 'F': case 'f': return fltkwdtoint<float>(*this);
            case 'D': case 'd': return fltkwdtoint<double>(*this);
        }
        return vector<int64>();
    }
    //}}}
    //{{{ bluekeywords

    struct bluekeywords {
        list<bluekeyword> storage;

        inline int64 count(const string& name) const;

        inline bluekeyword  getkwd(const string& name, int64 which=0) const;
        inline string       getstr(const string& name, int64 which=0) const;
        inline vector<double>  getflt(const string& name, int64 which=0) const;
        inline vector<int64>   getint(const string& name, int64 which=0) const;

        inline void update(const string& name, char code, const void* data, int64 len, int64 which);
        inline void update(const string& name, const string&   value,  int64 which=0);
        inline void update(const string& name, double       value,  int64 which=0);
        inline void update(const string& name, vector<double>  values, int64 which=0);
        inline void update(const string& name, int32_t      value,  int64 which=0);
        inline void update(const string& name, vector<int32_t> values, int64 which=0);
        inline void update(const string& name, int64_t      value,  int64 which=0);
        inline void update(const string& name, vector<int64_t> values, int64 which=0);

        inline void insert(const string& name, char code, const void* data, int64 len, int64 where);
        inline void insert(const string& name, const string&   value,  int64 where=-1);
        inline void insert(const string& name, double       value,  int64 where=-1);
        inline void insert(const string& name, vector<double>  values, int64 where=-1);
        inline void insert(const string& name, int32_t      value,  int64 where=-1);
        inline void insert(const string& name, vector<int32_t> values, int64 where=-1);
        inline void insert(const string& name, int64_t      value,  int64 where=-1);
        inline void insert(const string& name, vector<int64_t> values, int64 where=-1);

        inline void remove(const string& name, int64 which=0);

    };

    int64 bluekeywords::count(const string& name) const {
        int64 total = 0;
        for (int64 ii = 0; ii<storage.size(); ii++) {
            if (storage[ii].name == name) total++;
        }
        return total;
    }

    namespace internal {
        // these are some helper routines for the get functions below

        struct xmkeyword {
            uint32_t next_offset;
            uint16_t non_value;
            uint8_t  key_length;
            char     format_code;
            char     buffer[1]; // variable length
        };

        static int64 write_kwds(rawfile* pfile, const bluekeywords& kwds) {
            int64 total_bytes = 0;

            for (int64 ii = 0; ii<kwds.storage.size(); ii++) {
                const bluekeyword& kwd = kwds.storage[ii];
                int64 val_length = kwd.bytes.size();
                int64 key_length = kwd.name.size();
                check(key_length <= 255, "keyword name less than 255 chars (%lld)", key_length);
                int64 data_length = 8 + val_length + key_length;
                int64 padding = (data_length%8) ? 8 - data_length%8 : 0;
                total_bytes += data_length + padding;

                if (pfile) {
                    xmkeyword tmp = xmkeyword();
                    tmp.next_offset = data_length + padding;
                    tmp.non_value = data_length + padding - val_length;
                    tmp.key_length = key_length;
                    tmp.format_code = kwd.code;
                    check(pfile->write(&tmp, 8), "writing keyword descriptor");
                    check(pfile->write(kwd.bytes.data(), val_length), "writing keyword value");
                    check(pfile->write(kwd.name.data(), key_length), "writing keyword name");
                    uint64_t scratch = 0;
                    check(pfile->write(&scratch, padding), "writing keyword padding");
                }
            }

            return total_bytes;
        }

        static bluekeywords readkwds(rawfile& file, int64 len, bool byteswap) {
            vector<char> block(len);
            check(file.read(block.data(), len), "reading keywords");

            bluekeywords result;
            char* ptr = block.data();
            char* end = ptr + len;

            //print("\t\t", "keywords", byteswap ? "are" : "not", "byte swapped");
            while (ptr < end) {
                xmkeyword* kwd = (xmkeyword*)ptr;
                if (byteswap) byteswap4(&kwd->next_offset);
                if (byteswap) byteswap2(&kwd->non_value);
                int64 data_len = kwd->next_offset - kwd->non_value;
                string name = terminated(kwd->buffer + data_len, kwd->key_length);
                vector<char> bytes(data_len);
                memcpy(bytes.data(), kwd->buffer, data_len);
                result.storage.append((bluekeyword){ name, kwd->format_code, bytes});
                ptr += kwd->next_offset;
            }

            if (byteswap) {
                for (int64 ii = 0; ii<result.storage.size(); ii++) {
                    bluekeyword& kwd = result.storage[ii];
                    switch (kwd.code) {
                        case 'I':
                        case 'U': {
                            int64 count = kwd.bytes.size()/2;
                            for (int64 jj = 0; jj<count; jj++) {
                                byteswap2(&kwd.bytes[jj*2]);
                            }
                        } break;

                        case 'L':
                        case 'V':
                        case 'F': {
                            int64 count = kwd.bytes.size()/4;
                            for (int64 jj = 0; jj<count; jj++) {
                                byteswap4(&kwd.bytes[jj*4]);
                            }
                        } break;

                        case 'X':
                        case 'D': {
                            int64 count = kwd.bytes.size()/8;
                            for (int64 jj = 0; jj<count; jj++) {
                                byteswap8(&kwd.bytes[jj*8]);
                            }
                        } break;
                    }
                }
            }

            return result;
        }
    }

    bluekeyword bluekeywords::getkwd(const string& name, int64 which) const {
        check(which >= 0, "keyword index must be non-negative (%lld)", which);
        for (int64 ii = 0; ii<storage.size(); ii++) {
            const bluekeyword& kwd = storage[ii];
            if (kwd.name == name) {
                if (which == 0) {
                    return kwd;
                }
                --which;
            }
        }
        check(false, "requested non-existent keyword '%s'", name.data());
        return (bluekeyword){ "", 0, vector<char>() };
    }

    string bluekeywords::getstr(const string& name, int64 which) const {
        return getkwd(name, which).getstr();
    }

    vector<int64> bluekeywords::getint(const string& name, int64 which) const {
        return getkwd(name, which).getint();
    }

    vector<double> bluekeywords::getflt(const string& name, int64 which) const {
        return getkwd(name, which).getflt();
    }

    void bluekeywords::update(const string& name, char code, const void* data, int64 len,  int64 which) {
        check(which >= 0, "must specify non-negative keyword index (%lld)", which);
        for (int64 ii = 0; ii<storage.size(); ii++) {
            bluekeyword& kwd = storage[ii];
            if (kwd.name == name) {
                if (which == 0) {
                    vector<char> bytes(len);
                    memcpy(&bytes[0], data, len);
                    kwd.code = code;
                    swap(kwd.bytes, bytes);
                    return;
                } else {
                    --which;
                }
            }
        }
        check(false, "keyword to update not found '%s' (%lld)", name.data(), which);
    }

    void bluekeywords::update(const string& name, const string& value, int64 which) {
        update(name, 'A', value.data(), value.size(), which);
    }

    void bluekeywords::update(const string& name, double value, int64 which) {
        update(name, 'D', &value, sizeof(double), which);
    }

    void bluekeywords::update(const string& name, vector<double> values, int64 which) {
        update(name, 'D', values.data(), values.size()*sizeof(double), which);
    }

    void bluekeywords::update(const string& name, int32_t value, int64 which) {
        update(name, 'L', &value, sizeof(int32_t), which);
    }

    void bluekeywords::update(const string& name, vector<int32_t> values, int64 which) {
        update(name, 'L', values.data(), values.size()*sizeof(int32_t), which);
    }

    void bluekeywords::update(const string& name, int64_t value, int64 which) {
        update(name, 'X', &value, sizeof(int64), which);
    }

    void bluekeywords::update(const string& name, vector<int64_t> values, int64 which) {
        update(name, 'X', values.data(), values.size()*sizeof(int64), which);
    }

    void bluekeywords::insert(const string& name, char code, const void* data, int64 len, int64 where) {
        if (where < 0) where = storage.size();
        check(where <= storage.size(), "index in bounds %lld [0, %lld]", where, storage.size());
        vector<char> bytes(len);
        memcpy(&bytes[0], data, len);
        storage.insert(where, (bluekeyword){ name, code, bytes });
    }

    void bluekeywords::insert(const string& name, const string& value, int64 where) {
        insert(name, 'A', value.data(), value.size(), where);
    }

    void bluekeywords::insert(const string& name, double value,  int64 where) {
        insert(name, 'D', &value, sizeof(double), where);
    }

    void bluekeywords::insert(const string& name, vector<double>  values, int64 where) {
        insert(name, 'D', values.data(), values.size()*sizeof(double), where);
    }

    void bluekeywords::insert(const string& name, int64_t value, int64 where) {
        insert(name, 'X', &value, sizeof(int64), where);
    }

    void bluekeywords::insert(const string& name, vector<int64_t> values, int64 where) {
        insert(name, 'X', values.data(), values.size()*sizeof(int64), where);
    }

    void bluekeywords::insert(const string& name, int32_t value, int64 where) {
        insert(name, 'L', &value, sizeof(int32_t), where);
    }

    void bluekeywords::insert(const string& name, vector<int32_t> values, int64 where) {
        insert(name, 'L', values.data(), values.size()*sizeof(int32_t), where);
    }

    void bluekeywords::remove(const string& name, int64 which) {
        check(which >= 0, "must specify non-negative keyword index (%lld)", which);
        for (int64 ii = 0; ii<storage.size(); ii++) {
            if (storage[ii].name == name) {
                storage.remove(ii);
                return;
            }
        }
        check(false, "keyword to remove not found '%s' (%lld)", name.data(), which);
    }

    //}}}
    //{{{ bluefield
    struct bluefield {
        string name;
        string format;
        int16_t offset;
    };
    //}}}
    //{{{ bluemeta

    namespace blueunits {
        enum {
            none    =  0, // unitless
            time    =  1, // seconds
            delay   =  2, // seconds
            freq    =  3, // Hertz
            pos     =  5, // meters
            vel     =  6, // meters/sec
            acc     =  7, // meters/s^2
            jerk    =  8, // meters/s^3
            doppler =  9, // Hertz
            chirp   = 10, // Hertz/sec
            radians = 23, // radians
            degrees = 24, // degrees
            cycles  = 25, // cycles
            decibel = 35, // unitless
            lat     = 60, // degrees
            lon     = 61, // degrees
            alt     = 63, // meters
        };
    }

    namespace internal {
        static inline string xmunits (uint8_t code) {
            switch (code) {
                case blueunits::none:    return "none (unitless)";
                case blueunits::time:    return "time (seconds)";
                case blueunits::delay:   return "delay (seconds)";
                case blueunits::freq:    return "freq (Hertz)";
                case blueunits::pos:     return "pos (meters)";
                case blueunits::vel:     return "vel (meters/sec)";
                case blueunits::acc:     return "acc (meters/s^2)";
                case blueunits::jerk:    return "jerk (meters/s^3)";
                case blueunits::doppler: return "doppler (Hertz)";
                case blueunits::chirp:   return "chirp (Hertz/sec)";
                case blueunits::radians: return "angle (radians)";
                case blueunits::degrees: return "angle (degrees)";
                case blueunits::cycles:  return "angle (cycles)";
                case blueunits::decibel: return "dB (unitless)";
                case blueunits::lat:     return "lat (degrees)";
                case blueunits::lon:     return "lon (degrees)";
                case blueunits::alt:     return "alt (meters)";
                default: break;
            }
            return "other unknown";
        }
    }

    struct bluemeta {
        int32_t type;
        timecode time;
        string format;
        int32_t itemsize;
        int64   xcount, ycount;
        double  xstart, ystart;
        double  xdelta, ydelta;
        int32_t xunits, yunits;
        bluekeywords kwds;
        list<bluefield> fields;
    };

    namespace internal {
        static inline xmheader init_header(const bluemeta& meta) {
            check(sizeof(xmheader) == 512, "sanity");
            // need to write out the header, keywords, and padding
            xmheader hdr = xmheader();
            memcpy(hdr.version, "BLUE", 4);
            memcpy(hdr.head_rep, xmnative, 4);
            memcpy(hdr.data_rep, xmnative, 4);
            xmsettime(&hdr, meta.time);

            hdr.type = meta.type;
            check(meta.format.size() == 2, "expect digraph");
            memcpy(hdr.format, meta.format.data(), 2);

            int64 keyword_bytes = write_kwds(0, meta.kwds);
            //int64 padding_bytes = keyword_bytes%512 ? 512 - keyword_bytes%512 : 0;
            if (keyword_bytes) {
                hdr.ext_start = 1; // times 512
                hdr.ext_size = keyword_bytes;
            }

            hdr.data_start = 512 + keyword_bytes;// + padding_bytes;

            switch (hdr.type/1000) {
                case 1: {
                    int64 itemsize = xmbytesize(meta.format);
                    check(itemsize > 0, "supported sample format '%s'", meta.format.data());
                    check(meta.xcount >= 0, "non-negative xcount (%lld)", meta.xcount);
                    hdr.data_size = meta.xcount * itemsize;
                    hdr.xstart = meta.xstart;
                    hdr.xdelta = meta.xdelta;
                    hdr.xunits = meta.xunits;
                } break;

                case 2: {
                    int64 itemsize = xmbytesize(meta.format);
                    check(itemsize > 0, "supported sample format '%s'", meta.format.data());
                    check(meta.xcount >= 0, "non-negative xcount (%lld)", meta.xcount);
                    check(meta.ycount >= 0, "non-negative ycount (%lld)", meta.ycount);
                    hdr.data_size = meta.ycount * meta.xcount * itemsize;
                    hdr.xstart = meta.xstart;
                    hdr.xdelta = meta.xdelta;
                    hdr.xunits = meta.xunits;
                    hdr.subsize = meta.xcount;
                    hdr.ystart = meta.ystart;
                    hdr.ydelta = meta.ydelta;
                    hdr.yunits = meta.yunits;
                } break;

                case 3: {
                    int64 itemsize = meta.itemsize;
                    check(itemsize > 0, "need positive itemsize (%lld)", itemsize);
                    check(meta.xcount >= 0, "non-negative xcount (%lld)", meta.xcount);
                    check(meta.fields.size() > 0, "need some fields");
                    hdr.data_size = meta.xcount * itemsize;
                    hdr.xstart = meta.xstart;
                    hdr.xdelta = meta.xdelta;
                    hdr.xunits = meta.xunits;
                    hdr.subsize = meta.fields.size();
                    hdr.recsize = itemsize;
                    check(meta.fields.size() <= 26, "can't have more than 26 fields");
                    for (int64 ii = 0; ii<meta.fields.size(); ii++) {
                        const bluefield& field = meta.fields[ii];
                        check(field.name.size() <= 4, "max of 4 character field name '%s'", field.name.data());
                        memset(hdr.u.std.fields[ii].name, ' ', 4);
                        memcpy(hdr.u.std.fields[ii].name, field.name.data(), field.name.size());
                        check(field.format.size() == 2, "need digraph for field type '%s'", field.format.data());
                        memcpy(hdr.u.std.fields[ii].format, field.format.data(), 2);
                        hdr.u.std.fields[ii].offset = field.offset;
                    }

                } break;

                case 5: {
                    int64 itemsize = meta.itemsize;
                    check(itemsize > 0, "need positive itemsize (%lld)", itemsize);
                    check(meta.xcount >= 0, "non-negative xcount (%lld)", meta.xcount);
                    check(meta.fields.size() > 0, "need some fields");
                    hdr.data_size = meta.xcount * itemsize;
                    hdr.xstart = meta.xstart;
                    hdr.xdelta = meta.xdelta;
                    hdr.xunits = meta.xunits;
                    hdr.subsize = meta.fields.size();
                    hdr.recsize = itemsize;
                    check(meta.fields.size() <= 14, "can't have more than 14 fields");
                    int64 offset = 0;
                    for (int64 ii = 0; ii<meta.fields.size(); ii++) {
                        const bluefield& field = meta.fields[ii];
                        check(field.name.size() <= 4, "max of 4 character field name '%s'", field.name.data());
                        memset(hdr.u.eph.fields[ii].name, ' ', 4);
                        memcpy(hdr.u.eph.fields[ii].name, field.name.data(), field.name.size());
                        check(field.format.size() == 2, "need digraph for field type '%s'", field.format.data());
                        memcpy(hdr.u.eph.fields[ii].format, field.format.data(), 2);
                        check(offset == field.offset, "data offset (%lld) == field offset (%d)", offset, field.offset);
                        int64 bytes = xmbytesize(field.format.data());
                        check(bytes > 0, "need supported data format '%s'", field.format.data());
                        if (memcmp(field.name.data(), "POS", 3) == 0) {
                            hdr.u.eph.fields[ii].type  = 2;
                            hdr.u.eph.fields[ii].units = 5;
                        }
                        if (memcmp(field.name.data(), "VEL", 3) == 0) {
                            hdr.u.eph.fields[ii].type  = 2;
                            hdr.u.eph.fields[ii].units = 6;
                        }
                        if (memcmp(field.name.data(), "ACC", 3) == 0) {
                            hdr.u.eph.fields[ii].type  = 2;
                            hdr.u.eph.fields[ii].units = 7;
                        }
                        if (memcmp(field.name.data(), "TIME", 3) == 0) {
                            hdr.u.eph.fields[ii].type  = 1;
                            hdr.u.eph.fields[ii].units = 1;
                        }
                        offset += bytes;
                    }
                    check(itemsize == offset, "total itemsize (%lld) == sum of records (%lld)", itemsize, offset);
                    memcpy(hdr.u.eph.frame, "ECR     ", 8); // for building ephemeris
                } break;

                default:
                    check(false, "Type %d files are not supported", hdr.type);
            }

            return hdr;
        }

    }

    //}}}
    //{{{ bluereader

    struct bluereader {
        //inline ~bluereader() = default;
        //inline bluereader() = default;
        //inline bluereader(const bluereader&) = default;
        inline bluereader(const string& path);

        //inline bluereader& operator =(const bluereader&) = default;

        inline bool kwds_ready() const;
        inline bool is_swapped() const;

        inline const bluemeta* operator ->() const;
        inline const bluemeta& operator *() const;

        // this is the low level grab function, it works on bytes
        inline void grab(int64 offset, void* buffer, int64 length);

        // this grab function will do conversion and byteswapping
        inline void grabcf(int64 offset, cfloat* data, int64 length);

        inline const void* mmap();

        inline int64 byte_offset(int64 sample);

        private:
            struct implementation {
                internal::rawfile file;
                bluemeta meta;
                int64 data_offset;
                int64 data_length;
                int64 error_offset;
                int64 cache_offset;
                int64 cache_length;
                list<vector<char> > cache;
                vector<char> scratch;
                bool is_swapped;
                bool kwds_ready;
            };
            shared<implementation> pimpl;
    };

    bluereader::bluereader(const string& path) {
        using namespace internal;
        check(sizeof(xmheader) == 512, "sanity");

        // open the file or duplicate stdin
        int fd = path == "-" ? dup(0) : open(path.data(), O_RDONLY);
        check(fd >= 0, "opening '%s' for reading", path.data());
        rawfile file(fd);

        // read the header
        xmheader hdr;
        check(file.read(&hdr, sizeof(xmheader)), "reading X-Midas header");
        bool byteswap_kwds = fix_header(&hdr);

        int64 kwds_offset = hdr.ext_start * 512LL;
        int64 kwds_length = hdr.ext_size;
        int64 data_offset = llrint(hdr.data_start);
        int64 data_length = llrint(hdr.data_size);

        bluekeywords keywords;
        bool kwds_ready = false;
        if (hdr.detached) {
            if (kwds_length > 0) {
                check(file.skip(kwds_offset - 512), "skipping to keywords");
                keywords = readkwds(file, kwds_length, byteswap_kwds);
            }
            kwds_ready = true;

            // open the detached data portion of the file
            int64 pos = rfind(path, ".");
            check(pos >= 0, "need file extension: %s", path.data());
            string detpath = substr(path, 0, pos) + ".det";
            int detfd = open(detpath.data(), O_RDONLY);
            check(detfd >= 0, "opening '%s' for reading", detpath.data());
            rawfile detfile(detfd);
            swap(file, detfile);
            check(file.seek(data_offset), "seeking to detached data start");

        } else {
            if (kwds_length > 0) {
                if (kwds_offset < data_offset) {
                    // it's in pipe order, we can skip to the keywords
                    check(file.skip(kwds_offset - 512), "skipping to keywords");
                    keywords = readkwds(file, kwds_length, byteswap_kwds);
                    // now skip to the data
                    check(
                        file.skip(data_offset - (kwds_offset + kwds_length)),
                        "skipping to data"
                    );
                    kwds_ready = true;
                } else if (file.isfile()) {
                    // it's not pipe order, but  we can seek to the
                    // keywords and seek back to the data afterwards
                    check(file.seek(kwds_offset), "seeking to keywords");
                    keywords = readkwds(file, kwds_length, byteswap_kwds);
                    check(file.seek(data_offset), "seeking to data start");
                    kwds_ready = true;
                } else {
                    // the keywords come after the data, and it's in a pipe
                    // all we can do is skip to the data and ignore keywords
                    check(file.skip(data_offset - 512), "skipping to data offset");
                }
            } else {
                // no keywords, so we just skip to the offset
                check(file.skip(data_offset - 512), "skipping to data offset");
                kwds_ready = true;
            }
        }

        shared<implementation> tmp(new implementation());
        swap(pimpl, tmp);
        swap(pimpl->file, file);
        pimpl->meta.type     = hdr.type;
        pimpl->meta.time     = xmgettime(&hdr);
        pimpl->meta.format   = terminated(hdr.format, 2);
        pimpl->meta.kwds     = keywords;
        pimpl->data_offset   = data_offset;
        pimpl->data_length   = data_length;
        pimpl->error_offset  = INT64_MIN;
        pimpl->cache_offset  = 0;
        pimpl->cache_length  = 0;
        pimpl->is_swapped    = memcmp(hdr.data_rep, xmnative, 4) != 0;
        pimpl->kwds_ready    = kwds_ready;

        switch (hdr.type/1000) {
            case 1:
                pimpl->meta.itemsize = xmbytesize(hdr.format);
                check(pimpl->meta.itemsize != 0, "recognized sample type");
                pimpl->meta.xcount   = xmxcount(&hdr);
                pimpl->meta.xstart   = hdr.xstart;
                pimpl->meta.xdelta   = hdr.xdelta;
                pimpl->meta.xunits   = hdr.xunits;
                pimpl->meta.ycount   = 1;
                break;

            case 2:
                pimpl->meta.itemsize = xmbytesize(hdr.format);
                check(pimpl->meta.itemsize != 0, "recognized sample type");
                pimpl->meta.xcount   = xmxcount(&hdr);
                pimpl->meta.xstart   = hdr.xstart;
                pimpl->meta.xdelta   = hdr.xdelta;
                pimpl->meta.xunits   = hdr.xunits;
                pimpl->meta.ycount   = xmycount(&hdr);
                pimpl->meta.ystart   = hdr.ystart;
                pimpl->meta.ydelta   = hdr.ydelta;
                pimpl->meta.yunits   = hdr.yunits;
                break;

            case 3:
                pimpl->meta.itemsize = hdr.recsize;
                check(pimpl->meta.itemsize > 0, "positive record size");
                pimpl->meta.xcount   = llrint(hdr.data_size)/hdr.recsize;
                pimpl->meta.xstart   = hdr.xstart;
                pimpl->meta.xdelta   = hdr.xdelta;
                pimpl->meta.xunits   = hdr.xunits;
                pimpl->meta.ycount   = 1;

                for (int64 ii = 0; ii<hdr.subsize; ii++) {
                    pimpl->meta.fields.append(
                        (bluefield){
                            fortranstr(hdr.u.std.fields[ii].name, 4),
                            terminated(hdr.u.std.fields[ii].format, 2),
                            hdr.u.std.fields[ii].offset
                        }
                    );
                }
                break;

            case 5:
                pimpl->meta.itemsize = hdr.recsize;
                check(pimpl->meta.itemsize > 0, "positive record size");
                pimpl->meta.xcount   = llrint(hdr.data_size)/hdr.recsize;
                pimpl->meta.xstart   = hdr.xstart;
                pimpl->meta.xdelta   = hdr.xdelta;
                pimpl->meta.xunits   = hdr.xunits;
                pimpl->meta.ycount   = 1;
                // This doesn't really belong here, but we assume that all Type
                // 5000 files are ephemeris in the ECR (ECF) coordinate system.
                // It'll be good to catch errors if we ever find one that isn't.
                check(memcmp("ECR", hdr.u.eph.frame, 3) == 0, "ECR coordinates");

                /*
                for (int64 ii = 0; ii<3; ii++) {
                    printf("type: %d, units: %d\n", hdr.u.eph.fields[ii].type, hdr.u.eph.fields[ii].units);
                }

                printf("frame: ");
                for (int64 ii = 0; ii<8; ii++) {
                    printf("'%c' ", hdr.u.eph.frame[ii]);
                }
                printf("\n");
                for (int64 ii = 0; ii<11; ii++) {
                    printf("quads[%lld] == %lf\n", ii, hdr.u.eph.quads[ii]);
                }
                */

                for (int64 offset = 0, ii = 0; ii<hdr.subsize; ii++) {
                    string format = terminated(hdr.u.eph.fields[ii].format, 2);
                    int64 bytes = xmbytesize(format);
                    pimpl->meta.fields.append(
                        (bluefield){
                            fortranstr(hdr.u.eph.fields[ii].name, 4),
                            format, (int16_t)offset
                        }
                    );
                    check(bytes > 0, "need supported format '%s'", format.data());
                    offset += bytes;
                }
                break;

            default:
                check(false, "Type %d files are not supported", hdr.type);
        }
    }

    bool bluereader::kwds_ready() const {
        check(pimpl.borrow() != 0, "need an opened file");
        return pimpl->kwds_ready;
    }

    bool bluereader::is_swapped() const {
        check(pimpl.borrow() != 0, "need an opened file");
        return pimpl->is_swapped;
    }

    const bluemeta* bluereader::operator ->() const {
        check(pimpl.borrow() != 0, "need an opened file");
        return &pimpl->meta;
    }

    const bluemeta& bluereader::operator *() const {
        check(pimpl.borrow() != 0, "need an opened file");
        return pimpl->meta;
    }

    void bluereader::grab(int64 offset, void* buffer, int64 length) {
        char* pointer = (char*)buffer;
        check(pimpl.borrow() != 0, "need an opened file");
        // first we check that they don't go backwards
        check(
            offset >= pimpl->error_offset,
            "not allowed to read backward in file %lld < %lld",
            offset, pimpl->error_offset
        );
        pimpl->error_offset = offset;
        if (length == 0) return;

        // zeros before the start of the file
        if (offset < 0) {
            int64 amount = min(length, -offset);
            memset(pointer, 0, amount);
            offset  += amount;
            pointer += amount;
            length  -= amount;
            if (length == 0) return;
        }

        // discard blocks before our request
        while (pimpl->cache.size() != 0) {
            vector<char>& block = pimpl->cache[0];
            if (offset < pimpl->cache_offset + (int64)block.size()) break;
            pimpl->cache_offset += block.size();
            pimpl->cache_length -= block.size();
            pimpl->cache.remove(0);
        }

        // skip to the start if it's after our cache
        if (offset > pimpl->cache_offset + pimpl->cache_length) {
            check(pimpl->cache.size() == 0, "sanity");
            check(pimpl->cache_length == 0, "sanity");
            check(pimpl->cache_offset <= pimpl->data_length, "sanity");
            int64 amount = min(
                offset - pimpl->cache_offset,
                pimpl->data_length - pimpl->cache_offset
            );
            check(pimpl->file.skip(amount), "skip %lld", amount);
            pimpl->cache_offset += amount;
        }

        // read one new block to satisfy our request
        int64 cache_ending = pimpl->cache_offset + pimpl->cache_length;
        if (offset + length > cache_ending && cache_ending < pimpl->data_length) {
            const int64 PAGE_SIZE = 65536;
            int64 needed = (offset + length) - cache_ending;
            int64 extra = needed % PAGE_SIZE;
            int64 wanted = needed + (extra ? PAGE_SIZE - extra : 0);
            int64 remaining = pimpl->data_length - cache_ending;
            int64 amount = min(wanted, remaining);
            vector<char> block(amount);
            check(pimpl->file.read(block.data(), amount), "read %lld", amount);
            pimpl->cache.append(vector<char>());
            swap(pimpl->cache[pimpl->cache.size() - 1], block);
            pimpl->cache_length += amount;
            // XXX: Here is where we could check to see if we should read
            // keywords that come at the end of a pipe, but it isn't really
            // needed in practice.
        }

        // copy from the cache to the output
        int64_t block_offset = pimpl->cache_offset;
        for (int64 ii = 0; ii<pimpl->cache.size(); ii++) {
            vector<char>& block = pimpl->cache[ii];
            int64 start = offset - block_offset;
            int64 amount = min(length, (int64)block.size() - start);
            memcpy(pointer, block.data() + start, amount);
            offset  += amount;
            pointer += amount;
            length  -= amount;
            if (length == 0) return;
            block_offset += block.size();
        }

        // zero fill the rest
        memset(pointer, 0, length);
    }

    void bluereader::grabcf(int64 offset, cfloat* samples, int64 length) {
        using namespace internal;
        check(pimpl.borrow() != 0, "need an opened file");
        const int64 sample_size = pimpl->meta.itemsize;

        // this one reads directly into the samples vector
        if (pimpl->meta.format == "CF") {
            grab(offset*sample_size, (char*)samples, length*sample_size);

            if (pimpl->is_swapped) {
                float* elems = (float*)samples;
                for (int64 ii = 0; ii<2*length; ii++) {
                    byteswap4(elems + ii);
                }
            }
            return;
        }

        // the rest perform type conversions and use the scratch buffer
        pimpl->scratch.resize(length * sample_size);
        grab(offset*sample_size, (char*)pimpl->scratch.data(), length*sample_size);

        if (pimpl->meta.format == "SB") {
            int8_t* ptr = (int8_t*)pimpl->scratch.data();
            for (int64 ii = 0; ii<length; ii++) {
                samples[ii] = cfloat(ptr[ii], 0);
            }

        } else if (pimpl->meta.format == "SI") {
            int16_t* ptr = (int16_t*)pimpl->scratch.data();
            if (pimpl->is_swapped) {
                for (int64 ii = 0; ii<length; ii++) {
                    byteswap2(ptr + ii);
                }
            }
            for (int64 ii = 0; ii<length; ii++) {
                samples[ii] = cfloat(ptr[ii], 0);
            }

        } else if (pimpl->meta.format == "SL") {
            int32_t* ptr = (int32_t*)pimpl->scratch.data();
            if (pimpl->is_swapped) {
                for (int64 ii = 0; ii<length; ii++) {
                    byteswap4(ptr + ii);
                }
            }
            for (int64 ii = 0; ii<length; ii++) {
                samples[ii] = cfloat(ptr[ii], 0);
            }

        } else if (pimpl->meta.format == "SF") {
            float* ptr = (float*)pimpl->scratch.data();
            if (pimpl->is_swapped) {
                for (int64 ii = 0; ii<length; ii++) {
                    byteswap4(ptr + ii);
                }
            }
            for (int64 ii = 0; ii<length; ii++) {
                samples[ii] = cfloat(ptr[ii], 0);
            }

        } else if (pimpl->meta.format == "SD") {
            double* ptr = (double*)pimpl->scratch.data();
            if (pimpl->is_swapped) {
                for (int64 ii = 0; ii<length; ii++) {
                    byteswap8(ptr + ii);
                }
            }
            for (int64 ii = 0; ii<length; ii++) {
                samples[ii] = cfloat(ptr[ii], 0);
            }

        } else if (pimpl->meta.format == "CB") {
            int8_t* ptr = (int8_t*)pimpl->scratch.data();
            for (int64 ii = 0; ii<length; ii++) {
                samples[ii] = cfloat(ptr[2*ii + 0], ptr[2*ii + 1]);
            }

        } else if (pimpl->meta.format == "CI") {
            int16_t* ptr = (int16_t*)pimpl->scratch.data();
            if (pimpl->is_swapped) {
                for (int64 ii = 0; ii<2*length; ii++) {
                    byteswap2(ptr + ii);
                }
            }
            for (int64 ii = 0; ii<length; ii++) {
                samples[ii] = cfloat(ptr[2*ii + 0], ptr[2*ii + 1]);
            }

        } else if (pimpl->meta.format == "CL") {
            int32_t* ptr = (int32_t*)pimpl->scratch.data();
            if (pimpl->is_swapped) {
                for (int64 ii = 0; ii<2*length; ii++) {
                    byteswap4(ptr + ii);
                }
            }
            for (int64 ii = 0; ii<length; ii++) {
                samples[ii] = cfloat(ptr[2*ii + 0], ptr[2*ii + 1]);
            }

        } else if (pimpl->meta.format == "CD") {
            double* ptr = (double*)pimpl->scratch.data();
            if (pimpl->is_swapped) {
                for (int64 ii = 0; ii<2*length; ii++) {
                    byteswap8(ptr + ii);
                }
            }
            for (int64 ii = 0; ii<length; ii++) {
                samples[ii] = cfloat(ptr[2*ii + 0], ptr[2*ii + 1]);
            }

        } else {
            check(false, "unsuported conversion '%s' to 'CF'", pimpl->meta.format.data());
        }
    }

    const void* bluereader::mmap() {
        check(pimpl.borrow() != 0, "need an opened file");
        return pimpl->data_offset + (char*)pimpl->file.mmap();
    }

    inline int64 bluereader::byte_offset(int64 sample) {
        check(pimpl.borrow() != 0, "need an opened file");
        return pimpl->data_offset + sample*pimpl->meta.itemsize;
    }

    //}}}
    //{{{ bluewriter
    struct bluewriter {
        inline ~bluewriter();
        //inline bluewriter() = default;
        //inline bluewriter(const bluewriter&) = default;
        inline bluewriter(const string& path);

        //inline bluewriter& operator =(const bluewriter&) = default;

        inline bluemeta* operator ->();
        inline const bluemeta* operator ->() const;
        inline bluemeta& operator *();
        inline const bluemeta& operator *() const;

        inline void write(const void* ptr, int64 len);

        private:
            inline void setup();

            struct implementation {
                internal::rawfile file;
                bluemeta meta;
                int64 bytes_written;
                int64 total_length;
            };
            shared<implementation> pimpl;
    };

    bluewriter::~bluewriter() {
        if (pimpl.borrow() != 0) check(
            pimpl->bytes_written == pimpl->total_length,
            "need to write the total number of bytes (%lld/%lld)",
            pimpl->bytes_written, pimpl->total_length
        );
    }

    bluewriter::bluewriter(const string& path) {
        using namespace internal;

        int fd = path == "-" ? dup(1) : open(
            path.data(), O_WRONLY | O_TRUNC | O_CREAT, 0666
        );
        check(fd >= 0, "opening '%s' for writing", path.data());
        rawfile file(fd);

        shared<implementation> tmp(new implementation());
        swap(pimpl, tmp);
        swap(pimpl->file, file);
        pimpl->meta.type     = 1000;
        pimpl->meta.format   = "CF";
        pimpl->meta.xcount   = 0;
        pimpl->meta.ycount   = 1;
        pimpl->meta.xstart   = 0.0;
        pimpl->meta.ystart   = 0.0;
        pimpl->meta.xdelta   = 1.0;
        pimpl->meta.ydelta   = 1.0;
        pimpl->meta.xunits   = 0;
        pimpl->meta.yunits   = 0;
        // sentinel to indicate that we need
        // to write the header and keywords
        pimpl->bytes_written = -1;
    }

    bluemeta* bluewriter::operator ->() {
        check(pimpl.borrow() != 0, "need an opened file");
        return &pimpl->meta;
    }

    const bluemeta* bluewriter::operator ->() const {
        check(pimpl.borrow() != 0, "need an opened file");
        return &pimpl->meta;
    }

    bluemeta& bluewriter::operator *() {
        check(pimpl.borrow() != 0, "need an opened file");
        return pimpl->meta;
    }

    const bluemeta& bluewriter::operator *() const {
        check(pimpl.borrow() != 0, "need an opened file");
        return pimpl->meta;
    }

    void bluewriter::setup() {
        using namespace internal;
        xmheader hdr = init_header(pimpl->meta);

        check(pimpl->file.write(&hdr, sizeof(xmheader)), "writing header");
        write_kwds(&pimpl->file, pimpl->meta.kwds);
        /*
        if (padding_bytes) {
            static char scratch[512];
            check(pimpl->file.write(scratch, padding_bytes), "writing padding");
        }
        */

        pimpl->bytes_written = 0;
        pimpl->total_length = (uint64_t)hdr.data_size;
    }

    void bluewriter::write(const void* ptr, int64 len) {
        check(pimpl.borrow() != 0, "need an opened file");

        if (pimpl->bytes_written == -1) setup();

        check(
            pimpl->bytes_written + len <= pimpl->total_length,
            "can't write %lld bytes of data %lld/%lld", len,
            pimpl->bytes_written, pimpl->total_length
        );
        check(pimpl->file.write(ptr, len), "writing data");
        pimpl->bytes_written += len;
    }

    //}}}
    //{{{ dump2000 dump1000
    static inline void dump1000(
        const string& path, const string& format, const void* ptr, timecode time,
        double xstart, double xdelta, int xunits, int64 xcount
    ) {
        using namespace internal;
        int64 bytesize = xmbytesize(format);
        bluewriter output(path);
        output->time = time;
        output->type = 1000;
        output->format = format;
        output->xstart = xstart;
        output->xdelta = xdelta;
        output->xunits = xunits;
        output->xcount = xcount;
        output.write(ptr, xcount*bytesize);
    }

    static inline void dump2000(
        const string& path, const string& format, const void* ptr, timecode time,
        double xstart, double xdelta, int xunits, int64 xcount,
        double ystart, double ydelta, int yunits, int64 ycount
    ) {
        using namespace internal;
        int64 bytesize = xmbytesize(format);
        bluewriter output(path);
        output->time = time;
        output->type = 2000;
        output->format = format;
        output->xstart = xstart;
        output->xdelta = xdelta;
        output->xunits = xunits;
        output->xcount = xcount;
        output->ystart = ystart;
        output->ydelta = ydelta;
        output->yunits = yunits;
        output->ycount = ycount;
        output.write(ptr, xcount*ycount*bytesize);
    }

    //}}}
    //{{{ bluedetach
    static inline void bluedetach(
        const string& datapath, int64 byteoffset,
        const string& bluepath, const bluemeta& meta
    ) {
        string detfile = bluepath + ".det";
        string tmpfile = bluepath + ".tmp";

        using namespace internal;
        xmheader hdr = init_header(meta);
        hdr.detached = 1;
        hdr.data_start = byteoffset;

        int fd = open(tmpfile.data(), O_WRONLY | O_TRUNC | O_CREAT, 0660);
        check(fd >= 0, "opening '%s' for writing", tmpfile.data());
        rawfile file(fd);
        check(file.write(&hdr, sizeof(xmheader)), "writing header");
        write_kwds(&file, meta.kwds);

        unlink(detfile.data()); // delete if it exists
        check(symlink(datapath.data(), detfile.data()) == 0, "symlink '%s'", detfile.data());
    }
    //}}}
    //{{{ ephemeris

    struct ephemeris {

        //~ephemeris() = default;
        ephemeris() {}; // = default;
        //ephemeris(const ephemeris&) = default;
        inline ephemeris(const statevec&);
        inline ephemeris(const geodetic&);
        inline ephemeris(const string& path);
        //ephemeris& operator =(const ephemeris&) = default;

        inline statevec lookup(timecode tc) const;

        // timestates must be sorted
        vector<timestate> storage;
    };

    ephemeris::ephemeris(const statevec& sv) {
        storage.resize(1);
        storage[0] = (timestate){ timecode(), sv };
    }

    ephemeris::ephemeris(const geodetic& lla) {
        storage.resize(1);
        storage[0] = (timestate){
            timecode(), (statevec){
                lla_to_ecf(lla), cartesian(), cartesian()
            }
        };
    }

    ephemeris::ephemeris(const string& path) {
        using namespace internal;
        bluereader input(path);
        check(input->type/1000 == 5, "expected a Type 5000 file");

        //print(format(input->time, 12), input->xstart, input->xdelta);

        if (input->itemsize == 72) {
            check(input->fields.size() == 3, "expected POS VEL ACC");
            check(input->fields[0].format == "VD", "expected VD POS");
            check(input->fields[1].format == "VD", "expected VD VEL");
            check(input->fields[2].format == "VD", "expected VD ACC");
            check(memcmp(input->fields[0].name.data(), "POS", 3) == 0, "expected POS");
            check(memcmp(input->fields[1].name.data(), "VEL", 3) == 0, "expected VEL");
            check(memcmp(input->fields[2].name.data(), "ACC", 3) == 0, "expected ACC");
            check(input->xdelta > 0, "positive xdelta");

            storage.resize(input->xcount);
            for (int64 ii = 0; ii<input->xcount; ii++) {
                statevec sv;
                input.grab(ii*sizeof(statevec), &sv, sizeof(statevec));
                if (input.is_swapped()) {
                    byteswap8(&sv.pos.x); byteswap8(&sv.pos.y); byteswap8(&sv.pos.z);
                    byteswap8(&sv.vel.x); byteswap8(&sv.vel.y); byteswap8(&sv.vel.z);
                    byteswap8(&sv.acc.x); byteswap8(&sv.acc.y); byteswap8(&sv.acc.z);
                }
                timecode tc = input->time + (input->xstart + ii*input->xdelta);
                storage[ii] = (timestate){ tc, sv };
            }

            check(storage.size() >= 2, "need at least two statevectors");

            return;
        }

        if (input->itemsize == 80) {
            check(input->fields.size() == 4, "expected POS VEL ACC TIME");
            check(input->fields[0].format == "VD", "expected VD POS");
            check(input->fields[1].format == "VD", "expected VD VEL");
            check(input->fields[2].format == "VD", "expected VD ACC");
            check(input->fields[3].format == "SD", "expected SD TIME");
            check(memcmp(input->fields[0].name.data(), "POS", 3) == 0, "expected POS");
            check(memcmp(input->fields[1].name.data(), "VEL", 3) == 0, "expected VEL");
            check(memcmp(input->fields[2].name.data(), "ACC", 3) == 0, "expected ACC");
            check(input->fields[3].name == "TIME", "expected TIME");

            storage.resize(input->xcount);
            for (int64 ii = 0; ii<input->xcount; ii++) {
                struct record { statevec sv; double tc; } rec;
                input.grab(ii*sizeof(record), &rec, sizeof(record));
                if (input.is_swapped()) {
                    byteswap8(&rec.sv.pos.x); byteswap8(&rec.sv.pos.y); byteswap8(&rec.sv.pos.z);
                    byteswap8(&rec.sv.vel.x); byteswap8(&rec.sv.vel.y); byteswap8(&rec.sv.vel.z);
                    byteswap8(&rec.sv.acc.x); byteswap8(&rec.sv.acc.y); byteswap8(&rec.sv.acc.z);
                    byteswap8(&rec.tc);
                }
                // The double precision time field only has about microsecond precision,
                // so we subtract the file time, round it, and then re-add the file time.
                double dt = 1e-6*round(1e+6*(normalize((timecode){ 0, rec.tc }) - input->time));
                storage[ii] = (timestate){ input->time + dt, rec.sv };
                /*
                if (ii%99 == 0) print(format(input->time + dt));
                if (ii > 0 && storage[ii - 0].tc - storage[ii - 1].tc > 1.1*input->xdelta) {
                   print(">>>");
                   print("===", format(storage[ii-1].tc));
                   print("===", format(storage[ii-0].tc));
                   print("<<<");

                }
                */
            }

            check(storage.size() >= 2, "need at least two statevectors");
            // they should be sorted, but this promises they will be
            introsort(storage.data(), storage.size());

            return;
        }

        check(false, "unsupported ephemeris format");
    }

    statevec ephemeris::lookup(timecode tc) const {
        check(storage.size() > 0, "must use an initialized ephemeris");
        if (storage.size() == 1) return storage[0].sv;

        check(!(tc - storage[0].tc < 0), "timecode not before ephemeris");
        check(!(storage[storage.size() - 1].tc - tc < 0), "timecode not after epehemeris");

        // using secant search to find the correct interval
        int64 lo = 0;
        int64 hi = storage.size() - 1;

        int64 iteration = 0;
        while (lo < hi - 1) {
            // three out of four iterations use secant search and the
            // others use standard bisection to get log(n) worst case
            int64 mid = ++iteration%4 ? (
                lo + llrint(
                    (hi - lo)*(tc - storage[lo].tc) /
                    (storage[hi].tc - storage[lo].tc)
                )
            ) : (lo + hi)/2;
            if (mid <= lo) mid = lo + 1;
            if (mid >= hi) mid = hi - 1;

            if (storage[mid].tc < tc) { lo = mid; continue; }
            if (tc < storage[mid].tc) { hi = mid; continue; }
            return storage[mid].sv; // exact match
        }
        //check(!(tc < storage[lo].tc), "sanity");
        //check(!(storage[hi].tc < tc), "sanity");
        //print("final:", lo, hi, "iterations:", iteration);

        return interp(storage[lo], storage[hi], tc);
        //return storage[lo].sv; // XXX: debugging
    }

    //}}}
    //{{{ ephcache
    namespace internal {
        struct vehday {
            int64 veh;
            int64 day;
            bool operator ==(const vehday& other) const {
                return veh == other.veh && day == other.day;
            }
        };

        static inline size_t hash(const vehday& md) {
            return xm::hash(md.veh) ^ 5*xm::hash(md.day);
        }
    }

    static inline string ephpath(int64 veh, datetime dt) {
        const char* ephdir = getenv("EPHEMERIS_DIR");
        check(ephdir != 0, "EPHEMERIS_DIR must be set");

        list<string> paths;
        // We put the "u" versions first because they are supposed to be "updated" (newer and better).
        paths.append(format("%s/sv%d_%04d%02d%02du.prm", ephdir, (int)veh, dt.year, dt.month, dt.day));
        paths.append(format("%s/sv%d_%04d%02d%02du.tmp", ephdir, (int)veh, dt.year, dt.month, dt.day));
        paths.append(format("%s/sv%d_%04d%02d%02d.prm", ephdir, (int)veh, dt.year, dt.month, dt.day));
        paths.append(format("%s/sv%d_%04d%02d%02d.tmp", ephdir, (int)veh, dt.year, dt.month, dt.day));
        // These are alternative paths - they make more sense, but are not as common, so we put them last
        paths.append(format("%s/%04d/%02d/sv%d_%04d%02d%02du.prm", ephdir, dt.year, dt.month, (int)veh, dt.year, dt.month, dt.day));
        paths.append(format("%s/%04d/%02d/sv%d_%04d%02d%02du.tmp", ephdir, dt.year, dt.month, (int)veh, dt.year, dt.month, dt.day));
        paths.append(format("%s/%04d/%02d/sv%d_%04d%02d%02d.prm", ephdir, dt.year, dt.month, (int)veh, dt.year, dt.month, dt.day));
        paths.append(format("%s/%04d/%02d/sv%d_%04d%02d%02d.tmp", ephdir, dt.year, dt.month, (int)veh, dt.year, dt.month, dt.day));

        for (int64 ii = 0; ii<paths.size(); ii++) {
            const string& path = paths[ii];
            struct stat buf;
            if (stat(path.data(), &buf) == 0) return path;
        }

        check(false, "unable to find ephemeris for %d on %04d-%02d-%02d", (int)veh, dt.year, dt.month, dt.day);
        return "";
    }



    struct ephcache {

        //inline ~ephcache() = default;
        //inline ephcache(const ephcache&) = default;
        //inline ephcache& operator =(const ephcache&) = default;

        inline ephcache(int64 maxsize=64) : maxsize(maxsize), counter(0) {};

        inline ephemeris& geteph(int64 veh, timecode tc);
        inline statevec getsv(int64 veh, timecode tc);

        private:

            int64 maxsize;
            int64 counter;
            struct counteph {
                int64 count;
                ephemeris eph;
            };

            dict<internal::vehday, counteph> cache;
    };

    ephemeris& ephcache::geteph(int64 veh, timecode tc) {
        using namespace internal;
        tc = normalize(tc);
        vehday md = (vehday){ veh, tc.whole/86400 };
        if (!cache.haskey(md)) {
            if ((int64)cache.size() >= maxsize) {
                // the oldest one will have the lowest count
                vehday oldest = (vehday){-1,-1};
                int64 lowest = INT64_MAX;
                for (int64 ii = 0; ii<cache.bins(); ii++) {
                    if (cache.skip(ii)) continue;

                    if (cache.val(ii).count < lowest) {
                        oldest = cache.key(ii);
                        lowest = cache.val(ii).count;
                    }
                }
                cache.remove(oldest);
            }
            datetime dt = getdate(tc);

            cache[md] = (counteph){ counter, ephemeris(ephpath(veh, dt)) };
        }

        counteph& ce = cache[md];
        ce.count = ++counter;
        return ce.eph;
    }

    statevec ephcache::getsv(int64 veh, timecode tc) {
        return geteph(veh, tc).lookup(tc);
    }

    //}}}
    //{{{ lighttime
    namespace internal {

        struct dopdopdot {
            double doppler;
            double doppdot;
        };

        static const double SPEED_OF_LIGHT = 299792458.0;
        static const double INVERSE_SPEED_OF_LIGHT = 3.3356409519815204e-09;

        /*
        static dopdopdot doppler_derivs(statevec src, statevec dst) {
            cartesian range = dst.pos - src.pos;
            cartesian rrate = dst.vel - src.vel;
            double mag = len(range);
            cartesian unit = range * INVERSE_SPEED_OF_LIGHT / mag;

            double vdotr = dot(rrate, range);
            cartesian t0 = range * vdotr / mag;
            cartesian t1 = mag * rrate;

            double scale = 1.0 / (mag * mag * SPEED_OF_LIGHT);
            cartesian udot = scale * (t1 - t0);

            double p0 = dot(src.acc, unit) + dot(src.vel, udot);
            double p1 = 1.0 - dot(dst.vel, unit);
            double p2 = dot(dst.acc, unit) + dot(dst.vel, udot);
            double p3 = 1.0 - dot(src.vel, unit);

            return dopdopdot { p1/p3, (p0*p1 - p2*p3) / (p3*p3) };
        }
        */

        static dopdopdot doppler_derivs(statevec src, statevec dst) {
            cartesian diffp = dst.pos - src.pos;
            cartesian diffv = dst.vel - src.vel;
            double magp = len(diffp);
            cartesian unitp = diffp / magp;
            cartesian usubp = unitp * INVERSE_SPEED_OF_LIGHT;
            double dstdop = (1.0 - dot(dst.vel, usubp));
            double srcdop = (1.0 - dot(src.vel, usubp));

            cartesian usubpdot = (
                diffv - unitp*dot(diffv, unitp)
            ) / (magp * SPEED_OF_LIGHT);

            /*
            printf("XM first: %.18le, second: %.18le\n",
                (dot(src.acc, usubp) + dot(src.vel, usubpdot))*dstdop,
                (dot(dst.acc, usubp) + dot(dst.vel, usubpdot))*srcdop
            );
            */
            double doppdot = (
                (dot(src.acc, usubp) + dot(src.vel, usubpdot))*dstdop -
                (dot(dst.acc, usubp) + dot(dst.vel, usubpdot))*srcdop
            ) / (srcdop * srcdop);

            return (dopdopdot){ dstdop / srcdop, doppdot };
        }

        static inline statevec lighttime_rotate(statevec src, double seconds) {
            const double eav = 7292115.8553e-11;   // WGS-84 Earth Angular Velocity at J2000
            const double rot = seconds * eav;

            // Rotate by Greenwich Hour Angle (gha radians) at
            // Earth Angular Velocity (eav radians per second)
            const double eavt = eav + eav;
            const double eav2 = eav * eav;

            double ss = sin(rot);
            double cc = cos(rot);

            statevec dst;

            double px = cc*src.pos.x - ss*src.pos.y;
            double py = ss*src.pos.x + cc*src.pos.y;
            dst.pos.x = px;
            dst.pos.y = py;
            dst.pos.z = src.pos.z;

            double vx = cc*src.vel.x - ss*src.vel.y;
            double vy = ss*src.vel.x + cc*src.vel.y;
            dst.vel.x = vx - eav*py;
            dst.vel.y = vy + eav*px;
            dst.vel.z = src.vel.z;

            double ax = cc*src.acc.x - ss*src.acc.y - eavt*vy - eav2*px;
            double ay = ss*src.acc.x + cc*src.acc.y + eavt*vx - eav2*py;
            dst.acc.x = ax;
            dst.acc.y = ay;
            dst.acc.z = src.acc.z;

            return dst;
        }
    }

    struct timefreq {
        timecode time;
        double freq;
        double chirp;
    };

    static inline timefreq lighttime_forward(
        timefreq tf, const ephemeris& src, const ephemeris& dst
    ) {
        using namespace internal;
        statevec src_ecf = src.lookup(tf.time);
        statevec src_eci = lighttime_rotate(src_ecf, 0);
        statevec dst_eci = statevec();
        //print("src ecf:", std::setprecision(18), src_ecf);

        double delay = 0;
        for (int ii = 0; ii<5; ii++) {
            statevec dst_ecf = dst.lookup(tf.time + delay);
            //print("dst ecf:", std::setprecision(18), dst_ecf);
            dst_eci = lighttime_rotate(dst_ecf, +delay);
            double dist = len(dst_eci.pos - src_eci.pos);
            delay = dist * INVERSE_SPEED_OF_LIGHT;
        }

        dopdopdot ddd = doppler_derivs(src_eci, dst_eci);
        //printf("xm fwd: doppler=%.18lf, doppler dot=%.18le\n", ddd.doppler, ddd.doppdot);
        //printf("xm fwd: freq: %.18lf, chirp: %.18lf\n", tf.freq, tf.chirp);

        return (timefreq){
            tf.time + delay,
            tf.freq*ddd.doppler,
            tf.chirp*ddd.doppler + tf.freq*ddd.doppdot
        };
    }

    static inline timefreq lighttime_reverse(
        const ephemeris& src, const ephemeris& dst, timefreq tf
    ) {
        using namespace internal;
        statevec dst_ecf = dst.lookup(tf.time);
        //print("dst ecf:", std::setprecision(18), dst_ecf);
        statevec dst_eci = lighttime_rotate(dst_ecf, 0);
        statevec src_eci = statevec();

        double delay = 0;
        for (int ii = 0; ii<5; ii++) {
            statevec src_ecf = src.lookup(tf.time - delay);
            //print("src ecf:", std::setprecision(18), src_ecf);
            src_eci = lighttime_rotate(src_ecf, -delay);
            double dist = len(dst_eci.pos - src_eci.pos);
            delay = dist * INVERSE_SPEED_OF_LIGHT;
        }

        dopdopdot ddd = doppler_derivs(src_eci, dst_eci);
        double invdopp = 1.0 / ddd.doppler;
        //double src_freq = tf.freq / ddd.doppler;
        //printf("xm rev: doppler=%.18lf, doppler dot=%.18le\n", ddd.doppler, ddd.doppdot);
        //printf("xm rev: freq: %.18lf, chirp: %.18lf\n", tf.freq, tf.chirp);

        return (timefreq){
            tf.time - delay,
            tf.freq * invdopp,
            invdopp * (tf.chirp - invdopp * tf.freq * ddd.doppdot)
            //src_freq,
            //tf.chirp - src_freq*ddd.doppdot/ddd.doppler
        };
    }

    //}}}

}

#if 0
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
#endif

#endif
