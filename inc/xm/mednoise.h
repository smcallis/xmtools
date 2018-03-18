#ifndef XM_MEDNOISE_H_
#define XM_MEDNOISE_H_ 1

#include <math.h>

#include "basics.h"
#include "complex.h"
#include "vector.h"

namespace xm {

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

}

#endif // XM_MEDNOISE_H_
