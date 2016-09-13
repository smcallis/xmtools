#ifndef KISSFFT_H_
#define KISSFFT_H_

namespace xm {

    //{{{ spread
    //
    template<class type> type spread(double val);

    template<> float spread<float>(double val) {
        return (float)val;
    }

    template<> double spread<double>(double val) {
        return val;
    }

    template<> f32x4 spread<f32x4>(double dd) {
        float ff = (float)dd;
        return (f32x4){ ff, ff, ff, ff };
    }

    template<> f64x2 spread<f64x2>(double dd) {
        return (f64x2){ dd, dd };
    }
    //}}}
    //{{{ kissfft

    //{{{ License:
    //
    //  Some FFT routines below are taken from the KISS FFT library.  They have
    //  been modified a bit to support C++ and template arguments (float,
    //  double, simd).  As required, here is the original (BSD style) license:
    //
    //
    //    Copyright (c) 2003-2004, Mark Borgerding 
    //    
    //    All rights reserved.
    //   
    //    Redistribution and use in source and binary forms, with or without
    //    modification, are permitted provided that the following conditions
    //    are met:
    //   
    //      * Redistributions of source code must retain the above copyright
    //        notice, this list of conditions and the following disclaimer.
    //   
    //      * Redistributions in binary form must reproduce the above
    //        copyright notice, this list of conditions and the following
    //        disclaimer in the documentation and/or other materials provided
    //        with the distribution.
    //
    //      * Neither the author nor the names of any contributors may be
    //        used to endorse or promote products derived from this software
    //        without specific prior written permission.
    //
    //    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
    //    "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
    //    LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
    //    A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
    //    OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
    //    SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
    //    LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
    //    DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
    //    THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
    //    (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
    //    OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
    //
    //}}}

    template<class type>
    struct kissfft {
        kissfft(int64 samples, bool inverse=false);

        void exec(cx<type>* dst, const cx<type>* src) const;

        private:
            void work(
                cx<type>* dst, const cx<type>* src,
                int64 fstride, int64 istride, const int64* factptr
            ) const;

            void bfly2(cx<type>* dst, int64 fstride, int64 mm) const;
            void bfly3(cx<type>* dst, int64 fstride, int64 mm) const;
            void bfly4(cx<type>* dst, int64 fstride, int64 mm) const;
            void bfly5(cx<type>* dst, int64 fstride, int64 mm) const;
            void bflyn(cx<type>* dst, int64 fstride, int64 mm, int64 pp) const;

            bool inverse;
            list<int64> factors;
            vec<cx<type> > twiddles;
            mutable vec<cx<type> > scratch;
    };

    //{{{ constructor
    template<class type>
    kissfft<type>::kissfft(int64 nn, bool inv) : inverse(inv), twiddles(nn) {

        for (int64 ii = 0; ii < nn; ii++) {
            double phase = -2 * M_PI * ii / nn;
            if (inv) phase *= -1;
            twiddles[ii].re = spread<type>(cos(phase));
            twiddles[ii].im = spread<type>(sin(phase));
        }

        int64 pp = 4;
        double floor_sqrt = floor(sqrt((double) nn));

        do {
            while (nn % pp) {
                switch (pp) {
                    case 4: pp = 2; break;
                    case 2: pp = 3; break;
                    default: pp += 2; break;
                }
                if (pp > floor_sqrt) pp = nn;
            }
            nn /= pp;
            factors.append(pp);
            factors.append(nn);
        } while (nn > 1);
    }
    //}}}
    //{{{ exec and work
    template<class type>
    void kissfft<type>::exec(cx<type>* dst, const cx<type>* src) const {
        work(dst, src, 1, 1, factors.data());
    }


