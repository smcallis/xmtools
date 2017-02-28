
    //{{{ dict 
    // A sentinel type for dict sets with only keys (no vals)
    struct none {};

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

        void insert(const ktype& key);
        void insert(const ktype& key, const vtype& val);
        void remove(const ktype& key);
        bool haskey(const ktype& key) const;

        void reserve(int64 count);
        void shrink();
        void clear();
        int64 size() const;

        list<ktype> keys() const;
        list<vtype> vals() const;

        // Iterating through all keys and vals:
        //
        //    for (int64 ii = 0; ii<dd.bins(); ii++) {
        //        // must skip empty bins
        //        if (dd.skip(ii)) continue;
        //
        //        foo(dd.key(ii));
        //        bar(dd.val(ii));
        //
        //        // can also assign to dd.val(ii)
        //        dd.val(ii) = hmm;    
        //    }
        //
        int64 bins() const;
        bool skip(int64 index) const;
        const ktype& key(int64 index) const;
        const vtype& val(int64 index) const;
        vtype& val(int64 index);

        void debug() const;

        private:
            // general purpose implemenation
            template<class keytype, class valtype>
            struct keyval {
                size_t code;
                keytype key_;
                valtype val_;
                keytype& getkey() { return key_; }
                valtype& getval() { return val_; }
                const keytype& getkey() const { return key_; }
                const valtype& getval() const { return val_; }
                void setval(const valtype& vv) { val_ = vv; }
                keyval() {}
                keyval(
                    size_t cc,
                    const keytype& kk, 
                    const valtype& vv
                ) : code(cc), key_(kk), val_(vv) {}
                ~keyval() { code = 0; }
            };

            // specialization for vtype none
            template<class keytype>
            struct keyval<keytype, none> {
                size_t code;
                keytype key_;
                keytype& getkey() { return key_; }
                none& getval() {
                    static none nil;
                    return nil;
                }
                const keytype& getkey() const { return key_; }
                const none& getval() const {
                    static none nil;
                    return nil;
                }
                void setval(const none& vv) { (void)vv; }
                keyval() {}
                keyval(
                    size_t cc,
                    const keytype& kk, 
                    const none&
                ) : code(cc), key_(kk) {}
                ~keyval() { code = 0; }
            };

            template<class keytype, class valtype>
            static void trade(keyval<keytype, valtype>& flip, keyval<keytype, valtype>& flop) {
                swap(flip.code, flop.code);
                swap(flip.key_, flop.key_);
                swap(flip.val_, flop.val_);
            }

            template<class keytype>
            static void trade(keyval<keytype, none>& flip, keyval<keytype, none>& flop) {
                swap(flip.code, flop.code);
                swap(flip.key_, flop.key_);
            }

            enum { HIGHBIT = (((size_t)1) << (8*sizeof(size_t) - 1)) };
            template<class kk, class vv>
            void assign(const dict<kk, vv>& other);

            void rehash(const int64 capacity);
            int64 inject(const ktype& key, const vtype* ptr, size_t hh);
            // using Robin Hood collision resolution, searching, and backshifting
            void robinhood(
                int64 index, keyval<ktype, vtype>& kv, int64 cost
            );
            int64 search(const ktype& key, size_t hh) const;
            void backshift(int64 index);

            template<class kk, class vv> friend struct dict;
            //template<class kk, class vv> friend struct iter;
            template<class kk, class vv> friend void swap(
                dict<kk, vv>& flip, dict<kk, vv>&flop
            );

            struct implementation {
                int64 quantity, capacity;
                keyval<ktype, vtype> bucket[1];
            } *storage;
    };

    template<class ktype, class vtype>
    dict<ktype, vtype>::~dict() { clear(); }

    template<class ktype, class vtype>
    dict<ktype, vtype>::dict() : storage(0) {}

    template<class ktype, class vtype>
    dict<ktype, vtype>::dict(const dict<ktype, vtype>& other) : storage(0) {
        assign(other);
    }

    template<class ktype, class vtype>
    template<class kk, class vv>
    dict<ktype, vtype>::dict(const dict<kk, vv>& other) : storage(0) {
        assign(other);
    }

    template<class ktype, class vtype>
    dict<ktype, vtype>& dict<ktype, vtype>::operator =(
        const dict<ktype, vtype>& other
    ) {
        if (this == &other) return *this;
        clear();
        assign(other);
    }

    template<class ktype, class vtype>
    template<class kk, class vv>
    dict<ktype, vtype>& dict<ktype, vtype>::operator =(
        const dict<kk, vv>& other
    ) {
        clear();
        assign(other);
    }

    template<class ktype, class vtype>
    vtype& dict<ktype, vtype>::operator [](const ktype& key) {
        if (!storage) rehash(4);
        size_t hh = hash(key) | HIGHBIT;
        int64 index = search(key, hh);
        if (index < 0) index = inject(key, 0, hh);
        return storage->bucket[index].getval();
    }

    template<class ktype, class vtype>
    void dict<ktype, vtype>::insert(const ktype& key) {
        size_t hh = hash(key) | HIGHBIT;
        inject(key, 0, hh);
    }

    template<class ktype, class vtype>
    void dict<ktype, vtype>::insert(const ktype& key, const vtype& val) {
        size_t hh = hash(key) | HIGHBIT;
        inject(key, &val, hh);
    }

    template<class ktype, class vtype>
    void dict<ktype, vtype>::remove(const ktype& key) {
        if (!storage) return;
        size_t hh = hash(key) | HIGHBIT;
        int64 index = search(key, hh);
        if (index >= 0) backshift(index);
    }

    template<class ktype, class vtype>
    bool dict<ktype, vtype>::haskey(const ktype& key) const {
        if (!storage) return false;
        size_t hh = hash(key) | HIGHBIT;
        return search(key, hh) >= 0;
    }

    template<class ktype, class vtype>
    void dict<ktype, vtype>::reserve(int64 count) {
        check(count > 0, "must reserve a positive count: %lld", count);
        int64 newsize = 4;
        while (3*newsize/4 < count) newsize *= 2;
        if (!storage || storage->capacity < newsize) rehash(newsize);
    }

    template<class ktype, class vtype>
    void dict<ktype, vtype>::shrink() {
        if (!storage) return;
        const int64 len = storage->quantity;
        if (storage->quantity == 0) {
            delete storage;
            storage = 0;
        } else {
            int64 newsize = 4;
            while (3*newsize/4 < len) newsize *= 2;
            if (newsize == storage->capacity) return;
            rehash(newsize);
        }
    }

    template<class ktype, class vtype>
    void dict<ktype, vtype>::clear() {
        if (storage) {
            const int64 len = storage->capacity;
            for (int64 ii = 0; ii<len; ii++) {
                if (storage->bucket[ii].code) {
                    storage->bucket[ii].~keyval<ktype, vtype>();
                }
            }
            free(storage);
            storage = 0;
        }
    }

    template<class ktype, class vtype>
    int64 dict<ktype, vtype>::size() const {
        return storage ? storage->quantity : 0;
    }

    template<class ktype, class vtype>
    list<ktype> dict<ktype, vtype>::keys() const {
        list<ktype> result;
        if (storage) {
            const int64 len = storage->capacity;
            result.reserve(storage->quantity);
            for (int64 ii = 0; ii<len; ii++) {
                if (storage->bucket[ii].code) {
                    result.append(storage->bucket[ii].getkey());
                }
            }
        }
        return result;
    }

    template<class ktype, class vtype>
    list<vtype> dict<ktype, vtype>::vals() const {
        list<vtype> result;
        if (storage) {
            const int64 len = storage->capacity;
            result.reserve(storage->quantity);
            for (int64 ii = 0; ii<len; ii++) {
                if (storage->bucket[ii].code) {
                    result.append(storage->bucket[ii].getval());
                }
            }
        }
        return result;
    }

    template<class ktype, class vtype>
    int64 dict<ktype, vtype>::bins() const {
        return storage ? storage->capacity : 0;
    }

    template<class ktype, class vtype>
    bool dict<ktype, vtype>::skip(int64 index) const {
        return storage->bucket[index].code == 0;
    }

    template<class ktype, class vtype>
    const ktype& dict<ktype, vtype>::key(int64 index) const {
        return storage->bucket[index].getkey();
    }

    template<class ktype, class vtype>
    const vtype& dict<ktype, vtype>::val(int64 index) const {
        return storage->bucket[index].getval();
    }

    template<class ktype, class vtype>
    vtype& dict<ktype, vtype>::val(int64 index) {
        return storage->bucket[index].getval();
    }

    template<class ktype, class vtype>
    void dict<ktype, vtype>::debug() const {
        if (storage) {
            printf("quantity: %lld, capacity: %lld\n", storage->quantity, storage->capacity);
            for (int64 ii = 0; ii<storage->capacity/4; ii++) {
                for (int64 jj = 0; jj<4; jj++) {
                    int64 kk = jj*storage->capacity/4 + ii;
                    if (storage->bucket[kk].code) {
                        int64 want = storage->bucket[kk].code & (storage->capacity - 1);
                        int64 cost = kk - want;
                        if (cost < 0) cost += storage->capacity;
                        printf(
                            "%3lld: [%16zx (%3lld): %8ld -> %-8ld], ",
                            kk, storage->bucket[kk].code, cost,
                            storage->bucket[kk].getkey(),
                            storage->bucket[kk].getval()
                        );
                    } else {
                        printf("%3lld: ----------------------------------------------, ", kk);
                    }
                }
                printf("\n");
            }
        } else {
            printf("empty\n");
        }
    }

    template<class ktype, class vtype>
    template<class kk, class vv>
    void dict<ktype, vtype>::assign(const dict<kk, vv>& other) {
        if (other.storage) {
            int64 cap = other.storage->capacity;
            storage = alloc<implementation>((cap - 1)*sizeof(keyval<ktype, vtype>));
            storage->capacity = cap;
            for (int64 ii = 0; ii<cap; ii++) {
                storage->bucket[ii].code = 0;
            }
            for (int64 ii = 0; ii<cap; ii++) {
                if (other.storage->bucket[ii].code) {
                    keyval<ktype, vtype> kv(0,
                        other.storage->bucket[ii].getkey(),
                        other.storage->bucket[ii].getval()
                    );
                    // XXX: should copy the old hash code if we have the same keytype
                    kv.code = hash(kv.getkey()) | HIGHBIT;
                    int64 newspot = kv.code&(cap-1);
                    robinhood(newspot, kv, 0);
                }
            }
            storage->quantity = other.storage->quantity;
        }
    }

    template<class ktype, class vtype>
    void dict<ktype, vtype>::rehash(int64 capacity) {
        implementation* old = storage;
        storage = alloc<implementation>((capacity - 1)*sizeof(keyval<ktype, vtype>));
        storage->capacity = capacity;
        for (int64 ii = 0; ii<capacity; ++ii) {
            storage->bucket[ii].code = 0;
        }
        if (old) {
            storage->quantity = old->quantity;
            const int64 cap = old->capacity;
            for (int64 ii = 0; ii<cap; ii++) {
                if (old->bucket[ii].code) {
                    int64 newspot = old->bucket[ii].code&(capacity - 1);
                    robinhood(newspot, old->bucket[ii], 0);
                    old->bucket[ii].~keyval<ktype, vtype>();
                }
            }
            free(old);
        } else {
            storage->quantity = 0;
        }
    }

    template<class ktype, class vtype>
    int64 dict<ktype, vtype>::inject(const ktype& key, const vtype* ptr, size_t hh) {
        if (!storage || storage->quantity >= 3*storage->capacity/4) {
            rehash(storage ? 2*storage->capacity : 4);
        }
        const int64 cap = storage->capacity;
        for (int64 probe = 0; probe<cap; probe++) {
            size_t xx = (probe + hh)&(cap - 1);
            if (!storage->bucket[xx].code) {
                // we found an empty bucket to place our key and val
                if (ptr) {
                    new(storage->bucket + xx) keyval<ktype, vtype>(hh, key, *ptr);
                } else {
                    new(storage->bucket + xx) keyval<ktype, vtype>(hh, key, vtype());
                }
                ++storage->quantity;
                return xx;
            }
            if (storage->bucket[xx].code == hh) {
                // matching hash value, need to check the key
                if (storage->bucket[xx].getkey() == key) {
                    // we found the same key, so replace the val
                    storage->bucket[xx].setval(ptr ? *ptr : vtype());
                    return xx;
                }
            }
            // someone is in our spot, maybe we should move them
            int64 oldcost = xx - (storage->bucket[xx].code&(cap - 1));
            if (oldcost < 0) oldcost += cap;
            if (probe > oldcost) {
                keyval<ktype, vtype> kv(hh, key, ptr ? *ptr : vtype());
                trade(kv, storage->bucket[xx]);
                ++storage->quantity;
                robinhood(xx + 1, kv, oldcost + 1);
                return xx;
            }
            // their cost is greater than ours, keep probing
        }
        check(false, "should never get here");
        return -1;
    }

    template<class ktype, class vtype>
    void dict<ktype, vtype>::robinhood(
        int64 probe, keyval<ktype, vtype>& kv, int64 cost
    ) {
        const int64 cap = storage->capacity;
        for (;; ++probe) {
            int64 xx = probe&(cap - 1);
            if (!storage->bucket[xx].code) {
                new(storage->bucket + xx) keyval<ktype, vtype>();
                trade(kv, storage->bucket[xx]);
                return;
            }
            // compare costs to see who has to keep moving
            int64 oldcost = xx - (storage->bucket[xx].code&(cap - 1));
            if (oldcost < 0) oldcost += cap;
            if (cost >= oldcost) {
                // place the current one here, but keep going
                trade(kv, storage->bucket[xx]);
                cost = oldcost;
            } else {
                ++cost;
            }
            // swap or not, keep probing for an empty bin
        }
    }

    template<class ktype, class vtype>
    int64 dict<ktype, vtype>::search(const ktype& key, size_t hh) const {
        const int64 cap = storage->capacity;
        for (int64 probe = 0; probe<cap; probe++) {
            int64 xx = (probe + hh)&(cap - 1);
            if (!storage->bucket[xx].code) return -1;
            if (storage->bucket[xx].code == hh) {
                if (storage->bucket[xx].getkey() == key) return xx;
            }
            int64 cost = xx - (storage->bucket[xx].code&(cap - 1));
            if (cost < 0) cost += cap;
            // we can't be past a higher cost point
            if (probe > cost) return -1;
        }
        check(false, "should never get here");
        return -1;
    }

    template<class ktype, class vtype>
    void dict<ktype, vtype>::backshift(int64 index) {
        const int64 cap = storage->capacity;
        for (int64 ii = 0; ii<cap; ii++) {
            int64 next = (index + 1)&(cap - 1);
            if (!storage->bucket[next].code) {
                // nobody next to us to shift back
                break;
            }
            int64 cost = next - (storage->bucket[next].code&(cap - 1));
            if (cost < 0) cost += cap;
            if (cost == 0) {
                // the next guy is happy where he's at
                break;
            }
            // shuffle the next guy back, and pickup from there
            trade(storage->bucket[index], storage->bucket[next]);
            index = next;
        }

        storage->bucket[index].~keyval<ktype, vtype>();
        --storage->quantity;
    }

    template<class ktype, class vtype>
    void swap(dict<ktype, vtype>& flip, dict<ktype, vtype>& flop) {
        swap(flip.storage, flop.storage);
    }

    //}}}
