#ifndef XM_LIST_H_
#define XM_LIST_H_ 1

#include "basics.h"

namespace xm {

    template<class type>
    struct list {
        ~list();
        list();
        template<class tt>
        list(const list<tt>& other);
        list(const list<type>& other);

        template<class tt>
        list<type>& operator =(const list<tt>&);
        list<type>& operator =(const list<type>&);

        type& operator [](int64 index);
        const type& operator [](int64 index) const;

        void append(const type& value);
        void insert(int64 index, const type& value);
        void remove(int64 index);

        template<class tt>
        void extend(const list<tt>& other);

        void reserve(int64 count);
        void shrink();
        void clear();

        type* data();
        const type* data() const;
        int64 size() const;

        private:
            const type* contiguous() const;

            void realloc(int64 capacity);
            type* address(int64 index) const;

            template<class tt>
            void assign(const list<tt>& other);

            template<class tt> friend struct list;
            template<class tt> friend void swap(
                list<tt>& flip, list<tt>& flop
            );

            mutable struct implementation {
                int64 offset, quantity, capacity;
                type bucket[1];
            } *storage;
    };

    template<class type>
    list<type>::~list() { clear(); }

    template<class type>
    list<type>::list() : storage(0) {}

    template<class type>
    list<type>::list(const list<type>& other) : storage(0) {
        assign(other);
    }

    template<class type>
    template<class tt>
    list<type>::list(const list<tt>& other) : storage(0) {
        assign(other);
    }

    template<class type>
    list<type>& list<type>::operator =(const list<type>& other) {
        if (this == &other) return *this;
        clear();
        assign(other);
        return *this;
    }

    template<class type>
    template<class tt>
    list<type>& list<type>::operator =(const list<tt>& other) {
        clear();
        assign(other);
        return *this;
    }

    template<class type>
    type& list<type>::operator [](const int64 index) {
        return *address(index);
    }

    template<class type>
    const type& list<type>::operator [](const int64 index) const {
        return *address(index);
    }

    template<class type>
    void list<type>::append(const type& value) {
        if (!storage || storage->quantity >= storage->capacity) {
            realloc(storage ? 2*storage->capacity : 8);
        }
        new(address(storage->quantity++)) type(value);
    }

    template<class type>
    void list<type>::insert(int64 index, const type& value) {
        const int64 len = size();
        check(0 <= index && index <= len, "index in [0, %lld]", len);
        if (storage && len < storage->capacity) {
            if (index < len/2) {
                // shuffle the early elements back to make room
                type* dst = address(-1);
                new(dst) type(value);
                for (int64 ii = 0; ii<index; ++ii) {
                    type* src = (type*)address(ii);
                    swap(*dst, *src);
                    dst = src;
                }
                storage->offset -= 1;
                if (storage->offset < 0) {
                    storage->offset += storage->capacity;
                }
            } else {
                // shuffle the later elements forward to make room
                type* dst = address(len);
                new(dst) type(value);
                for (int64 ii = len - 1; ii>=index; --ii) {
                    type* src = address(ii);
                    swap(*dst, *src);
                    dst = src;
                }
            }
        } else {
            // special version of resize, swapping elements only once
            int64 newsize = storage ? 2*storage->capacity : 8;
            implementation* larger = alloc<implementation>(
                (newsize - 1)*sizeof(type)
            );
            larger->offset = 0;
            larger->quantity = len;
            larger->capacity = newsize;
            for (int64 ii = 0; ii<index; ++ii) {
                type* dst = larger->bucket + ii;
                new(dst) type;
                type* src = address(ii);
                swap(*dst, *src);
                src->~type();
            }
            new(larger->bucket + index) type(value);
            for (int64 ii = index; ii<len; ++ii) {
                type* dst = larger->bucket + ii + 1;
                new(dst) type;
                type* src = address(ii);
                swap(*src, *dst);
                src->~type();
            }
            if (storage) free(storage);
            storage = larger;
        }
        storage->quantity++;
    }

    template<class type>
    void list<type>::remove(int64 index) {
        const int64 len = size();
        check(0 <= index && index < len, "index in [0, %lld)", len);
        if (index < len/2) {
            // shuffle the early elements forward
            for (int64 ii = index; ii>0; --ii) {
                swap(*address(ii - 1), *address(ii));
            }
            address(0)->~type();
            ++storage->offset;
            --storage->quantity;
        } else {
            // shuffle the later elements backward
            for (int64 ii = index; ii<len - 1; ++ii) {
                swap(*address(ii), *address(ii + 1));
            }
            address(len - 1)->~type();
            --storage->quantity;
        }
    }

    template<class type>
    template<class tt>
    void list<type>::extend(const list<tt>& other) {
        const int64 len = size();
        const int64 count = other.size();
        if (len + count) reserve(len + count);
        for (int64 ii = 0; ii<count; ++ii) {
            new (address(len + ii)) type(other[ii]);
        }
        if (storage) storage->quantity += count;
    }

