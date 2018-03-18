#ifndef XM_FIRWIN_H_
#define XM_FIRWIN_H_ 1

#include <math.h>

#include "bessel.h"

namespace xm {

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
                // taps = 1.673904 * input_sample_rate / transition_bandwidth
                return 0.5 + 0.5*cos(xx);
            case 2: // Nuttall 3-term (-80.41dB, -18dB per octave)
                // taps = 2.810852 * input_sample_rate / transition_bandwidth
                return 0.41 + 0.5*cos(xx) + 0.09*cos(2*xx);
            case 3: // Nuttall 4-term (-114.38dB, -18dB per octave)
                // taps = 3.891769 * input_sample_rate / transition_bandwidth
                return (
                    0.353478834 + 0.486608654*cos(xx) +
                    0.146521166*cos(2*xx) + 0.013391346*cos(3*xx)
                );
            case 4: // Nuttall, Albrecht (-134.96dB, -18dB per octave)
                // taps = 4.830276 * input_sample_rate / transition_bandwidth
                return (
                    0.323497331 + 0.471838046*cos(xx) +
                    0.175295953*cos(2*xx) + 0.028161954*cos(3*xx) +
                    0.001206716*cos(4*xx)
                );
            case 5: // Nuttall, Albrecht (-163.69dB, -18dB per octave)
                // taps = 5.901855 * input_sample_rate / transition_bandwidth
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

}

#endif // XM_FIRWIN_H_