    template<class type>
    void kissfft<type>::work(
        cx<type>* dst, const cx<type>* src,
        int64 fstride, int64 istride, const int64* factptr
    ) const {

        cx<type> *dst_beg = dst;
        const int64 pp = *factptr++;
        const int64 mm = *factptr++;
        const cx<type> *dst_end = dst + pp * mm;

        if (mm == 1) {
            do {
                *dst = *src;
                src += fstride * istride;
            } while (++dst != dst_end);
        } else {
            do {
                work(dst, src, fstride * pp, istride, factptr);
                src += fstride * istride;
            } while ((dst += mm) != dst_end);
        }

        dst = dst_beg;

        switch (pp) {
            case  2: bfly2(dst, fstride, mm); break;
            case  3: bfly3(dst, fstride, mm); break;
            case  4: bfly4(dst, fstride, mm); break;
            case  5: bfly5(dst, fstride, mm); break;
            default: bflyn(dst, fstride, mm, pp); break;
        }
    }
    //}}}
    //{{{ bfly2
    template<class type>
    void kissfft<type>::bfly2(cx<type>* dst, int64 fstride, int64 mm) const {
        cx<type> *dst2 = dst + mm;
        const cx<type> *tw1 = twiddles.data();
        cx<type> tt;
        do {
            tt.re = dst2->re*tw1->re - dst2->im*tw1->im;
            tt.im = dst2->re*tw1->im + dst2->im*tw1->re;
            tw1 += fstride;
            dst2->re = dst->re - tt.re;
            dst2->im = dst->im - tt.im;
            dst->re += tt.re;
            dst->im += tt.im;
            ++dst2;
            ++dst;
        } while (--mm);
    }
    //}}}
    //{{{ bfly3
    template<class type>
    void kissfft<type>::bfly3(cx<type>* dst, int64 fstride, int64 mm) const {
        int64 kk = mm;
        const int64 m2 = 2 * mm;
        const cx<type> *tw1, *tw2;
        cx<type> scratch[5];
        cx<type> epi3 = twiddles[fstride * mm];

        tw1 = tw2 = twiddles.data();

        do {
            scratch[1].re = dst[mm].re*tw1->re - dst[mm].im*tw1->im;
            scratch[1].im = dst[mm].re*tw1->im + dst[mm].im*tw1->re;
            scratch[2].re = dst[m2].re*tw2->re - dst[m2].im*tw2->im;
            scratch[2].im = dst[m2].re*tw2->im + dst[m2].im*tw2->re;

            scratch[3].re = scratch[1].re + scratch[2].re;
            scratch[3].im = scratch[1].im + scratch[2].im;
            scratch[0].re = scratch[1].re - scratch[2].re;
            scratch[0].im = scratch[1].im - scratch[2].im;
            tw1 += fstride;
            tw2 += fstride * 2;

            dst[mm].re = dst->re - scratch[3].re*spread<type>(.5);
            dst[mm].im = dst->im - scratch[3].im*spread<type>(.5);

            scratch[0].re *= epi3.im;
            scratch[0].im *= epi3.im;

            dst->re += scratch[3].re;
            dst->im += scratch[3].im;

            dst[m2].re = dst[mm].re + scratch[0].im;
            dst[m2].im = dst[mm].im - scratch[0].re;

            dst[mm].re -= scratch[0].im;
            dst[mm].im += scratch[0].re;

            ++dst;
        } while (--kk);
    }
    //}}}
    //{{{ bfly4
    template<class type>
    void kissfft<type>::bfly4(cx<type>* dst, int64 fstride, int64 mm) const {

        const cx<type> *tw1, *tw2, *tw3;
        cx<type> scratch[6];
        int64 kk = mm;
        const int64 m2 = 2 * mm;
        const int64 m3 = 3 * mm;

        tw3 = tw2 = tw1 = twiddles.data();

        do {
            scratch[0].re = dst[mm].re*tw1->re - dst[mm].im*tw1->im;
            scratch[0].im = dst[mm].re*tw1->im + dst[mm].im*tw1->re;
            scratch[1].re = dst[m2].re*tw2->re - dst[m2].im*tw2->im;
            scratch[1].im = dst[m2].re*tw2->im + dst[m2].im*tw2->re;
            scratch[2].re = dst[m3].re*tw3->re - dst[m3].im*tw3->im;
            scratch[2].im = dst[m3].re*tw3->im + dst[m3].im*tw3->re;

            scratch[5].re = dst->re - scratch[1].re;
            scratch[5].im = dst->im - scratch[1].im;
            dst->re += scratch[1].re;
            dst->im += scratch[1].im;
            scratch[3].re = scratch[0].re + scratch[2].re;
            scratch[3].im = scratch[0].im + scratch[2].im;
            scratch[4].re = scratch[0].re - scratch[2].re;
            scratch[4].im = scratch[0].im - scratch[2].im;
            dst[m2].re = dst->re - scratch[3].re;
            dst[m2].im = dst->im - scratch[3].im;
            tw1 += fstride;
            tw2 += fstride * 2;
            tw3 += fstride * 3;
            dst->re += scratch[3].re;
            dst->im += scratch[3].im;

            if (inverse) {
                dst[mm].re = scratch[5].re - scratch[4].im;
                dst[mm].im = scratch[5].im + scratch[4].re;
                dst[m3].re = scratch[5].re + scratch[4].im;
                dst[m3].im = scratch[5].im - scratch[4].re;
            } else {
                dst[mm].re = scratch[5].re + scratch[4].im;
                dst[mm].im = scratch[5].im - scratch[4].re;
                dst[m3].re = scratch[5].re - scratch[4].im;
                dst[m3].im = scratch[5].im + scratch[4].re;
            }
            ++dst;
        } while (--kk);
    }
    //}}}
    //{{{ bfly5
    template<class type>
    void kissfft<type>::bfly5(cx<type>* dst, int64 fstride, int64 mm) const {
        cx<type> *dst0, *dst1, *dst2, *dst3, *dst4;
        int64 uu;
        cx<type> scratch[13];
        //const cx<type> *twiddles = twiddles.data();
        const cx<type> *tw;
        cx<type> ya, yb;
        ya = twiddles[fstride * mm];
        yb = twiddles[fstride * 2 * mm];

        dst0 = dst;
        dst1 = dst0 + mm;
        dst2 = dst0 + 2 * mm;
        dst3 = dst0 + 3 * mm;
        dst4 = dst0 + 4 * mm;

        tw = twiddles.data();
        for (uu = 0; uu < mm; ++uu) {
            scratch[0] = *dst0;

            scratch[ 1].re = dst1->re*tw[  uu*fstride].re - dst1->im*tw[  uu*fstride].im;
            scratch[ 1].im = dst1->re*tw[  uu*fstride].im + dst1->im*tw[  uu*fstride].re;
            scratch[ 2].re = dst2->re*tw[2*uu*fstride].re - dst2->im*tw[2*uu*fstride].im;
            scratch[ 2].im = dst2->re*tw[2*uu*fstride].im + dst2->im*tw[2*uu*fstride].re;
            scratch[ 3].re = dst3->re*tw[3*uu*fstride].re - dst3->im*tw[3*uu*fstride].im;
            scratch[ 3].im = dst3->re*tw[3*uu*fstride].im + dst3->im*tw[3*uu*fstride].re;
            scratch[ 4].re = dst4->re*tw[4*uu*fstride].re - dst4->im*tw[4*uu*fstride].im;
            scratch[ 4].im = dst4->re*tw[4*uu*fstride].im + dst4->im*tw[4*uu*fstride].re;

            scratch[ 7].re = scratch[1].re + scratch[4].re;
            scratch[ 7].im = scratch[1].im + scratch[4].im;
            scratch[10].re = scratch[1].re - scratch[4].re;
            scratch[10].im = scratch[1].im - scratch[4].im;
            scratch[ 8].re = scratch[2].re + scratch[3].re;
            scratch[ 8].im = scratch[2].im + scratch[3].im;
            scratch[ 9].re = scratch[2].re - scratch[3].re;
            scratch[ 9].im = scratch[2].im - scratch[3].im;

            dst0->re += scratch[7].re + scratch[8].re;
            dst0->im += scratch[7].im + scratch[8].im;

            scratch[5].re = scratch[0].re + scratch[7].re*ya.re + scratch[8].re*yb.re;
            scratch[5].im = scratch[0].im + scratch[7].im*ya.re + scratch[8].im*yb.re;

            scratch[6].re =  scratch[10].im * ya.im + scratch[9].im * yb.im;
            scratch[6].im = -scratch[10].re * ya.im - scratch[9].re * yb.im;

            dst1->re = scratch[5].re - scratch[6].re;
            dst1->im = scratch[5].im - scratch[6].im;
            dst4->re = scratch[5].re + scratch[6].re;
            dst4->im = scratch[5].im + scratch[6].im;

            scratch[11].re =  scratch[0].re + scratch[7].re*yb.re + scratch[8].re*ya.re;
            scratch[11].im =  scratch[0].im + scratch[7].im*yb.re + scratch[8].im*ya.re;
            scratch[12].re = -scratch[10].im*yb.im + scratch[9].im*ya.im;
            scratch[12].im =  scratch[10].re*yb.im - scratch[9].re*ya.im;

            dst2->re = scratch[11].re + scratch[12].re;
            dst2->im = scratch[11].im + scratch[12].im;
            dst3->re = scratch[11].re - scratch[12].re;
            dst3->im = scratch[11].im - scratch[12].im;

            ++dst0;
            ++dst1;
            ++dst2;
            ++dst3;
            ++dst4;
        }
    }
    //}}}
    //{{{ bflyn
    template<class type>
    void kissfft<type>::bflyn(cx<type>* dst, int64 fstride, int64 mm, int64 pp) const {
        if (scratch.size() < pp) scratch.resize(pp);

        int64 uu, kk, q1, qq;
        //const cx<type> *twiddles = st->twiddles;
        cx<type> tt;
        int64 Norig = twiddles.size();

        for (uu = 0; uu < mm; ++uu) {
            kk = uu;
            for (q1 = 0; q1 < pp; ++q1) {
                scratch[q1] = dst[kk];
                ;
                kk += mm;
            }

            kk = uu;
            for (q1 = 0; q1 < pp; ++q1) {
                int64 twidx = 0;
                dst[kk] = scratch[0];
                for (qq = 1; qq < pp; ++qq) {
                    twidx += fstride * kk;
                    if (twidx >= Norig)
                        twidx -= Norig;
                    tt.re = scratch[qq].re*twiddles[twidx].re - scratch[qq].im*twiddles[twidx].im;
                    tt.im = scratch[qq].re*twiddles[twidx].im + scratch[qq].im*twiddles[twidx].re;
                    dst[kk].re += tt.re;
                    dst[kk].im += tt.im;
                }
                kk += mm;
            }
        }
    }
    //}}}
    //}}}

