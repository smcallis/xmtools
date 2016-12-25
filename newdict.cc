#include "xmtools.h"

namespace xm {

    //{{{ dict implementation
    namespace internal {

        template<class ktype, class vtype>
        struct dictbase {
            virtual ~dictbase() = 0;

            virtual int64 getsize() const = 0;
            virtual int64 getbins() const = 0;

            virtual uint64_t gethash(int64 ii) const = 0;
            virtual const ktype& getkey(int64 ii) const = 0;
            virtual const vtype& getval(int64 ii) const = 0;
            virtual vtype& getval(int64 ii) = 0;

            virtual void insert(uint64_t code, const ktype& key, const ktype& val) = 0;
            virtual int64 search(uint64_t code, const ktype& key) const = 0;
            virtual void remove(uint64_t code, const ktype& key) = 0;

            virtual void debug() const = 0;
        };

        template<class ktype, class vtype>
        struct keyval {
            int64 bin;
            ktype key;
            vtype val;
            const vtype& getval() const { return val; }
            vtype& getval() { return val; }
        };

        static none nothing;
        template<class ktype>
        struct keyval<ktype, none> {
            int64 bin;
            ktype key;
            const none& getval() const { return nothing; }
            none& getval() { return nothing; }
        };

        template<class ktype, class vtype, int64 bins>
        struct dictimpl : dictbase<ktype, vtype> {
            ~dictimpl();
            dictimpl();

            int64 getsize() const;
            int64 getbins() const;

            uint64_t gethash(int64 ii) const;
            const ktype& getkey(int64 ii) const;
            const vtype& getval(int64 ii) const;
                  vtype& getval(int64 ii);

            void insert(uint64_t code, const ktype& key, const vtype& val);
            int64 search(uint64_t code, const ktype& key) const;
            void remove(uint64_t code, const ktype& key);

            void debug() const;

            private:

                int64 size;
                struct {
                    uint64_t code;
                    int64_t offset;
                } index[bins];
                union {
                    // these are intentionally not constructed
                    keyval<ktype, vtype> table[3*bins/4];
                };
        };

        template<class ktype, class vtype, int64 bins>
        dictimpl<ktype, vtype, bins>::~dictimpl() {
            for (int64 ii = 0; ii<size; ii++) {
                table[ii].~keyval<ktype, vtype>();
            }
            free(this);
        }

        template<class ktype, class vtype, int64 bins>
        int64 dictimpl<ktype, vtype, bins>::getsize() const {
            return size;
        }

        template<class ktype, class vtype, int64 bins>
        int64 dictimpl<ktype, vtype, bins>::getbins() const {
            return bins;
        }

        template<class ktype, class vtype, int64 bins>
        uint64_t dictimpl<ktype, vtype, bins>::gethash(int64 ii) const {
            return index[table[ii].bin].code;
        }

        template<class ktype, class vtype, int64 bins>
        const ktype& dictimpl<ktype, vtype, bins>::getkey(int64 ii) const {
            return table[ii].key;
        }

        template<class ktype, class vtype, int64 bins>
        const vtype& dictimpl<ktype, vtype, bins>::getval(int64 ii) const {
            return table[ii].getval();
        }

        template<class ktype, class vtype, int64 bins>
        vtype& dictimpl<ktype, vtype, bins>::getval(int64 ii) {
            return table[ii].getval();
        }

        template<class ktype, class vtype, int64 bins>
        void dictimpl<ktype, vtype, bins>::insert(
            uint64_t code, const ktype& key, const vtype& val
        ) {
            (void)code; (void)key; (void)val; // XXX
        }

        template<class ktype, class vtype, int64 bins>
        int64 dictimpl<ktype, vtype, bins>::search(
            uint64_t code, const ktype& key
        ) const {
            return -1; // XXX
        }

        template<class ktype, class vtype, int64 bins>
        void dictimpl<ktype, vtype, bins>::remove(
            uint64_t code, const ktype& key
        ) {
            (void)code; (void)key; // XXX
        }

        template<class ktype, class vtype, int64 bins>
        void dictimpl<ktype, vtype, bins>::debug() const {
            fprintf(stderr, "----------------\n");
            for (int64 ii = 0; ii<bins; ii++) {
                fprintf(stderr, "%016x : %lld\n", index[ii].code, index[ii].offset);
            }
            for (int64 ii = 0; ii<size; ii++) {
                fprintf(stderr, "key: %10lld, val: %10lld\n", table[ii].getkey(), table[ii].getval());
            }
        }

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
            static internal::dictbase<ktype, vtype>* create(int64 size);
            internal::dictbase<ktype, vtype>* storage;
    };

    template<class ktype, class vtype>
    newdict<ktype, vtype>::~newdict() {
        if (storage) delete storage;
    }

    template<class ktype, class vtype>
    newdict<ktype, vtype>::newdict() : storage(0) {}

