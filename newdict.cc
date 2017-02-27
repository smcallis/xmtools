#include "xmtools.h"

namespace xm {

    namespace internal {

        //{{{ bucket

        template<class ktype, class vtype>
        struct bucket {
            uint64_t index;
            uint64_t code;
            ktype key;
            vtype val;

            const vtype& getval() const;
            vtype& getval();
        };

        template<class ktype, class vtype>
        const vtype& bucket<ktype, vtype>::getval() const {
            return val;
        }

        template<class ktype, class vtype>
        vtype& bucket<ktype, vtype>::getval() {
            return val;
        }

        template<class ktype>
        struct bucket<ktype, none> {
            // specialized for vtype none
            uint64_t index;
            uint64_t code;
            ktype key;

            const none& getval() const;
            none& getval();
        };

        template<class ktype>
        const none& bucket<ktype, none>::getval() const {
            static none nil;
            return nil;
        }

        template<class ktype>
        none& bucket<ktype, none>::getval() {
            static none nil;
            return nil;
        }

        //}}}
        //{{{ dictbase
        template<class ktype, class vtype>
        struct dictbase {
            virtual ~dictbase() {};
            
            virtual int64 bins() const = 0;
            virtual int64 size() const = 0;

            virtual const bucket<ktype, vtype>* buckets() const = 0;
            virtual bucket<ktype, vtype>* buckets() = 0;

            virtual vtype& inswap(uint64_t code, ktype& key, vtype& val) = 0;
            virtual const vtype* locate(uint64_t code, const ktype& key) const = 0;
            virtual void reject(uint64_t code, const ktype& key) = 0;

            virtual void debug() const = 0;
        };
        //}}}

        template<class ktype, class vtype, class ttype, uint64_t bsize>
        struct dictimpl : dictbase<ktype, vtype> {
            dictimpl();
            ~dictimpl();
            
            int64 bins() const;
            int64 size() const;

            const bucket<ktype, vtype>* buckets() const;
            bucket<ktype, vtype>* buckets();

            vtype& inswap(uint64_t code, ktype& key, vtype& val);
            const vtype* locate(uint64_t code, const ktype& key) const;
            void reject(uint64_t code, const ktype& key);

            void debug() const;
            private:
                dictimpl(const dictimpl<ktype, vtype, ttype, bsize>&); // = delete
                void operator =(const dictimpl<ktype, vtype, ttype, bsize>&); // = delete

                // appends key:val to the store, then updates table and count
                vtype& insert(int64 probe, uint64_t code, ktype& key, vtype& val);

                // shuffles the table to make room at the probe location
                void robinhood(int64 probe);
            
                int64 count;
                struct { ttype index, trunc; } table[bsize];
                // placed in a union so they are not constructed
                union { bucket<ktype, vtype> store[3*bsize/4]; };
        };

        template<class ktype, class vtype, class ttype, uint64_t bsize>
        dictimpl<ktype, vtype, ttype, bsize>::dictimpl() : count(0) {
            const ttype sentinel = (ttype)-1ULL;
            for (int64 ii = 0; ii<bsize; ii++) {
                table[ii].index = sentinel;
            }
        }

        template<class ktype, class vtype, class ttype, uint64_t bsize>
        dictimpl<ktype, vtype, ttype, bsize>::~dictimpl() {
            for (int64 ii = 0; ii<count; ii++) {
                store[ii].~bucket<ktype, vtype>();
            }
        }
        
        template<class ktype, class vtype, class ttype, uint64_t bsize>
        int64 dictimpl<ktype, vtype, ttype, bsize>::bins() const {
            return bsize;
        }

        template<class ktype, class vtype, class ttype, uint64_t bsize>
        int64 dictimpl<ktype, vtype, ttype, bsize>::size() const {
            return count;
        }

        template<class ktype, class vtype, class ttype, uint64_t bsize>
        const bucket<ktype, vtype>* dictimpl<ktype, vtype, ttype, bsize>::buckets() const {
            return store;
        }

        template<class ktype, class vtype, class ttype, uint64_t bsize>
        bucket<ktype, vtype>* dictimpl<ktype, vtype, ttype, bsize>::buckets() {
            return store;
        }

