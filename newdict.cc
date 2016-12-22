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
            internal::dictbase<ktype, vtype>* storage;
    };

    template<class ktype, class vtype>
    newdict<ktype, vtype>::~newdict() {
        if (storage) delete storage;
    }

    template<class ktype, class vtype>
    newdict<ktype, vtype>::newdict() : storage(0) {}



}

int main() {

    using namespace xm::internal;


    printf("size: %zd\n", sizeof(dictimpl<int, int, 8>));

    return 0;
}

