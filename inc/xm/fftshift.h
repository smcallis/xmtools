#ifndef XM_FFTSHIFT_H_
#define XM_FFTSHIFT_H_ 1

namespace xm {

    //{{{ internal
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
    //}}}

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


}


#endif // XM_FFTSHIFT_H_