        template<class ktype, class vtype, class ttype, uint64_t bsize>
        vtype& dictimpl<ktype, vtype, ttype, bsize>::inswap(
            uint64_t code, ktype& key, vtype& val
        ) {
            const ttype sentinel = (ttype)-1ULL;
            ttype truncode = code%bsize;
            int64 probe = code%bsize;
            for (int64 newcost = 0; newcost<bsize; newcost++) {
                if (table[probe].index == sentinel) {
                    // found an empty spot
                    return insert(probe, code, key, val);
                }

                if (table[probe].trunc == truncode) {
                    // the truncated hash codes match, check the store
                    bucket<ktype, vtype>& occupant = store[table[probe].index];
                    if (occupant.code == code && occupant.key == key) {
                        // matching key, replace val
                        xm::swap(occupant.val, val);
                        return occupant.val;
                    }
                }

                int64 oldcost = (probe - table[probe].trunc)%bsize;
                if (newcost >= oldcost) {
                    // we've searched far enough that we will not
                    // find our key, and we can steal this spot
                    robinhood(probe);
                    return insert(probe, code, key, val);
                }
                probe = (probe + 1)%bsize;
            }
            check(false, "should never get here");
            return store[0].val; // this is invalid
        }

        template<class ktype, class vtype, class ttype, uint64_t bsize>
        vtype& dictimpl<ktype, vtype, ttype, bsize>::insert(
            int64 probe, uint64_t code, ktype& key, vtype& val
        ) {
            ttype truncode = code%bsize;
            table[probe].index = (ttype)count;
            table[probe].trunc = truncode;
            store[count].code = code;
            new(&store[count].key) ktype;
            new(&store[count].val) vtype;
            xm::swap(store[count].key, key);
            xm::swap(store[count].val, val);
            return store[count++].val;
        }

        template<class ktype, class vtype, class ttype, uint64_t bsize>
        void dictimpl<ktype, vtype, ttype, bsize>::robinhood(int64 probe) {
            const ttype sentinel = (ttype)-1ULL;
            ttype oldtrunc = table[probe].trunc;
            ttype oldindex = table[probe].index;
            int64 oldcost = (probe - oldtrunc)%bsize;

            for (int64 ii = 0; ii<bsize; ii++) {
                probe = (probe + 1)%bsize;
                ttype& newtrunc = table[probe].trunc;
                ttype& newindex = table[probe].index;
                if (newindex == sentinel) {
                    // XXX: update store[probe].index;
                    newtrunc = oldtrunc;
                    newindex = oldindex;
                    return;
                }
                ++oldcost;
                int64 newcost = (probe - newtrunc)%bsize;
                if (oldcost >= newcost) {
                    // XXX: update store[probe].index;
                    xm::swap(oldtrunc, newtrunc);
                    xm::swap(oldindex, newindex);
                    xm::swap(oldcost, newcost);
                }
            }
            check(false, "should never get here");
        }

        template<class ktype, class vtype, class ttype, uint64_t bsize>
        const vtype* dictimpl<ktype, vtype, ttype, bsize>::locate(
            uint64_t code, const ktype& key
        ) const {
            const ttype sentinel = (ttype)-1ULL;
            ttype truncode = code%bsize;
            int64 probe = code%bsize;
            for (int64 newcost = 0; newcost<bsize; newcost++) {
                if (table[probe].index == sentinel) {
                    // it is not in the table
                    return 0;
                }
                
                if (table[probe].trunc == truncode) {
                    // the truncated hash codes match, check the store
                    const bucket<ktype, vtype>& occupant = store[table[probe].index];
                    if (occupant.code == code && occupant.key == key) {
                        // matching key, return pointer to val
                        return &occupant.val;
                    }
                }

                int64 oldcost = (probe - table[probe].trunc)%bsize;
                if (newcost > oldcost) {
                    // we won't find it after this point
                    return 0;
                }
                probe = (probe + 1)%bsize;
            }

            check(false, "should never get here");
            return 0;
        }

