#ifndef XM_COMPLEX_H_
#define XM_COMPLEX_H_ 1

#include "promote.h"

namespace xm {

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

    // common complex types
    typedef complex<int8_t>  cbyte;
    typedef complex<int16_t> cshort;
    typedef complex<float>   cfloat;
    typedef complex<double>  cdouble;

    
    // prefix - 
    template<class type>
    complex<type> operator -(const complex<type>& zz) {
        return complex<type>(-zz.re, -zz.im);
    }

    // prefix + 
    template<class type>
    complex<type> operator +(const complex<type>& zz) {
        return zz;
    }

    // binary == 
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

    // binary != 
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

    // binary + 
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

    // binary - 
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

    // binary * 
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

    // binary / 
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

    // operator += 
    template<class atype, class btype>
    complex<atype>& operator += (complex<atype>& aa, const complex<btype>& bb) {
        return aa = aa + bb;
    }

    template<class atype, class btype>
    complex<atype>& operator += (complex<atype>& aa, const btype& bb) {
        return aa = aa + bb;
    }

    // operator -= 
    template<class atype, class btype>
    complex<atype>& operator -= (complex<atype>& aa, const complex<btype>& bb) {
        return aa = aa - bb;
    }

    template<class atype, class btype>
    complex<atype>& operator -= (complex<atype>& aa, const btype& bb) {
        return aa = aa - bb;
    }

    // operator *= 
    template<class atype, class btype>
    complex<atype>& operator *= (complex<atype>& aa, const complex<btype>& bb) {
        return aa = aa * bb;
    }

    template<class atype, class btype>
    complex<atype>& operator *= (complex<atype>& aa, const btype& bb) {
        return aa = aa * bb;
    }

    // operator /= 
    template<class atype, class btype>
    complex<atype>& operator /= (complex<atype>& aa, const complex<btype>& bb) {
        return aa = aa / bb;
    }

    template<class atype, class btype>
    complex<atype>& operator /= (complex<atype>& aa, const btype& bb) {
        return aa = aa / bb;
    }

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

}

#endif // XM_COMPLEX_H_