    template<class ktype, class vtype>
    internal::dictbase<ktype, vtype>* newdict<ktype, vtype>::create(int64 size) {
        using namespace internal;
        switch (size) {
            case 1ULL<< 2: return new dictimpl<ktype, vtype, 1ULL<< 2>();
            case 1ULL<< 3: return new dictimpl<ktype, vtype, 1ULL<< 3>();
            case 1ULL<< 4: return new dictimpl<ktype, vtype, 1ULL<< 4>();
            case 1ULL<< 6: return new dictimpl<ktype, vtype, 1ULL<< 6>();
            case 1ULL<< 7: return new dictimpl<ktype, vtype, 1ULL<< 7>();
            case 1ULL<< 8: return new dictimpl<ktype, vtype, 1ULL<< 8>();
            case 1ULL<< 9: return new dictimpl<ktype, vtype, 1ULL<< 9>();
            case 1ULL<<10: return new dictimpl<ktype, vtype, 1ULL<<10>();
            case 1ULL<<11: return new dictimpl<ktype, vtype, 1ULL<<11>();
            case 1ULL<<12: return new dictimpl<ktype, vtype, 1ULL<<12>();
            case 1ULL<<13: return new dictimpl<ktype, vtype, 1ULL<<13>();
            case 1ULL<<14: return new dictimpl<ktype, vtype, 1ULL<<14>();
            case 1ULL<<16: return new dictimpl<ktype, vtype, 1ULL<<16>();
            case 1ULL<<17: return new dictimpl<ktype, vtype, 1ULL<<17>();
            case 1ULL<<18: return new dictimpl<ktype, vtype, 1ULL<<18>();
            case 1ULL<<19: return new dictimpl<ktype, vtype, 1ULL<<19>();
            case 1ULL<<20: return new dictimpl<ktype, vtype, 1ULL<<20>();
            case 1ULL<<21: return new dictimpl<ktype, vtype, 1ULL<<21>();
            case 1ULL<<22: return new dictimpl<ktype, vtype, 1ULL<<22>();
            case 1ULL<<23: return new dictimpl<ktype, vtype, 1ULL<<23>();
            case 1ULL<<24: return new dictimpl<ktype, vtype, 1ULL<<24>();
            case 1ULL<<26: return new dictimpl<ktype, vtype, 1ULL<<26>();
            case 1ULL<<27: return new dictimpl<ktype, vtype, 1ULL<<27>();
            case 1ULL<<28: return new dictimpl<ktype, vtype, 1ULL<<28>();
            case 1ULL<<29: return new dictimpl<ktype, vtype, 1ULL<<29>();
            case 1ULL<<30: return new dictimpl<ktype, vtype, 1ULL<<30>();
            case 1ULL<<31: return new dictimpl<ktype, vtype, 1ULL<<31>();
            case 1ULL<<32: return new dictimpl<ktype, vtype, 1ULL<<32>();
            case 1ULL<<33: return new dictimpl<ktype, vtype, 1ULL<<33>();
            case 1ULL<<34: return new dictimpl<ktype, vtype, 1ULL<<34>();
            case 1ULL<<36: return new dictimpl<ktype, vtype, 1ULL<<36>();
            case 1ULL<<37: return new dictimpl<ktype, vtype, 1ULL<<37>();
            case 1ULL<<38: return new dictimpl<ktype, vtype, 1ULL<<38>();
            case 1ULL<<39: return new dictimpl<ktype, vtype, 1ULL<<39>();
            case 1ULL<<40: return new dictimpl<ktype, vtype, 1ULL<<40>();
            case 1ULL<<41: return new dictimpl<ktype, vtype, 1ULL<<41>();
            case 1ULL<<42: return new dictimpl<ktype, vtype, 1ULL<<42>();
            case 1ULL<<43: return new dictimpl<ktype, vtype, 1ULL<<43>();
            case 1ULL<<44: return new dictimpl<ktype, vtype, 1ULL<<44>();
            case 1ULL<<46: return new dictimpl<ktype, vtype, 1ULL<<46>();
            case 1ULL<<47: return new dictimpl<ktype, vtype, 1ULL<<47>();
            case 1ULL<<48: return new dictimpl<ktype, vtype, 1ULL<<48>();
            case 1ULL<<49: return new dictimpl<ktype, vtype, 1ULL<<49>();
            case 1ULL<<50: return new dictimpl<ktype, vtype, 1ULL<<50>();
            case 1ULL<<51: return new dictimpl<ktype, vtype, 1ULL<<51>();
            case 1ULL<<52: return new dictimpl<ktype, vtype, 1ULL<<52>();
            case 1ULL<<53: return new dictimpl<ktype, vtype, 1ULL<<53>();
            case 1ULL<<54: return new dictimpl<ktype, vtype, 1ULL<<54>();
            case 1ULL<<56: return new dictimpl<ktype, vtype, 1ULL<<56>();
            case 1ULL<<57: return new dictimpl<ktype, vtype, 1ULL<<57>();
            case 1ULL<<58: return new dictimpl<ktype, vtype, 1ULL<<58>();
            case 1ULL<<59: return new dictimpl<ktype, vtype, 1ULL<<59>();
            case 1ULL<<60: return new dictimpl<ktype, vtype, 1ULL<<60>();
            default: break;
        }
        check(false, "internal error bins: %lld\n", size);
        return 0;
    }

}

int main() {
    using namespace xm::internal;

    printf("size: %zd\n", sizeof(dictimpl<int, int, 8>));

    return 0;
}

