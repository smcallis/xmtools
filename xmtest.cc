#include "xmtools.h"
#include <typeinfo>

static void test0000() {
    xm::check(true, "check true works");
}

static void test0001() {
    // Does vector<float> promote to vector<cfloat> correctly?
    xm::vector<float> v(100, 0);
    v[1] = 3;
    xm::cfloat z(1, 3);
    xm::vector<xm::cfloat> w = z*v + v;
    //xm::vector<xm::cfloat> w = v*z + v;
    xm::check(w.size() == 100, "size");
    xm::check(w[1].re == 6, "real part");
    xm::check(w[1].im == 9, "imag part");
}

static void test0002() {
    // Does matrix<float> promote to matrix<cfloat> correctly?
    xm::matrix<float> m(100, 100, 0);
    m(1, 1) = 3;
    xm::cfloat z(1, 3);
    xm::matrix<xm::cfloat> w = z*m + m;
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
    typename xm::arithmetic<atype, btype>::type tt = 0;
    typename xm::conditional<atype, btype>::type uu = 0;
    xm::check(
        strcmp(typeid(tt).name(), typeid(aval + bval).name()) == 0,
        "promotion of %s + %s to %s", aname, bname, typeid(tt).name()
    );
    xm::check(
        strcmp(typeid(tt).name(), typeid(aval * bval).name()) == 0,
        "promotion of %s * %s to %s", aname, bname, typeid(tt).name()
    );
    xm::check(
        strcmp(typeid(uu).name(), typeid(true ? aval : bval).name()) == 0,
        "promotion of cond ? %s : %s to %s", aname, bname, typeid(uu).name()
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

static void test0006_helper(bool reserve) {
    const int64 maxsize = 1000;
    using namespace xm;
    dict<int32_t, int64_t> dd;
    dd.test();
    if (reserve) dd.reserve(maxsize);
    dd.test();
    for (int64 ii = 0; ii<maxsize; ii++) {
        dd.insert(ii*ii, (ii + 1)*1001);
        dd.test();
        for (int jj = 0; jj<=ii; jj++) {
            check(dd[jj*jj] == (jj + 1)*1001, "matches %d %d", jj*jj, (jj + 1)*1001);
        }
    }
    list<int32_t> keys = dd.keys();
    list<int32_t> vals = dd.vals();
    for (int64 ii = 0; ii<maxsize; ii++) {
        check(keys[ii] == ii*ii, "keys correct");
        check(vals[ii] == (ii + 1)*1001, "vals correct");
    }

    for (int64 ii = 0; ii<maxsize; ii++) {
        dd.remove(ii*ii);
        if (!reserve) dd.shrink();
        dd.test();
        for (int jj = ii+1; jj<maxsize; jj++) {
            check(dd[jj*jj] == (jj + 1)*1001, "matches %d %d", jj*jj, (jj + 1)*1001);
        }
    }
    dd.test();
}

static void test0006() {
    test0006_helper(true);
    test0006_helper(false);
}

static void test0007() {
    using namespace xm;

    dict<int64> one;
    for (int64 ii = 0; ii<5000000; ii++) {
        one.insert(ii);
    }
    one.test();

    // this used to exhibit quadratic runtime
    dict<int64> two;
    for (int64 ii = 0; ii<one.size(); ii++) {
        two.insert(one.key(ii));
    }
    two.test();
}

namespace test0008_internal {
    static int64 keyrefs;
    struct countedkey {
        int64 val;
        countedkey() : val(0) {
            ++keyrefs;
        }
        countedkey(int64 val) : val(val) {
            ++keyrefs;
        }
        ~countedkey() {
            --keyrefs;
        }
        private:
            countedkey(const countedkey&); // = delete
            void operator =(const countedkey&); // = delete
    };
    static inline bool operator ==(
        const countedkey& aa, const countedkey& bb
    ) {
        return aa.val == bb.val;
    }

    static inline uint64_t hash(const countedkey& key) {
        return xm::hash(key.val);
    }

    static inline void swap(countedkey& aa, countedkey& bb) {
        xm::swap(aa.val, bb.val);
    }

    static int64 valrefs;
    struct countedval {
        int64 val;
        countedval() : val(0) {
            ++valrefs;
        }
        countedval(int64 val) : val(val) {
            ++valrefs;
        }
        ~countedval() {
            --valrefs;
        }
        private:
            countedval(const countedval&); // = delete
            void operator =(const countedval&); // = delete
    };

    static inline void swap(countedval& aa, countedval& bb) {
        xm::swap(aa.val, bb.val);
    }

}

static void test0008() {
    using namespace xm;
    using namespace test0008_internal;

    check(keyrefs == 0, "keyrefs 0 before");
    check(valrefs == 0, "valrefs 0 before");
    {
        dict<countedkey, countedval> dd;

        for (int64 ii = 0; ii<10000; ii++) {
            countedkey key(ii);
            countedval val(ii*ii);
            dd.inswap(key, val);
        }
    }

    check(keyrefs == 0, "keyrefs 0 after");
    check(valrefs == 0, "valrefs 0 after");
}

int main() {
    test0000();
    test0001();
    test0002();
    test0003();
    test0004();
    test0005();
    test0006();
    test0007();
    test0008();

    return 0;
}
