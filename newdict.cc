#include "xmtools.h"

namespace xm {

    //{{{ dict implementation
    namespace internal {

        template<class ktype, class vtype>
        struct dictbase {
            virtual ~dictbase() = 0;
            virtual void insert(const ktype& key, const ktype& val) = 0;
            //virtual void insert(const ktype& key) = 0;
            //virtual void remove(const ktype& key) = 0;
            //virtual bool haskey(const ktype& key) = 0;
            //virtual vtype& lookup(const ktype& key) = 0;
            //virtual const vtype& lookup(const ktype& key) const = 0;
            //virtual void rehash(dictbase<ktype, vtype>* old) = 0;

            virtual void debug() = 0;
        };

        template<class ktype, class vtype>
        struct keyval {
            ktype key;
            vtype val;
            const ktype& getkey() const { return key; }
            const vtype& getval() const { return val; }
            vtype& getval() { return val; }
        };

        static none nothing;
        template<class ktype>
        struct keyval<ktype, none> {
            ktype key;
            const ktype& getkey() const { return key; }
            const none& getval() const { return nothing; }
            none& getval() { return nothing; }
        };

        template<class ktype, class vtype, int64 bins>
        struct dictimpl : dictbase<ktype, vtype> {
            ~dictimpl();
            dictimpl(int64 size);
            void insert(const ktype& key, const ktype& val);

            void debug();

            private:

                int64 size;
                struct {
                    uint64_t code;
                    int64_t offset;
                } index[bins];

                keyval<ktype, vtype> table[0];
        };
    }
    //}}}

    template<class ktype, class vtype=none>
    struct newdict {
        ~newdict();
        newdict();

        newdict(const newdict<ktype, vtype>& other);
        template<class kk, class vv>
        newdict(const newdict<kk, vv>& other);

        newdict& operator =(const newdict<ktype, vtype>& other);
        template<class kk, class vv>
        newdict& operator =(const newdict<kk, vv>& other);

        void insert(const ktype& key, const vtype& val);
        void insert(const ktype& key);
        bool haskey(const ktype& key) const;
        void remove(const ktype& key);

        void reserve(int64 size);
        void shrink();
        void clear();

        list<ktype> keys() const;
        list<vtype> vals() const;

        int64 size() const;
        const ktype& key(int64 bin) const;
        const vtype& val(int64 bin) const;
        vtype& val(int64 bin);

        private:
            template<int64 bins>
            internal::dictbase<ktype, vtype>* init();
            internal::dictbase<ktype, vtype>* create(int64 size);
            internal::dictbase<ktype, vtype>* storage;
    };

    template<class ktype, class vtype>
    newdict<ktype, vtype>::~newdict() {
        if (storage) delete storage;
    }

    template<class ktype, class vtype>
    newdict<ktype, vtype>::newdict() : storage(0) {}

    template<class ktype, class vtype>
    template<int64 bins>
    internal::dictbase<ktype, vtype>* newdict<ktype, vtype>::init() {
        internal::dictimpl<ktype, vtype, bins>* result = (
            alloc<internal::dictimpl<ktype, vtype, bins> >()
        );
        new(result) internal::dictimpl<ktype, vtype, bins>();
        return result;
    }

    template<class ktype, class vtype>
    internal::dictbase<ktype, vtype>* newdict<ktype, vtype>::create(int64 size) {
        check(size >= 0, "must have positive size (%lld)", size);
        int64 bins = 4;
        while (3*bins/4 < size) bins *= 2;
        switch (bins) {
            case                   4: return init<                  4>();
            case                   8: return init<                  8>();
            case                  16: return init<                 16>();
            case                  32: return init<                 32>();
            case                  64: return init<                 64>();
            case                 128: return init<                128>();
            case                 256: return init<                256>();
            case                 512: return init<                512>();
            case                1024: return init<               1024>();
            case                2048: return init<               2048>();
            case                4096: return init<               4096>();
            case                8192: return init<               8192>();
            case               16384: return init<              16384>();
            case               32768: return init<              32768>();
            case               65536: return init<              65536>();
            case              131072: return init<             131072>();
            case              262144: return init<             262144>();
            case              524288: return init<             524288>();
            case             1048576: return init<            1048576>();
            case             2097152: return init<            2097152>();
            case             4194304: return init<            4194304>();
            case             8388608: return init<            8388608>();
            case            16777216: return init<           16777216>();
            case            33554432: return init<           33554432>();
            case            67108864: return init<           67108864>();
            case           134217728: return init<          134217728>();
            case           268435456: return init<          268435456>();
            case           536870912: return init<          536870912>();
            case          1073741824: return init<         1073741824>();
            case          2147483648: return init<         2147483648>();
            case          4294967296: return init<         4294967296>();
            case          8589934592: return init<         8589934592>();
            case         17179869184: return init<        17179869184>();
            case         34359738368: return init<        34359738368>();
            case         68719476736: return init<        68719476736>();
            case        137438953472: return init<       137438953472>();
            case        274877906944: return init<       274877906944>();
            case        549755813888: return init<       549755813888>();
            case       1099511627776: return init<      1099511627776>();
            case       2199023255552: return init<      2199023255552>();
            case       4398046511104: return init<      4398046511104>();
            case       8796093022208: return init<      8796093022208>();
            case      17592186044416: return init<     17592186044416>();
            case      35184372088832: return init<     35184372088832>();
            case      70368744177664: return init<     70368744177664>();
            case     140737488355328: return init<    140737488355328>();
            case     281474976710656: return init<    281474976710656>();
            case     562949953421312: return init<    562949953421312>();
            case    1125899906842624: return init<   1125899906842624>();
            case    2251799813685248: return init<   2251799813685248>();
            case    4503599627370496: return init<   4503599627370496>();
            case    9007199254740992: return init<   9007199254740992>();
            case   18014398509481984: return init<  18014398509481984>();
            case   36028797018963968: return init<  36028797018963968>();
            case   72057594037927936: return init<  72057594037927936>();
            case  144115188075855872: return init< 144115188075855872>();
            case  288230376151711744: return init< 288230376151711744>();
            case  576460752303423488: return init< 576460752303423488>();
            case 1152921504606846976: return init<1152921504606846976>();
            case 2305843009213693952: return init<2305843009213693952>();
            case 4611686018427387904: return init<4611686018427387904>();
            default: break;
        }
        check(false, "internal error bins: %lld\n", bins);
        return 0;
    }

}

int main() {
    using namespace xm::internal;

    printf("size: %zd\n", sizeof(dictimpl<int, int, 8>));

    return 0;
}

