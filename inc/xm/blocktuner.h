#ifndef XM_BLOCKTUNER_H_
#define XM_BLOCKTUNER_H_ 1

namespace xm {

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

}

#endif // XM_BLOCKTUNER_H_

