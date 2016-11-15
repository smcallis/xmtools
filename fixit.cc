
//{{{ includes

#include <new>
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
    namespace internal { struct is_not_arithmetic {}; }
    template<class t0, class t1> struct arithmetic { typedef internal::is_not_arithmetic type; };

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
    namespace internal { struct is_not_conditional {}; }
    template<class t0, class t1> struct conditional { typedef internal::is_not_conditional type; };

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

    template<class atype, class btype>
    typename conditional<atype, btype>::type max(
        const atype& aa, const btype& bb
    ) { 
        return bb < aa ? aa : bb;
    }

    static inline double stopwatch() {
        struct timespec ts = { 0, 0 };
        clock_gettime(CLOCK_MONOTONIC, &ts);
        return ts.tv_sec + 1e-9*ts.tv_nsec;
    }
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
            template<class anyt, int64 anys> friend class vector;

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
            data[ii] = other.storage[ii];
        }
        return *this;
    }

    template<class type, int64 ss>
    template<class tt>
    vector<type, ss>& vector<type, ss>::operator =(const vector<tt, -1>& other) {
        check(other.size() == ss, "matching size");
        for (int64 ii = 0; ii<ss; ii++) {
            data[ii] = other[ii];
        }
        return *this;
    }

    template<class type, int64 ss>
    vector<type, ss>& vector<type, ss>::operator =(const vector<type, ss>& other) {
        for (int64 ii = 0; ii<ss; ii++) {
            data[ii] = other.storage[ii];
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
            template<class anyt, int64 anys> friend class vector;

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
            template<class anyt, int64 anyr, int64 anyc> friend class matrix;

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
            data[ii] = other.storage[ii];
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
            data[ii] = other[ii];
        }
        return *this;
    }

    template<class type, int64 rr, int64 cc>
    matrix<type, rr, cc>& matrix<type, rr, cc>::operator =(
        const matrix<type, rr, cc>& other
    ) {
        for (int64 ii = 0; ii<rr*cc; ii++) {
            data[ii] = other.storage[ii];
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
            template<class anyt, int64 anyr, int64 anyc> friend class matrix;

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

    //{{{ Scalar, Vector, Matrix Ops:    operator + - * / % etc 

    //{{{ prefix - 
    //{{{ - complex 
    template<class type>
    complex<type> operator -(const complex<type>& zz) {
        return complex<type>(-zz.re, -zz.im);
    }
    //}}}
    //{{{ - vector 
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
    //{{{ - matrix 
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
    //}}}
    //{{{ prefix + 
    //{{{ + complex 
    template<class type>
    complex<type> operator +(const complex<type>& zz) {
        return zz;
    }
    //}}}
    //{{{ + vector 
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
    //{{{ + matrix
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
    //{{{ binary + 
    //{{{ complex + complex 
    template<class atype, class btype>
    complex<typename arithmetic<atype, btype>::type> operator +(
        complex<atype> aa, complex<btype> bb
    ) {
        return complex<typename arithmetic<atype, btype>::type>(
            aa.re + bb.re, aa.im + bb.im
        );
    }
    //}}}
    //{{{ real    + complex 
    template<class atype, class btype>
    complex<typename arithmetic<atype, btype>::type> operator +(
        atype aa, complex<btype> bb
    ) {
        return complex<typename arithmetic<atype, btype>::type>(
            aa + bb.re, bb.im
        );
    }
    //}}}
    //{{{ complex +    real 
    template<class atype, class btype>
    complex<typename arithmetic<atype, btype>::type> operator +(
        complex<atype> aa, btype bb
    ) {
        return complex<typename arithmetic<atype, btype>::type>(
            aa.re + bb, aa.im
        );
    }
    //}}}
    //}}}
    //{{{ binary * 
    //{{{ complex * complex 
    template<class atype, class btype>
    complex<typename arithmetic<atype, btype>::type> operator *(
        complex<atype> aa, complex<btype> bb
    ) {
        return complex<typename arithmetic<atype, btype>::type>(
            aa.re*bb.re - aa.im*bb.im,
            aa.re*bb.im + aa.im*bb.re
        );
    }
    //}}}
    //{{{ real    * complex 
    template<class atype, class btype>
    complex<typename arithmetic<atype, btype>::type> operator *(
        atype aa, complex<btype> bb
    ) {
        return complex<typename arithmetic<atype, btype>::type>(
            aa*bb.re, aa*bb.im
        );
    }
    //}}}
    //{{{ complex *    real 
    template<class atype, class btype>
    complex<typename arithmetic<atype, btype>::type> operator *(
        complex<atype> aa, btype bb
    ) {
        return complex<typename arithmetic<atype, btype>::type>(
            aa.re*bb, aa.im*bb
        );
    }
    //}}}

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
    //{{{ matrix * vector 
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
    //}}}
    //{{{ vector * matrix 
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
    //{{{ matrix * matrix 
    template<class t0, int64 r0, int64 c0, class t1, int64 r1, int64 c1>
    typename internal::matmultype<t0, r0, c0, t1, r1, c1>::type operator *(
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
    
    //}}}
    

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

}

int main() {
    using namespace xm;

    matrix<cdouble> A(10, 10);
    matrix<cdouble> B(10, 10);
    dump("A*B", A*B);
    //matrix<cdouble> C = A*B;
    //A*B;



    return 0;
}
