#ifndef XM_DICT_H_
#define XM_DICT_H_ 1

#include "basics.h"
#include "hash.h"

namespace xm {
    // A sentinel type for dict sets with only keys (no vals)
    struct none {};

    //{{{ internal
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
            virtual bucket<ktype, vtype>* locate(uint64_t code, const ktype& key) = 0;
            virtual void reject(uint64_t code, const ktype& key) = 0;

            virtual void debug() const = 0;
            virtual void test() const = 0;
        };
        //}}}
        //{{{ dictimpl
        template<class ktype, class vtype, class htype>
        struct dictimpl : dictbase<ktype, vtype> {
            dictimpl(uint64_t tsize);
            ~dictimpl();
            
            int64 bins() const;
            int64 size() const;

            const bucket<ktype, vtype>* buckets() const;
            bucket<ktype, vtype>* buckets();

            vtype& inswap(uint64_t code, ktype& key, vtype& val);
            bucket<ktype, vtype>* locate(uint64_t code, const ktype& key);
            void reject(uint64_t code, const ktype& key);

            void debug() const;
            void test() const;
            private:
                dictimpl(); // = delete
                dictimpl(const dictimpl<ktype, vtype, htype>&); // = delete
                void operator =(const dictimpl<ktype, vtype, htype>&); // = delete

                // appends key:val to the store, then updates table and count
                vtype& insert(int64 tspot, uint64_t code, ktype& key, vtype& val);

                // shuffles the table to make room at the location
                void robinhood(int64 tspot);

                // shuffles the table to occupy a recently removed hole
                void backshift(int64 tspot);
            
                uint64_t count;
                const uint64_t tsize;
                static const htype sentinel = (htype)-1ULL;
                struct ttype { htype index, trunc; };
                // We're using placement new to allocated extra storage
                // after the end of this struct.  The following fields
                // would be here:
                // ttype table[tsize];
                // bucket<ktype, vtype> store[3*tsize/4];
        };

        template<class ktype, class vtype, class htype>
        dictimpl<ktype, vtype, htype>::dictimpl(uint64_t tsize) :
            count(0), tsize(tsize)
        {
            ttype* table = (ttype*)(void*)(this + 1);

            for (uint64_t ii = 0; ii<tsize; ii++) {
                table[ii].index = sentinel;
            }
        }

        template<class ktype, class vtype>
        void destruct(bucket<ktype, vtype>* store, int64 count) {
            for (int64 ii = 0; ii<count; ii++) {
                store[ii].key.~ktype();
                store[ii].val.~vtype();
            }
        }

        template<class ktype>
        void destruct(bucket<ktype, none>* store, int64 count) {
            for (int64 ii = 0; ii<count; ii++) {
                store[ii].key.~ktype();
            }
        }

        template<class ktype, class vtype, class htype>
        dictimpl<ktype, vtype, htype>::~dictimpl() {
            ttype* table = (ttype*)(void*)(this + 1);
            bucket<ktype, vtype>* store = (
                (bucket<ktype,vtype>*)(void*)(table + tsize)
            );
            destruct(store, count);
        }
        
        template<class ktype, class vtype, class htype>
        int64 dictimpl<ktype, vtype, htype>::bins() const {
            return tsize;
        }

        template<class ktype, class vtype, class htype>
        int64 dictimpl<ktype, vtype, htype>::size() const {
            return count;
        }

        template<class ktype, class vtype, class htype>
        const bucket<ktype, vtype>* dictimpl<ktype, vtype, htype>::buckets() const {
            const ttype* table = (const ttype*)(void*)(this + 1);
            const bucket<ktype, vtype>* store = (
                (const bucket<ktype,vtype>*)(const void*)(table + tsize)
            );
            return store;
        }

        template<class ktype, class vtype, class htype>
        bucket<ktype, vtype>* dictimpl<ktype, vtype, htype>::buckets() {
            ttype* table = (ttype*)(void*)(this + 1);
            bucket<ktype, vtype>* store = (
                (bucket<ktype,vtype>*)(void*)(table + tsize)
            );
            return store;
        }

        template<class ktype, class vtype, class htype>
        vtype& dictimpl<ktype, vtype, htype>::inswap(
            uint64_t code, ktype& key, vtype& val
        ) {
            ttype* table = (ttype*)(void*)(this + 1);
            bucket<ktype, vtype>* store = (
                (bucket<ktype,vtype>*)(void*)(table + tsize)
            );
            const uint64_t tmask = tsize - 1;
            htype truncode = code&tmask;
            int64 tspot = code&tmask;
            for (uint64_t newcost = 0; newcost<tsize; newcost++) {
                if (table[tspot].index == sentinel) {
                    // found an empty spot
                    return insert(tspot, code, key, val);
                }

                if (table[tspot].trunc == truncode) {
                    // the truncated hash codes match, check the store
                    bucket<ktype, vtype>& occupant = store[table[tspot].index];
                    if (occupant.code == code && occupant.key == key) {
                        // matching key, replace val
                        // argument dependent lookup
                        using namespace xm;
                        swap(occupant.getval(), val);
                        return occupant.getval();
                    }
                }

                uint64_t oldcost = (tspot - table[tspot].trunc)&tmask;
                if (newcost >= oldcost) {
                    // we've searched far enough that we will not
                    // find our key, and we can steal this spot
                    robinhood(tspot);
                    return insert(tspot, code, key, val);
                }
                tspot = (tspot + 1)&tmask;
            }
            check(false, "should never get here");
            return store[0].getval(); // this is invalid
        }

        template<class ktype, class vtype>
        void newandswap(bucket<ktype, vtype>& bucket, ktype& key, vtype& val) {
            new(&bucket.key) ktype();
            new(&bucket.val) vtype();
            // argument dependent lookup
            using namespace xm;
            swap(bucket.key, key);
            swap(bucket.val, val);
        }

        template<class ktype>
        void newandswap(bucket<ktype, none>& bucket, ktype& key, none&) {
            new(&bucket.key) ktype();
            // argument dependent lookup
            using namespace xm;
            swap(bucket.key, key);
        }

        template<class ktype, class vtype, class htype>
        vtype& dictimpl<ktype, vtype, htype>::insert(
            int64 tspot, uint64_t code, ktype& key, vtype& val
        ) {
            ttype* table = (ttype*)(void*)(this + 1);
            bucket<ktype, vtype>* store = (
                (bucket<ktype,vtype>*)(void*)(table + tsize)
            );
            const uint64_t tmask = tsize - 1;
            htype truncode = code&tmask;
            table[tspot].index = (htype)count;
            table[tspot].trunc = truncode;
            store[count].index = tspot;
            store[count].code = code;
            newandswap(store[count], key, val);
            return store[count++].getval();
        }

        template<class ktype, class vtype, class htype>
        void dictimpl<ktype, vtype, htype>::robinhood(int64 tspot) {
            ttype* table = (ttype*)(void*)(this + 1);
            bucket<ktype, vtype>* store = (
                (bucket<ktype,vtype>*)(void*)(table + tsize)
            );
            const uint64_t tmask = tsize - 1;
            htype oldtrunc = table[tspot].trunc;
            htype oldindex = table[tspot].index;
            uint64_t oldcost = (tspot - oldtrunc)&tmask;

            for (uint64_t ii = 0; ii<tsize; ii++) {
                tspot = (tspot + 1)&tmask;
                htype& newtrunc = table[tspot].trunc;
                htype& newindex = table[tspot].index;
                if (newindex == sentinel) {
                    store[oldindex].index = tspot;
                    newtrunc = oldtrunc;
                    newindex = oldindex;
                    return;
                }
                ++oldcost;
                uint64_t newcost = (tspot - newtrunc)&tmask;
                if (oldcost >= newcost) {
                    store[oldindex].index = tspot;
                    // argument dependent lookup
                    using namespace xm;
                    swap(oldtrunc, newtrunc);
                    swap(oldindex, newindex);
                    swap(oldcost, newcost);
                }
            }
            check(false, "should never get here");
        }

        template<class ktype, class vtype, class htype>
        bucket<ktype, vtype>* dictimpl<ktype, vtype, htype>::locate(
            uint64_t code, const ktype& key
        ) {
            ttype* table = (ttype*)(void*)(this + 1);
            bucket<ktype, vtype>* store = (
                (bucket<ktype,vtype>*)(void*)(table + tsize)
            );
            const uint64_t tmask = tsize - 1;
            htype truncode = code&tmask;
            int64 tspot = code&tmask;
            for (uint64_t newcost = 0; newcost<tsize; newcost++) {
                if (table[tspot].index == sentinel) {
                    // it is not in the table
                    return 0;
                }
                
                if (table[tspot].trunc == truncode) {
                    // the truncated hash codes match, check the store
                    bucket<ktype, vtype>& occupant = store[table[tspot].index];
                    if (occupant.code == code && occupant.key == key) {
                        // matching key, return pointer to val
                        return &occupant;
                    }
                }

                uint64_t oldcost = (tspot - table[tspot].trunc)&tmask;
                if (newcost > oldcost) {
                    // we won't find it after this point
                    return 0;
                }
                tspot = (tspot + 1)&tmask;
            }

            check(false, "should never get here");
            return 0;
        }

        template<class ktype, class vtype, class htype>
        void dictimpl<ktype, vtype, htype>::reject(
            uint64_t code, const ktype& key
        ) {
            ttype* table = (ttype*)(void*)(this + 1);
            bucket<ktype, vtype>* store = (
                (bucket<ktype,vtype>*)(void*)(table + tsize)
            );
            (void)code; (void)key;
            bucket<ktype, vtype>* found = locate(code, key);
            if (!found) return;
            --count;
            int64 tspot = found->index;
            int64 sspot = table[tspot].index;
            // swap/move the last store item to sspot
            store[sspot].index = store[count].index;
            store[sspot].code = store[count].code;
            {
                // argument dependent lookup
                using namespace xm;
                swap(found->key, store[count].key);
                swap(found->getval(), store[count].getval());
            }
            table[store[sspot].index].index = sspot;

            // destruct the item we moved to the end
            destruct(store + count, 1);

            backshift(tspot);
        }

        template<class ktype, class vtype, class htype>
        void dictimpl<ktype, vtype, htype>::backshift(int64 tspot) {
            ttype* table = (ttype*)(void*)(this + 1);
            bucket<ktype, vtype>* store = (
                (bucket<ktype,vtype>*)(void*)(table + tsize)
            );
            const uint64_t tmask = tsize - 1;
            for (uint64_t ii = 0; ii<tsize; ii++) {
                htype next = (tspot + 1)&tmask;
                if (table[next].index == sentinel) {
                    table[tspot].index = sentinel;
                    return;
                }
                int64 cost = (next - table[next].trunc)&tmask;
                if (cost == 0) {
                    table[tspot].index = sentinel;
                    return;
                }
                table[tspot].index = table[next].index;
                table[tspot].trunc = table[next].trunc;
                store[table[tspot].index].index = tspot;

                tspot = next;
            }
            check(false, "should never get here");
        }

        template<class ktype, class vtype, class htype>
        void dictimpl<ktype, vtype, htype>::debug() const {
            ttype* table = (ttype*)(void*)(this + 1);
            /*
            bucket<ktype, vtype>* store = (
                (bucket<ktype,vtype>*)(void*)(table + tsize)
            );
            */
            const uint64_t tmask = tsize - 1;
            for (uint64_t row = 0; row<tsize/4; row++) {
                for (int64 col = 0; col<4; col++) {
                    int64 ii = row + col*tsize/4;
                    int64 cost = (ii - table[ii].trunc)&tmask;
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

        template<class ktype, class vtype, class htype>
        void dictimpl<ktype, vtype, htype>::test() const {
            ttype* table = (ttype*)(void*)(this + 1);
            bucket<ktype, vtype>* store = (
                (bucket<ktype,vtype>*)(void*)(table + tsize)
            );
            const uint64_t tmask = tsize - 1;
            for (uint64_t ii = 0; ii<count; ii++) {
                check(
                    table[store[ii].index].index == ii,
                    "table matches store"
                );
            }
            uint64_t used = 0;
            for (uint64_t ii = 0; ii<tsize; ii++) {
                if (table[ii].index != sentinel) used++;
            }
            check(used == count, "table count equals store count");
            for (uint64_t ii = 0; ii<tsize; ii++) {
                if (table[ii].index != sentinel) {
                    check(
                        store[table[ii].index].index == ii,
                        "store matches table"
                    );
                }
            }
            for (uint64_t ii = 0; ii<tsize; ii++) {
                htype curr = ((htype)ii + 0)&tmask;
                htype next = ((htype)ii + 1)&tmask;
                int64 currcost = (curr - table[curr].trunc)&tmask;
                int64 nextcost = (next - table[next].trunc)&tmask;
                if (table[curr].index == sentinel) currcost = 0;
                if (table[next].index == sentinel) nextcost = 0;
                check(nextcost <= currcost + 1, "costs correct");
            }
        }
        //}}}
        //{{{ makedict
        template<class ktype, class vtype, class htype>
        dictimpl<ktype, vtype, htype>* newdict(int64 tsize) {
            int64 extra = (
                2*tsize*sizeof(htype) +
                3*tsize/4*sizeof(bucket<ktype, vtype>)
            );
            dictimpl<ktype, vtype, htype>* result = (
                alloc<dictimpl<ktype, vtype, htype> >(extra)
            );
            new(result) dictimpl<ktype, vtype, htype>(tsize);
            return result;
        }

        template<class ktype, class vtype>
        void destroydict(dictbase<ktype, vtype>* ptr) {
            ptr->~dictbase<ktype, vtype>();
            ::free(ptr);
        }

        template<class ktype, class vtype>
        dictbase<ktype, vtype>* makedict(uint64_t size) {
            if (size <= 1ULL<<8)  return newdict<ktype, vtype,  uint8_t>(size);
            if (size <= 1ULL<<16) return newdict<ktype, vtype, uint16_t>(size);
            if (size <= 1ULL<<32) return newdict<ktype, vtype, uint32_t>(size);
            return newdict<ktype, vtype, uint64_t>(size);
        }
        //}}}

    }
    //}}}

    template<class ktype, class vtype=none>
    struct dict {
        ~dict();
        dict();
        dict(const dict<ktype, vtype>& other);

        template<class kk, class vv>
        dict(const dict<kk, vv>& other);

        dict<ktype, vtype>& operator =(
            const dict<ktype, vtype>& other
        );

        template<class kk, class vv>
        dict<ktype, vtype>& operator =(
            const dict<kk, vv>& other
        );

        vtype& operator [](const ktype& key);

        void insert(const ktype& key, const vtype& val);
        void insert(const ktype& key); // val is default
        // These two swap (steal) the key and value and
        // do not make a copy.  Use for non-copyable types.
        void inswap(ktype& key, vtype& val);
        void inswap(ktype& key); // val is default
        void remove(const ktype& key);
        bool haskey(const ktype& key) const;
        const vtype* lookup(const ktype& key) const;
        vtype* lookup(const ktype& key);

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
        void test() const { if (storage) storage->test(); }
        private:
            // updates or inserts key:val and returns
            // reference to val (this may resize and rehash)
            vtype& inswap(uint64_t code, ktype& key, vtype& val);

            // makes a new storage of the requested size
            // and moves all elements to the new storage
            // (callers must ensure it is big enough)
            void rehash(int64 bins);

            // replaces current storage and copies other
            template<class kk, class vv>
            void assign(internal::dictbase<kk, vv>* other);

            template<class kk, class vv> friend struct dict;
            template<class kk, class vv> friend void swap(
                dict<kk, vv>& flip, dict<kk, vv>&flop
            );

            internal::dictbase<ktype, vtype>* storage;
    };

    template<class ktype, class vtype>
    dict<ktype, vtype>::~dict() {
        using namespace internal;
        if (storage) destroydict(storage);
    }

    template<class ktype, class vtype>
    dict<ktype, vtype>::dict() : storage(0) {}

    template<class ktype, class vtype>
    dict<ktype, vtype>::dict(
        const dict<ktype, vtype>& other
    ) : storage(0) { assign(other.storage); }

    template<class ktype, class vtype>
    template<class kk, class vv>
    dict<ktype, vtype>::dict(
        const dict<kk, vv>& other
    ) : storage(0) { assign(other.storage); }

    template<class ktype, class vtype>
    dict<ktype, vtype>& dict<ktype, vtype>::operator =(
        const dict<ktype, vtype>& other
    ) { assign(other.storage); }

    template<class ktype, class vtype>
    template<class kk, class vv>
    dict<ktype, vtype>& dict<ktype, vtype>::operator =(
        const dict<kk, vv>& other
    ) { assign(other.storage); }

    template<class ktype, class vtype>
    vtype& dict<ktype, vtype>::operator [](
        const ktype& key
    ) {
        using namespace internal;
        uint64_t code = hash(key);
        if (!storage) {
            storage = makedict<ktype, vtype>(4);
            ktype kk = key;
            vtype vv = vtype();
            return storage->inswap(code, kk, vv);
        }
        const bucket<ktype, vtype>* result = storage->locate(code, key);
        if (!result) {
            ktype kk = key; vtype vv = vtype();
            return inswap(code, kk, vv);
        }
        return (vtype&)result->getval();
    }

    template<class ktype, class vtype>
    void dict<ktype, vtype>::insert(
        const ktype& key, const vtype& val
    ) { 
        ktype kk = key; vtype vv = val;
        inswap(hash(key), kk, vv);
    }

    template<class ktype, class vtype>
    void dict<ktype, vtype>::insert(
        const ktype& key
    ) {
        ktype kk = key; vtype vv = vtype();
        inswap(hash(key), kk, vv);
    }

    template<class ktype, class vtype>
    void dict<ktype, vtype>::inswap(
        ktype& key, vtype& val
    ) { 
        inswap(hash(key), key, val);
    }

    template<class ktype, class vtype>
    void dict<ktype, vtype>::inswap(
        ktype& key
    ) { 
        vtype vv = vtype();
        inswap(hash(key), key, vv);
    }

    template<class ktype, class vtype>
    vtype& dict<ktype, vtype>::inswap(
        uint64_t code, ktype& key, vtype& val
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
    void dict<ktype, vtype>::remove(
        const ktype& key
    ) {
        if (storage) {
            storage->reject(hash(key), key);
        }
    }

    template<class ktype, class vtype>
    bool dict<ktype, vtype>::haskey(
        const ktype& key
    ) const {
        return 0 != lookup(key);
    }

    template<class ktype, class vtype>
    const vtype* dict<ktype, vtype>::lookup(
        const ktype& key
    ) const {
        if (!storage) return 0;
        internal::bucket<ktype, vtype>* result = (
            storage->locate(hash(key), key)
        );
        return result ? &result->val : 0;
    }

    template<class ktype, class vtype>
    vtype* dict<ktype, vtype>::lookup(
        const ktype& key
    ) {
        return (vtype*)lookup(key);
    }

    template<class ktype, class vtype>
    void dict<ktype, vtype>::reserve(
        int64 count
    ) {
        int64 len = max(size(), count);
        if (len == 0) {
            if (storage) {
                using namespace internal;
                destroydict(storage);
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
    void dict<ktype, vtype>::shrink() {
        reserve(0);
    }

    template<class ktype, class vtype>
    void dict<ktype, vtype>::clear() {
        if (storage) {
            using namespace internal;
            destroydict(storage);
            storage = 0;
        }
    }

    template<class ktype, class vtype>
    int64 dict<ktype, vtype>::size() const {
        return storage ? storage->size() : 0;
    }

    template<class ktype, class vtype>
    const ktype& dict<ktype, vtype>::key(int64 index) const {
        return storage->buckets()[index].key;
    }

    template<class ktype, class vtype>
    const vtype& dict<ktype, vtype>::val(int64 index) const {
        return storage->buckets()[index].getval();
    }

    template<class ktype, class vtype>
    vtype& dict<ktype, vtype>::val(int64 index) {
        return storage->buckets()[index].getval();
    }

    template<class ktype, class vtype>
    list<ktype> dict<ktype, vtype>::keys() const {
        using namespace internal;
        list<ktype> result;
        int64 len = size();
        if (!len) return result;
        const bucket<ktype, vtype>* buckets = storage->buckets();
        result.reserve(len);
        for (int64 ii = 0; ii<len; ii++) {
            result.append(buckets[ii].key);
        }
        return result;
    }

    template<class ktype, class vtype>
    list<vtype> dict<ktype, vtype>::vals() const {
        using namespace internal;
        list<vtype> result;
        int64 len = size();
        if (!len) return result;
        const bucket<ktype, vtype>* buckets = storage->buckets();
        result.reserve(len);
        for (int64 ii = 0; ii<len; ii++) {
            result.append(buckets[ii].getval());
        }
        return result;
    }

    template<class ktype, class vtype>
    void dict<ktype, vtype>::rehash(int64 bins) {
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
                buckets[ii].getval()
            );
        }
        destroydict(oldstore);
    }

    template<class ktype, class vtype>
    template<class kk, class vv>
    void dict<ktype, vtype>::assign(
        internal::dictbase<kk, vv>* other
    ) {
        // XXX: There're lots of opportunities for optimization in here.  We
        // could re-use our existing storage if it is large enough.  We could
        // make storage only as big as it needs to be.  We could re-use the
        // hash code if the keys are the same type.
        using namespace internal;
        if (storage) {
            destroydict(storage);
            storage = 0;
        }
        if (other) {
            storage = makedict<ktype, vtype>(other->bins());
            int64 len = other->size();
            bucket<kk, vv>* buckets = other->buckets();
            for (int64 ii = 0; ii<len; ii++) {
                ktype key = ktype(buckets[ii].key);
                vtype val = vtyle(buckets[ii].val);
                storage->inswap(hash(key), key, val);
            }
        }
    }

    template<class ktype, class vtype>
    void swap(dict<ktype, vtype>& flip, dict<ktype, vtype>& flop) {
        swap(flip.storage, flop.storage);
    }

}

#endif // XM_DICT_H_