    //{{{ kisssse
    
    struct kisssse {
        kisssse(int64 samples);

        void fft(cfloat* data);

        private:
            kissfft<f32x4> kiss;
            vec<cfloat> scratch;
    };

    kisssse::kisssse(int64 samples) : kiss(samples/4), scratch(samples) {
        check(samples > 0 && samples%4 == 0, "must be multiple of 4 size");
    }

    void kisssse::fft(cfloat* data) {
        const int64 size = scratch.size();
        const int64 quarter = size/4;
        float* ptr = (float*)data;

        for (int64 ii = 0; ii<quarter; ii++) {
            float re0 = ptr[ii*8 + 0];
            float im0 = ptr[ii*8 + 1];
            float re1 = ptr[ii*8 + 2];
            float im1 = ptr[ii*8 + 3];
            float re2 = ptr[ii*8 + 4];
            float im2 = ptr[ii*8 + 5];
            float re3 = ptr[ii*8 + 6];
            float im3 = ptr[ii*8 + 7];

            ptr[ii*8 + 0] = re0;
            ptr[ii*8 + 1] = re1;
            ptr[ii*8 + 2] = re2;
            ptr[ii*8 + 3] = re3;
            ptr[ii*8 + 4] = im0;
            ptr[ii*8 + 5] = im1;
            ptr[ii*8 + 6] = im2;
            ptr[ii*8 + 7] = im3;
        }

        float* tmp = (float*)scratch.data();
        typedef cx<f32x4> cf32x4;
        kiss.exec((cf32x4*)tmp, (cf32x4*)ptr);
        
        //memcpy(ptr, tmp, size*sizeof(cfloat));


        double cc = 1, ss = 0;
        double aa = 2*sqr(::sin(-M_PI/size));
        double bb = ::sin(-2*M_PI/size);

        for (int64 ii = 0; ii<quarter; ii++) {
            cfloat t1(cc, ss);
            cfloat t2 = t1*t1;
            cfloat t3 = t2*t1;

            cfloat z0 = cfloat(tmp[ii*8 + 0], tmp[ii*8 + 4]);
            cfloat z1 = cfloat(tmp[ii*8 + 1], tmp[ii*8 + 5])*t1;
            cfloat z2 = cfloat(tmp[ii*8 + 2], tmp[ii*8 + 6])*t2;
            cfloat z3 = cfloat(tmp[ii*8 + 3], tmp[ii*8 + 7])*t3;

            data[ii + 0*quarter].re = z0.re + z1.re + z2.re + z3.re;
            data[ii + 0*quarter].im = z0.im + z1.im + z2.im + z3.im;
            data[ii + 1*quarter].re = z0.re + z1.im - z2.re - z3.im;
            data[ii + 1*quarter].im = z0.im - z1.re - z2.im + z3.re;
            data[ii + 2*quarter].re = z0.re - z1.re + z2.re - z3.re;
            data[ii + 2*quarter].im = z0.im - z1.im + z2.im - z3.im;
            data[ii + 3*quarter].re = z0.re - z1.im - z2.re + z3.im;
            data[ii + 3*quarter].im = z0.im + z1.re - z2.im - z3.re;

            double tt = cc - (aa*cc + bb*ss);
            ss = ss + (bb*cc - aa*ss);
            cc = tt;
        }

    }


    //}}}
}

#endif
