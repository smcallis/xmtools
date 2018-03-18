#ifndef XM_PROMOTE_H_
#define XM_PROMOTE_H_ 1

namespace xm {

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

}

#endif // XM_PROMOTE_H_

