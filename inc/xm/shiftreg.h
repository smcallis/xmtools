#ifndef XM_SHIFTREG_H_
#define XM_SHIFTREG_H_ 1

namespace xm {

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


}

#endif // XM_SHIFTREG_H_

