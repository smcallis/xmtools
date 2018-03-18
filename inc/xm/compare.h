#ifndef XM_COMPARE_H_
#define XM_COMPARE_H_ 1

namespace xm {

    template<class type>
    bool compare_lt(const type& aa, const type& bb) { return aa <  bb; }

    template<class type>
    bool compare_le(const type& aa, const type& bb) { return aa <= bb; }

    template<class type>
    bool compare_ge(const type& aa, const type& bb) { return aa >= bb; }

    template<class type>
    bool compare_gt(const type& aa, const type& bb) { return aa >  bb; }

    template<class type>
    bool compare_eq(const type& aa, const type& bb) { return aa == bb; }

    template<class type>
    bool compare_ne(const type& aa, const type& bb) { return aa != bb; }

}

#endif // XM_COMPARE_H_

