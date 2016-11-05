#include "xmtools.h"
#include <typeinfo>

static void test0000() {
    xm::check(true, "check true works");
}

static void test0001() {
    // Does vec<float> promote to vec<cfloat> correctly?
    xm::vec<float> v(100);
    v[1] = 3;
    xm::cfloat z(1, 3);
    xm::vec<xm::cfloat> w = z*v + v;
    xm::check(w.size() == 100, "size");
    xm::check(w[1].re == 6, "real part");
    xm::check(w[1].im == 9, "imag part");
}

static void test0002() {
    // Does mat<float> promote to mat<cfloat> correctly?
    xm::mat<float> m(100, 100);
    m(1, 1) = 3;
    xm::cfloat z(1, 3);
    xm::mat<xm::cfloat> w = z*m + m;
    xm::check(w.size() == 100*100, "size");
    xm::check(w(1, 1).re == 6, "real part");
    xm::check(w(1, 1).im == 9, "imag part");
}

static void test0003() {
    // testing tuples

    int8_t aa;
    int16_t bb;
    int32_t cc;
    int64_t dd;
    uint8_t ee;
    uint16_t ff;
    uint32_t gg;
    uint64_t hh;

    xm::multiref(aa, bb, cc, dd, ee, ff, gg, hh) = xm::multival(1, 2, 3, 4, 5, 6, 7, 8);
    xm::multiref(aa, bb, cc, dd, ee, ff, gg    ) = xm::multival(1, 2, 3, 4, 5, 6, 7   );
    xm::multiref(aa, bb, cc, dd, ee, ff        ) = xm::multival(1, 2, 3, 4, 5, 6      );
    xm::multiref(aa, bb, cc, dd, ee            ) = xm::multival(1, 2, 3, 4, 5         );
    xm::multiref(aa, bb, cc, dd                ) = xm::multival(1, 2, 3, 4            );
    xm::multiref(aa, bb, cc                    ) = xm::multival(1, 2, 3               );
    xm::multiref(aa, bb                        ) = xm::multival(1, 2                  );
    xm::multiref(aa                            ) = xm::multival(1                     );
    xm::multiref(                              ) = xm::multival(                      );

    xm::check(aa == 1, "aa");
    xm::check(bb == 2, "bb");
    xm::check(cc == 3, "cc");
    xm::check(dd == 4, "dd");
    xm::check(ee == 5, "ee");
    xm::check(ff == 6, "ff");
    xm::check(gg == 7, "gg");
    xm::check(hh == 8, "hh");

    xm::dict<xm::tuple<int64, int64>, int> xx;
    xx[xm::multival(10, 20)] = 30;
    xx[xm::multival(30, 40)] = 90;
    xm::check(xx[xm::multival(10, 20)] == 30, "10,20");
    xm::check(xx[xm::multival(30, 40)] == 90, "30,40");

    xm::check(xm::multiref(aa, bb, cc, dd, ee, ff, gg, hh) == xm::multival(1, 2, 3, 4, 5u, 6u, 7u, 8u), "8");
    xm::check(xm::multiref(aa, bb, cc, dd, ee, ff, gg    ) == xm::multival(1, 2, 3, 4, 5u, 6u, 7u    ), "7");
    xm::check(xm::multiref(aa, bb, cc, dd, ee, ff        ) == xm::multival(1, 2, 3, 4, 5u, 6u        ), "6");
    xm::check(xm::multiref(aa, bb, cc, dd, ee            ) == xm::multival(1, 2, 3, 4, 5u            ), "5");
    xm::check(xm::multiref(aa, bb, cc, dd                ) == xm::multival(1, 2, 3, 4                ), "4");
    xm::check(xm::multiref(aa, bb, cc                    ) == xm::multival(1, 2, 3                   ), "3");
    xm::check(xm::multiref(aa, bb                        ) == xm::multival(1, 2                      ), "2");
    xm::check(xm::multiref(aa                            ) == xm::multival(1                         ), "1");
    xm::check(xm::multiref(                              ) == xm::multival(                          ), "0");
 
}


static void test0004() {
    static const int correct[38] = {
        // See ICD GPC 200, pages 8-9, First 10 C/A Chips in Octal
            0, 01440, 01620, 01710, 01744, 01133, 01455, 01131, 01454, 01626,
        01504, 01642, 01750, 01764, 01772, 01775, 01776, 01156, 01467, 01633,
        01715, 01746, 01763, 01063, 01706, 01743, 01761, 01770, 01774, 01127,
        01453, 01625, 01712, 01745, 01713, 01134, 01456, 01713
    };

    for (int prn = 1; prn<=37; prn++) {
        xm::gpsgold gold(prn);
        int test = 0;
        for (int ii = 0; ii<10; ii++) {
            test <<= 1;
            test |= gold.bit();
            gold.next();
        }
        //printf("%2d: %04o %04o\n", prn, test, correct[prn]);
        xm::check(test == correct[prn], "matching prn %d", prn);
    }

}

template<class atype, class btype>
void test0005_t1(const char* aname, const char* bname) {
    atype aval = 0;
    btype bval = 0;
    typename xm::promotion<atype, btype>::type tt = 0;
    xm::check(
        strcmp(typeid(tt).name(), typeid(aval + bval).name()) == 0,
        "promotion of %s + %s to %s", aname, bname, typeid(tt).name()
    );
    xm::check(
        strcmp(typeid(tt).name(), typeid(aval * bval).name()) == 0,
        "promotion of %s * %s to %s", aname, bname, typeid(tt).name()
    );
}

template<class atype>
void test0005_t0(const char* aname) {
    test0005_t1<atype,               char>(aname, "              char");
    test0005_t1<atype,        signed char>(aname, "       signed char");
    test0005_t1<atype,      unsigned char>(aname, "     unsigned char");
    test0005_t1<atype,       signed short>(aname, "      signed short");
    test0005_t1<atype,     unsigned short>(aname, "    unsigned short");
    test0005_t1<atype,         signed int>(aname, "        signed int");
    test0005_t1<atype,       unsigned int>(aname, "      unsigned int");
    test0005_t1<atype,        signed long>(aname, "       signed long");
    test0005_t1<atype,      unsigned long>(aname, "     unsigned long");
    test0005_t1<atype,   signed long long>(aname, "  signed long long");
    test0005_t1<atype, unsigned long long>(aname, "unsigned long long");
    test0005_t1<atype,              float>(aname, "             float");
    test0005_t1<atype,             double>(aname, "            double");
    test0005_t1<atype,        long double>(aname, "       long double");
}


static void test0005() {
    test0005_t0<              char>("              char");
    test0005_t0<       signed char>("       signed char");
    test0005_t0<     unsigned char>("     unsigned char");
    test0005_t0<      signed short>("      signed short");
    test0005_t0<    unsigned short>("    unsigned short");
    test0005_t0<        signed int>("        signed int");
    test0005_t0<      unsigned int>("      unsigned int");
    test0005_t0<       signed long>("       signed long");
    test0005_t0<     unsigned long>("     unsigned long");
    test0005_t0<  signed long long>("  signed long long");
    test0005_t0<unsigned long long>("unsigned long long");
    test0005_t0<             float>("             float");
    test0005_t0<            double>("            double");
    test0005_t0<       long double>("       long double");
}



int main() {
    test0000();
    test0001();
    test0002();
    test0003();
    test0004();
    test0005();

    return 0;
}
