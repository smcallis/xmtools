#ifndef XM_GPSGOLD_H_
#define XM_GPSGOLD_H_ 1

#include "shiftreg.h"

namespace xm {

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

}

#endif // XM_GPSGOLD_H_