        template<class ktype, class vtype, class ttype, uint64_t bsize>
        void dictimpl<ktype, vtype, ttype, bsize>::reject(
            uint64_t code, const ktype& key
        ) {
            (void)code; (void)key;
        }

        template<class ktype, class vtype, class ttype, uint64_t bsize>
        void dictimpl<ktype, vtype, ttype, bsize>::debug() const {
            const ttype sentinel = (ttype)-1ULL;
            for (int64 row = 0; row<bsize/4; row++) {
                for (int64 col = 0; col<4; col++) {
                    int64 ii = row + col*bsize/4;
                    int64 cost = (ii - table[ii].trunc)%bsize;
                    if (table[ii].index == sentinel) {
                        printf("%-3lld =========================== | ", ii);
                    } else {
                        printf(
                            "%-3lld wnt: %3lld, ind: %3lld, cst: %2lld | ",
                            ii, (int64)table[ii].trunc, (int64)table[ii].index, cost
                        );
                    }
                }
                printf("\n");
            }
            /*
            printf("\n---\n");
            for (int64 ii = 0; ii<count; ii++) {
                printf(
                    "index: %4lld, key: %4lld, val: %4lld\n",
                    ii, (int64)store[ii].key, (int64)store[ii].val
                );
                if (ii%4 == 3) printf("\n");
            }
            */
            printf("\n===========================\n");
        }