    template<class type>
    void list<type>::reserve(int64 count) {
        check(count > 0, "must reserve a positive count: %lld", count);
        int64 needed = 8; // must be power of 2
        while (needed < count) needed *= 2;
        if (!storage || storage->capacity < needed) realloc(needed);
    }

    template<class type>
    void list<type>::shrink() {
        if (!storage) return;
        const int64 len = size();
        if (len == 0) {
            free(storage);
            storage = 0;
            return;
        }
        int64 newsize = 4;
        while (newsize < len) newsize *= 2;
        if (newsize >= storage->capacity) return;
        implementation* smaller = alloc<implementation>(
            (newsize - 1)*sizeof(type)
        );
        for (int64 ii = 0; ii<len; ++ii) {
            type* dst = smaller->bucket + ii;
            type* src = address(ii);
            new(dst) type;
            swap(*dst, *src);
            src->~type();
        }
        free(storage);
        smaller->offset = 0;
        smaller->quantity = len;
        smaller->capacity = newsize;
        storage = smaller;
    }

    template<class type>
    void list<type>::clear() {
        if (storage) {
            for (int64 ii = 0; ii<storage->quantity; ++ii) {
                type* ptr = address(ii);
                ptr->~type();
            }
            free(storage);
            storage = 0;
        }
    }

    template<class type>
    type* list<type>::data() {
        return (type*)contiguous();
    }

    template<class type>
    const type* list<type>::data() const {
        return contiguous();
    }

    template<class type>
    const type* list<type>::contiguous() const {
        if (!storage || storage->quantity == 0) return 0;
        const int64 len = size();
        if (storage->offset + len <= storage->capacity) {
            return storage->bucket + storage->offset;
        }

        // Shuffle the head data into the whole using swaps
        // [---- tail ----][--- hole ---][----- head -----]
        int64 head = storage->capacity - storage->offset;
        int64 tail = storage->quantity - head;
        int64 hole = storage->capacity - storage->quantity;
        int64 temp = xm::min(hole, head);
        for (int64 ii = 0; ii<temp; ii++) {
            new(storage->bucket + tail + ii) type;
        }
        for (int64 ii = 0; ii<head; ii++) {
            swap(
                storage->bucket[tail + ii],
                storage->bucket[storage->offset + ii]
            );
        }
        // [---- tail ----][----- head -----][--- hole ---]
        for (int64 ii = 0; ii<temp; ii++) {
            storage->bucket[storage->capacity - 1 - ii].~type();
        }

        // Rotate the data using the "Juggling" algorithm
        int64 gcd = len;
        int64 tmp = tail;
        while (tmp) {
            int64 tt = gcd%tmp;
            gcd = tmp;
            tmp = tt;
        }

        int64 cyc = len/gcd - 1;
        for (int64 ii = 0; ii<gcd; ii++) {
            int64 lo = ii;
            int64 hi = ii + tail;
            for (int64 kk = 0; kk<cyc; kk++) {
                swap(storage->bucket[lo], storage->bucket[hi]);
                lo = hi; hi += tail;
                if (hi >= len) hi -= len;
            }
        }

        storage->offset = 0;
        return storage->bucket;
    }

    template<class type>
    int64 list<type>::size() const {
        return storage ? storage->quantity : 0;
    }

    template<class type>
    void list<type>::realloc(int64 capacity) {
        implementation* changed = alloc<implementation>(
            (capacity - 1)*sizeof(type)
        );
        changed->offset = 0;
        changed->quantity = 0;
        changed->capacity = capacity;
        if (storage) {
            for (int64 ii = 0; ii<storage->quantity; ++ii) {
                new(changed->bucket + ii) type;
                type* old = address(ii);
                swap(changed->bucket[ii], *old);
                old->~type();
            }
            changed->quantity = storage->quantity;
            free(storage);
        }
        storage = changed;
    }

    template<class type>
    type* list<type>::address(int64 index) const {
        // circular indexing, requires capacity is power of 2
        return storage->bucket + (
            (storage->offset + index)&(storage->capacity - 1)
        );
    }

    template<class type>
    template<class tt>
    void list<type>::assign(const list<tt>& other) {
        if (other.storage) {
            const int64 len = other.storage->quantity;
            const int64 cap = other.storage->capacity;
            storage = alloc<implementation>((cap - 1)*sizeof(type));
            storage->offset = 0;
            storage->quantity = len;
            storage->capacity = cap;
            for (int64 ii = 0; ii<len; ++ii) {
                new(address(ii)) type(other[ii]);
            }
        }
    }

    template<class type>
    void swap(list<type>& flip, list<type>& flop) {
        swap(flip.storage, flop.storage);
    }

}

#endif // XM_LIST_H_