        //{{{ makedict
        template<class ktype, class vtype>
        dictbase<ktype, vtype>* makedict(uint64_t size) {
            switch (size) {
                case 1ULL<< 2: return new dictimpl<ktype, vtype,  uint8_t, 1ULL<< 2>();
                case 1ULL<< 3: return new dictimpl<ktype, vtype,  uint8_t, 1ULL<< 3>();
                case 1ULL<< 4: return new dictimpl<ktype, vtype,  uint8_t, 1ULL<< 4>();
                case 1ULL<< 5: return new dictimpl<ktype, vtype,  uint8_t, 1ULL<< 5>();
                case 1ULL<< 6: return new dictimpl<ktype, vtype,  uint8_t, 1ULL<< 6>();
                case 1ULL<< 7: return new dictimpl<ktype, vtype,  uint8_t, 1ULL<< 7>();
                case 1ULL<< 8: return new dictimpl<ktype, vtype,  uint8_t, 1ULL<< 8>();
                case 1ULL<< 9: return new dictimpl<ktype, vtype, uint16_t, 1ULL<< 9>();
                case 1ULL<<10: return new dictimpl<ktype, vtype, uint16_t, 1ULL<<10>();
                case 1ULL<<11: return new dictimpl<ktype, vtype, uint16_t, 1ULL<<11>();
                case 1ULL<<12: return new dictimpl<ktype, vtype, uint16_t, 1ULL<<12>();
                case 1ULL<<13: return new dictimpl<ktype, vtype, uint16_t, 1ULL<<13>();
                case 1ULL<<14: return new dictimpl<ktype, vtype, uint16_t, 1ULL<<14>();
                case 1ULL<<15: return new dictimpl<ktype, vtype, uint16_t, 1ULL<<15>();
                case 1ULL<<16: return new dictimpl<ktype, vtype, uint16_t, 1ULL<<16>();
                case 1ULL<<17: return new dictimpl<ktype, vtype, uint32_t, 1ULL<<17>();
                case 1ULL<<18: return new dictimpl<ktype, vtype, uint32_t, 1ULL<<18>();
                case 1ULL<<19: return new dictimpl<ktype, vtype, uint32_t, 1ULL<<19>();
                case 1ULL<<20: return new dictimpl<ktype, vtype, uint32_t, 1ULL<<20>();
                case 1ULL<<21: return new dictimpl<ktype, vtype, uint32_t, 1ULL<<21>();
                case 1ULL<<22: return new dictimpl<ktype, vtype, uint32_t, 1ULL<<22>();
                case 1ULL<<23: return new dictimpl<ktype, vtype, uint32_t, 1ULL<<23>();
                case 1ULL<<24: return new dictimpl<ktype, vtype, uint32_t, 1ULL<<24>();
                case 1ULL<<25: return new dictimpl<ktype, vtype, uint32_t, 1ULL<<25>();
                case 1ULL<<26: return new dictimpl<ktype, vtype, uint32_t, 1ULL<<26>();
                case 1ULL<<27: return new dictimpl<ktype, vtype, uint32_t, 1ULL<<27>();
                case 1ULL<<28: return new dictimpl<ktype, vtype, uint32_t, 1ULL<<28>();
                case 1ULL<<29: return new dictimpl<ktype, vtype, uint32_t, 1ULL<<29>();
                case 1ULL<<30: return new dictimpl<ktype, vtype, uint32_t, 1ULL<<30>();
                case 1ULL<<31: return new dictimpl<ktype, vtype, uint32_t, 1ULL<<31>();
                case 1ULL<<32: return new dictimpl<ktype, vtype, uint32_t, 1ULL<<32>();
                case 1ULL<<33: return new dictimpl<ktype, vtype, uint64_t, 1ULL<<33>();
                case 1ULL<<34: return new dictimpl<ktype, vtype, uint64_t, 1ULL<<34>();
                case 1ULL<<35: return new dictimpl<ktype, vtype, uint64_t, 1ULL<<35>();
                case 1ULL<<36: return new dictimpl<ktype, vtype, uint64_t, 1ULL<<36>();
                case 1ULL<<37: return new dictimpl<ktype, vtype, uint64_t, 1ULL<<37>();
                case 1ULL<<38: return new dictimpl<ktype, vtype, uint64_t, 1ULL<<38>();
                case 1ULL<<39: return new dictimpl<ktype, vtype, uint64_t, 1ULL<<39>();
                case 1ULL<<40: return new dictimpl<ktype, vtype, uint64_t, 1ULL<<40>();
                case 1ULL<<41: return new dictimpl<ktype, vtype, uint64_t, 1ULL<<41>();
                case 1ULL<<42: return new dictimpl<ktype, vtype, uint64_t, 1ULL<<42>();
                case 1ULL<<43: return new dictimpl<ktype, vtype, uint64_t, 1ULL<<43>();
                case 1ULL<<44: return new dictimpl<ktype, vtype, uint64_t, 1ULL<<44>();
                case 1ULL<<45: return new dictimpl<ktype, vtype, uint64_t, 1ULL<<45>();
                case 1ULL<<46: return new dictimpl<ktype, vtype, uint64_t, 1ULL<<46>();
                case 1ULL<<47: return new dictimpl<ktype, vtype, uint64_t, 1ULL<<47>();
                case 1ULL<<48: return new dictimpl<ktype, vtype, uint64_t, 1ULL<<48>();
                case 1ULL<<49: return new dictimpl<ktype, vtype, uint64_t, 1ULL<<49>();
                case 1ULL<<50: return new dictimpl<ktype, vtype, uint64_t, 1ULL<<50>();
                case 1ULL<<51: return new dictimpl<ktype, vtype, uint64_t, 1ULL<<51>();
                case 1ULL<<52: return new dictimpl<ktype, vtype, uint64_t, 1ULL<<52>();
                case 1ULL<<53: return new dictimpl<ktype, vtype, uint64_t, 1ULL<<53>();
                case 1ULL<<54: return new dictimpl<ktype, vtype, uint64_t, 1ULL<<54>();
                case 1ULL<<55: return new dictimpl<ktype, vtype, uint64_t, 1ULL<<55>();
                case 1ULL<<56: return new dictimpl<ktype, vtype, uint64_t, 1ULL<<56>();
                case 1ULL<<57: return new dictimpl<ktype, vtype, uint64_t, 1ULL<<57>();
                case 1ULL<<58: return new dictimpl<ktype, vtype, uint64_t, 1ULL<<58>();
                case 1ULL<<59: return new dictimpl<ktype, vtype, uint64_t, 1ULL<<59>();
                case 1ULL<<60: return new dictimpl<ktype, vtype, uint64_t, 1ULL<<60>();
                default: break;
            }
            check(false, "internal error bins: %lld\n", (int64)size);
            return 0;
        }
        //}}}

    }

    template<class ktype, class vtype>
    struct newdict {
        ~newdict();
        newdict();
        newdict(const newdict<ktype, vtype>& other);

        template<class kk, class vv>
        newdict(const newdict<kk, vv>& other);

        newdict<ktype, vtype>& operator =(
            const newdict<ktype, vtype>& other
        );

        template<class kk, class vv>
        newdict<ktype, vtype>& operator =(
            const newdict<kk, vv>& other
        );

        const vtype& operator [](const ktype& key) const;
        vtype& operator [](const ktype& key);

        void insert(const ktype& key, const vtype& val);
        void insert(const ktype& key); // val is default
        void remove(const ktype& key);
        bool haskey(const ktype& key) const;

        void reserve(int64 count);
        void shrink();
        void clear();
        int64 size() const;

        const ktype& key(int64 index) const;
        const vtype& val(int64 index) const;
        vtype& val(int64 index);

        list<ktype> keys() const;
        list<vtype> vals() const;

        void debug() const { if (storage) storage->debug(); }
        private:
            // updates or inserts key:val and returns
            // reference to val (this may resize and rehash)
            vtype& inject(uint64_t code, ktype key, vtype val);

            // makes a new storage of the requested size
            // and moves all elements to the new storage
            // (callers must ensure it is big enough)
            void rehash(int64 bins);

            // replaces current storage and copies other
            template<class kk, class vv>
            void assign(internal::dictbase<kk, vv>* other);

            internal::dictbase<ktype, vtype>* storage;
    };

    template<class ktype, class vtype>
    newdict<ktype, vtype>::~newdict() {
        if (storage) delete storage;
    }

    template<class ktype, class vtype>
    newdict<ktype, vtype>::newdict() : storage(0) {}

    template<class ktype, class vtype>
    newdict<ktype, vtype>::newdict(
        const newdict<ktype, vtype>& other
    ) : storage(0) { assign(other.storage); }

    template<class ktype, class vtype>
    template<class kk, class vv>
    newdict<ktype, vtype>::newdict(
        const newdict<kk, vv>& other
    ) : storage(0) { assign(other.storage); }

    template<class ktype, class vtype>
    newdict<ktype, vtype>& newdict<ktype, vtype>::operator =(
        const newdict<ktype, vtype>& other
    ) { assign(other.storage); }

    template<class ktype, class vtype>
    template<class kk, class vv>
    newdict<ktype, vtype>& newdict<ktype, vtype>::operator =(
        const newdict<kk, vv>& other
    ) { assign(other.storage); }

    /* XXX: Rethink these ones.  Should a const dict be
            modified when the key is not found?
            Return option?  Throw exception?

    template<class ktype, class vtype>
    const vtype& newdict<ktype, vtype>::operator [](
        const ktype& key
    ) const {
        using namespace internal;
        uint64_t code = hash(key);
        if (!storage) {
            storage = makedict<ktype, vtype>(4);
            return storage->inswap(code, ktype(key), vtype());
        }
        vtype* result = storage->locate(code, key);
        if (!result) {
            return inject(code, key, vtype());
        }
        return result;
    }

    template<class ktype, class vtype>
    vtype& newdict<ktype, vtype>::operator [](
        const ktype& key
    ) {
        // dispatch to the const version
        // then cast-away the constness
        return (vtype&)operator [](key);
    }
    */

    template<class ktype, class vtype>
    void newdict<ktype, vtype>::insert(
        const ktype& key, const vtype& val
    ) { inject(hash(key), key, val); }

    template<class ktype, class vtype>
    void newdict<ktype, vtype>::insert(
        const ktype& key
    ) { inject(hash(key), key, vtype()); }

    template<class ktype, class vtype>
    void newdict<ktype, vtype>::remove(
        const ktype& key
    ) {
        if (storage) {
            storage->reject(hash(key), key);
        }
    }

    template<class ktype, class vtype>
    bool newdict<ktype, vtype>::haskey(
        const ktype& key
    ) const {
        if (!storage) return false;
        return 0 != storage->locate(hash(key), key);
    }

    template<class ktype, class vtype>
    void newdict<ktype, vtype>::reserve(
        int64 count
    ) {
        int64 len = max(size(), count);
        if (len == 0) {
            if (storage) {
                delete storage;
                storage = 0;
            }
            return;
        }
        int64 bins = 4;
        while (3*bins/4 < len) {
            bins *= 2;
        }
        if (!storage || bins != storage->bins()) {
            rehash(bins);
        }
    }
    
    template<class ktype, class vtype>
    void newdict<ktype, vtype>::shrink() {
        reserve(0);
    }

    template<class ktype, class vtype>
    void newdict<ktype, vtype>::clear() {
        if (storage) {
            delete storage;
            storage = 0;
        }
    }

    template<class ktype, class vtype>
    int64 newdict<ktype, vtype>::size() const {
        return storage ? storage->size() : 0;
    }

    template<class ktype, class vtype>
    const ktype& newdict<ktype, vtype>::key(int64 index) const {
        return storage->buckets()[index].key;
    }

    template<class ktype, class vtype>
    const vtype& newdict<ktype, vtype>::val(int64 index) const {
        return storage->buckets()[index].getval();
    }

    template<class ktype, class vtype>
    vtype& newdict<ktype, vtype>::val(int64 index) {
        return storage->buckets()[index].getval();
    }

    template<class ktype, class vtype>
    list<ktype> newdict<ktype, vtype>::keys() const {
        using namespace internal;
        list<ktype> result;
        int64 len = size();
        if (!len) return result;
        bucket<ktype, vtype>* buckets = storage->buckets();
        result.reserve(len);
        for (int64 ii = 0; ii<len; ii++) {
            result.append(buckets[ii].key);
        }
        return result;
    }

    template<class ktype, class vtype>
    list<vtype> newdict<ktype, vtype>::vals() const {
        using namespace internal;
        list<vtype> result;
        int64 len = size();
        if (!len) return result;
        bucket<ktype, vtype>* buckets = storage->buckets();
        result.reserve(len);
        for (int64 ii = 0; ii<len; ii++) {
            result.append(buckets[ii].getval());
        }
        return result;
    }

    template<class ktype, class vtype>
    vtype& newdict<ktype, vtype>::inject(
        uint64_t code, ktype key, vtype val
    ) {
        int64 next_size = size() + 1; // assume key not there
        int64 curr_bins = storage ? storage->bins() : 0;
        int64 next_bins = max(curr_bins, 4);
        while (3*next_bins/4 < next_size) {
            next_bins *= 2;
        }
        if (next_bins != curr_bins) rehash(next_bins);
        return storage->inswap(code, key, val);
    }

    template<class ktype, class vtype>
    void newdict<ktype, vtype>::rehash(int64 bins) {
        using namespace internal;
        dictbase<ktype, vtype>* oldstore = storage;
        storage = makedict<ktype, vtype>(bins);
        if (!oldstore) return;
        int64 len = oldstore->size();
        bucket<ktype, vtype>* buckets = oldstore->buckets();
        for (int64 ii = 0; ii<len; ii++) {
            storage->inswap(
                buckets[ii].code,
                buckets[ii].key,
                buckets[ii].val
            );
        }
        delete oldstore;
    }

    template<class ktype, class vtype>
    template<class kk, class vv>
    void newdict<ktype, vtype>::assign(
        internal::dictbase<kk, vv>* other
    ) {
        // XXX: There're lots of opportunities for optimization in here.  We
        // could re-use our existing storage if it is large enough.  We could
        // make storage only as big as it needs to be.  We could re-use the
        // hash code if the keys are the same type.
        using namespace internal;
        if (storage) {
            delete storage;
            storage = 0;
        }
        if (other) {
            storage = makedict<ktype, vtype>(other.bins());
            int64 len = other.size();
            bucket<kk, vv>* buckets = other->buckets();
            for (int64 ii = 0; ii<len; ii++) {
                ktype key = ktype(buckets[ii].key);
                storage.inswap(hash(key), key, vtype(buckets[ii].val));
            }
        }
    }


}

int main() {
    using namespace xm;
    newdict<int32_t, int32_t> dd;
    for (int ii = 0; ii<1000; ii++) {
        dd.insert(ii*ii, (ii + 1)*1001);
        dd.debug();
        getchar();
    }

    return 0;
}

