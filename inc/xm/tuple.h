#ifndef XM_TUPLE_H_
#define XM_TUPLE_H_ 1

#include "hash.h"

namespace xm {

    //{{{ tuple 8
    template<
        class type1=none, class type2=none, class type3=none, class type4=none,
        class type5=none, class type6=none, class type7=none, class type8=none
    >
    struct tuple {
        type1 first; type2 second; type3 third; type4 fourth;
        type5 fifth; type6 sixth; type7 seventh; type8 eighth;

        tuple() {}

        tuple(type1 v1, type2 v2, type3 v3, type4 v4, type5 v5, type6 v6, type7 v7, type8 v8) :
            first(v1), second(v2), third(v3), fourth(v4),
            fifth(v5), sixth(v6), seventh(v7), eighth(v8) {
        }

        template<
            class t1, class t2, class t3, class t4, 
            class t5, class t6, class t7, class t8
        >
        tuple(const tuple<t1, t2, t3, t4, t5, t6, t7, t8>& tt) :
            first(tt.first), second(tt.second), third(tt.third), fourth(tt.fourth),
            fifth(tt.fifth), sixth(tt.sixth), seventh(tt.seventh), eighth(tt.eighth) {
        }

        template<
            class t1, class t2, class t3, class t4, 
            class t5, class t6, class t7, class t8
        >
        tuple<type1, type2, type3, type4, type5, type6, type7, type8>&
        operator =(const tuple<t1, t2, t3, t4, t5, t6, t7, t8>& that) {
            first   = that.first;   second = that.second;
            third   = that.third;   fourth = that.fourth;
            fifth   = that.fifth;   sixth  = that.sixth;
            seventh = that.seventh; eighth = that.eighth;
            return *this;
        }
    };

    template<
        class type1, class type2, class type3, class type4,
        class type5, class type6, class type7, class type8
    >
    tuple<type1, type2, type3, type4, type5, type6, type7, type8>
    multival(
        const type1& val1, const type2& val2,
        const type3& val3, const type4& val4,
        const type5& val5, const type6& val6,
        const type7& val7, const type8& val8
    ) {
        return tuple<
            type1, type2, type3, type4,
            type5, type6, type7, type8
        >(val1, val2, val3, val4, val5, val6, val7, val8);
    }

    template<
        class type1, class type2, class type3, class type4,
        class type5, class type6, class type7, class type8
    >
    tuple<type1&, type2&, type3&, type4&, type5&, type6&, type7&, type8&>
    multiref(
        type1& ref1, type2& ref2,
        type3& ref3, type4& ref4,
        type5& ref5, type6& ref6,
        type7& ref7, type8& ref8
    ) {
        return tuple<
            type1&, type2&, type3&, type4&,
            type5&, type6&, type7&, type8&
        >(ref1, ref2, ref3, ref4, ref5, ref6, ref7, ref8);
    }

    template<
        class type1, class type2, class type3, class type4,
        class type5, class type6, class type7, class type8
    >
    size_t hash(
        const tuple<
            type1, type2, type3, type4, 
            type5, type6, type7, type8
        >& tt
    ) {
        return (
            1*hash(tt.first) + 3*hash(tt.second) + 5*hash(tt.third) + 7*hash(tt.fourth) +
            11*hash(tt.fifth) + 13*hash(tt.sixth) + 17*hash(tt.seventh) + 19*hash(tt.eighth)
        );
    }

    template<
        class a1, class a2, class a3, class a4, class a5, class a6, class a7, class a8,
        class b1, class b2, class b3, class b4, class b5, class b6, class b7, class b8
    >
    bool operator ==(
        const tuple<a1, a2, a3, a4, a5, a6, a7, a8>& aa,
        const tuple<b1, b2, b3, b4, b5, b6, b7, b8>& bb
    ) {
        return (
            aa.first == bb.first && aa.second == bb.second && aa.third == bb.third && aa.fourth == bb.fourth &&
            aa.fifth == bb.fifth && aa.sixth == bb.sixth && aa.seventh == bb.seventh && aa.eighth == bb.eighth
        );
    }

    //}}}
    //{{{ tuple 7
    template<
        class type1, class type2, class type3, class type4,
        class type5, class type6, class type7
    > 
    struct tuple<type1, type2, type3, type4, type5, type6, type7> {
        type1 first; type2 second; type3 third; type4 fourth;
        type5 fifth; type6 sixth; type7 seventh;

        tuple() {}

        tuple(type1 v1, type2 v2, type3 v3, type4 v4, type5 v5, type6 v6, type7 v7) :
            first(v1), second(v2), third(v3), fourth(v4),
            fifth(v5), sixth(v6), seventh(v7) {
        }

        template<
            class t1, class t2, class t3, class t4, 
            class t5, class t6, class t7
        >
        tuple(const tuple<t1, t2, t3, t4, t5, t6, t7>& tt) :
            first(tt.first), second(tt.second), third(tt.third), fourth(tt.fourth),
            fifth(tt.fifth), sixth(tt.sixth), seventh(tt.seventh) {
        }

        template<
            class t1, class t2, class t3, class t4, 
            class t5, class t6, class t7
        >
        tuple<type1, type2, type3, type4, type5, type6, type7>&
        operator =(const tuple<t1, t2, t3, t4, t5, t6, t7>& that) {
            first   = that.first;   second = that.second;
            third   = that.third;   fourth = that.fourth;
            fifth   = that.fifth;   sixth  = that.sixth;
            seventh = that.seventh;
            return *this;
        }
    };

    template<
        class type1, class type2, class type3, class type4,
        class type5, class type6, class type7
    >
    tuple<type1, type2, type3, type4, type5, type6, type7>
    multival(
        const type1& val1, const type2& val2,
        const type3& val3, const type4& val4,
        const type5& val5, const type6& val6,
        const type7& val7
    ) {
        return tuple<
            type1, type2, type3, type4,
            type5, type6, type7
        >(val1, val2, val3, val4, val5, val6, val7);
    }

    template<
        class type1, class type2, class type3, class type4,
        class type5, class type6, class type7
    >
    tuple<type1&, type2&, type3&, type4&, type5&, type6&, type7&>
    multiref(
        type1& ref1, type2& ref2,
        type3& ref3, type4& ref4,
        type5& ref5, type6& ref6,
        type7& ref7
    ) {
        return tuple<
            type1&, type2&, type3&, type4&,
            type5&, type6&, type7&
        >(ref1, ref2, ref3, ref4, ref5, ref6, ref7);
    }

    template<
        class type1, class type2, class type3, class type4,
        class type5, class type6, class type7
    >
    size_t hash(
        const tuple<
            type1, type2, type3, type4, 
            type5, type6, type7
        >& tt
    ) {
        return (
            1*hash(tt.first) + 3*hash(tt.second) + 5*hash(tt.third) + 7*hash(tt.fourth) +
            11*hash(tt.fifth) + 13*hash(tt.sixth) + 17*hash(tt.seventh)
        );
    }

    template<
        class a1, class a2, class a3, class a4, class a5, class a6, class a7,
        class b1, class b2, class b3, class b4, class b5, class b6, class b7
    >
    bool operator ==(
        const tuple<a1, a2, a3, a4, a5, a6, a7>& aa,
        const tuple<b1, b2, b3, b4, b5, b6, b7>& bb
    ) {
        return (
            aa.first == bb.first && aa.second == bb.second && aa.third == bb.third && aa.fourth == bb.fourth &&
            aa.fifth == bb.fifth && aa.sixth == bb.sixth && aa.seventh == bb.seventh
        );
    }

    //}}}
    //{{{ tuple 6
    template<
        class type1, class type2, class type3, class type4,
        class type5, class type6
    > 
    struct tuple<type1, type2, type3, type4, type5, type6> {
        type1 first; type2 second; type3 third; type4 fourth;
        type5 fifth; type6 sixth;

        tuple() {}

        tuple(type1 v1, type2 v2, type3 v3, type4 v4, type5 v5, type6 v6) :
            first(v1), second(v2), third(v3), fourth(v4),
            fifth(v5), sixth(v6) {
        }

        template<
            class t1, class t2, class t3, class t4, 
            class t5, class t6
        >
        tuple(const tuple<t1, t2, t3, t4, t5, t6>& tt) :
            first(tt.first), second(tt.second), third(tt.third), fourth(tt.fourth),
            fifth(tt.fifth), sixth(tt.sixth) {
        }

        template<
            class t1, class t2, class t3, class t4, 
            class t5, class t6
        >
        tuple<type1, type2, type3, type4, type5, type6>&
        operator =(const tuple<t1, t2, t3, t4, t5, t6>& that) {
            first   = that.first;   second = that.second;
            third   = that.third;   fourth = that.fourth;
            fifth   = that.fifth;   sixth  = that.sixth;
            return *this;
        }
    };

    template<
        class type1, class type2, class type3, class type4,
        class type5, class type6
    >
    tuple<type1, type2, type3, type4, type5, type6>
    multival(
        const type1& val1, const type2& val2,
        const type3& val3, const type4& val4,
        const type5& val5, const type6& val6
    ) {
        return tuple<
            type1, type2, type3, type4,
            type5, type6
        >(val1, val2, val3, val4, val5, val6);
    }

    template<
        class type1, class type2, class type3, class type4,
        class type5, class type6
    >
    tuple<type1&, type2&, type3&, type4&, type5&, type6&>
    multiref(
        type1& ref1, type2& ref2,
        type3& ref3, type4& ref4,
        type5& ref5, type6& ref6
    ) {
        return tuple<
            type1&, type2&, type3&, type4&,
            type5&, type6&
        >(ref1, ref2, ref3, ref4, ref5, ref6);
    }

    template<
        class type1, class type2, class type3, class type4,
        class type5, class type6
    >
    size_t hash(
        const tuple<
            type1, type2, type3, type4, 
            type5, type6
        >& tt
    ) {
        return (
            1*hash(tt.first) + 3*hash(tt.second) + 5*hash(tt.third) + 7*hash(tt.fourth) +
            11*hash(tt.fifth) + 13*hash(tt.sixth)
        );
    }

    template<
        class a1, class a2, class a3, class a4, class a5, class a6,
        class b1, class b2, class b3, class b4, class b5, class b6
    >
    bool operator ==(
        const tuple<a1, a2, a3, a4, a5, a6>& aa,
        const tuple<b1, b2, b3, b4, b5, b6>& bb
    ) {
        return (
            aa.first == bb.first && aa.second == bb.second && aa.third == bb.third && aa.fourth == bb.fourth &&
            aa.fifth == bb.fifth && aa.sixth == bb.sixth
        );
    }

    //}}}
    //{{{ tuple 5
    template<
        class type1, class type2, class type3, class type4,
        class type5
    > 
    struct tuple<type1, type2, type3, type4, type5> {
        type1 first; type2 second; type3 third; type4 fourth;
        type5 fifth;

        tuple() {}

        tuple(type1 v1, type2 v2, type3 v3, type4 v4, type5 v5) :
            first(v1), second(v2), third(v3), fourth(v4), fifth(v5) {
        }

        template<class t1, class t2, class t3, class t4, class t5>
        tuple(const tuple<t1, t2, t3, t4, t5>& tt) :
            first(tt.first), second(tt.second), third(tt.third), fourth(tt.fourth),
            fifth(tt.fifth) {
        }

        template<
            class t1, class t2, class t3, class t4, 
            class t5
        >
        tuple<type1, type2, type3, type4, type5>&
        operator =(const tuple<t1, t2, t3, t4, t5>& that) {
            first   = that.first;   second = that.second;
            third   = that.third;   fourth = that.fourth;
            fifth   = that.fifth;
            return *this;
        }
    };

    template<
        class type1, class type2, class type3, class type4,
        class type5
    >
    tuple<type1, type2, type3, type4, type5>
    multival(
        const type1& val1, const type2& val2,
        const type3& val3, const type4& val4,
        const type5& val5
    ) {
        return tuple<type1, type2, type3, type4, type5>(
            val1, val2, val3, val4, val5
        );
    }

    template<
        class type1, class type2, class type3, class type4,
        class type5
    >
    tuple<type1&, type2&, type3&, type4&, type5&>
    multiref(
        type1& ref1, type2& ref2,
        type3& ref3, type4& ref4,
        type5& ref5
    ) {
        return tuple<type1&, type2&, type3&, type4&, type5&>(
            ref1, ref2, ref3, ref4, ref5
        );
    }

    template<
        class type1, class type2, class type3, class type4,
        class type5
    >
    size_t hash(const tuple<type1, type2, type3, type4, type5>& tt) {
        return (
            1*hash(tt.first) + 3*hash(tt.second) + 5*hash(tt.third) + 7*hash(tt.fourth) +
            11*hash(tt.fifth)
        );
    }

    template<
        class a1, class a2, class a3, class a4, class a5,
        class b1, class b2, class b3, class b4, class b5
    >
    bool operator ==(
        const tuple<a1, a2, a3, a4, a5>& aa,
        const tuple<b1, b2, b3, b4, b5>& bb
    ) {
        return (
            aa.first == bb.first && aa.second == bb.second && aa.third == bb.third && aa.fourth == bb.fourth &&
            aa.fifth == bb.fifth
        );
    }

    //}}}
    //{{{ tuple 4
    template<class type1, class type2, class type3, class type4> 
    struct tuple<type1, type2, type3, type4> {
        type1 first; type2 second; type3 third; type4 fourth;

        tuple() {}

        tuple(type1 v1, type2 v2, type3 v3, type4 v4) :
            first(v1), second(v2), third(v3), fourth(v4) {
        }

        template<class t1, class t2, class t3, class t4>
        tuple(const tuple<t1, t2, t3, t4>& tt) :
            first(tt.first), second(tt.second), third(tt.third), fourth(tt.fourth) {
        }

        template<class t1, class t2, class t3, class t4>
        tuple<type1, type2, type3, type4>&
        operator =(const tuple<t1, t2, t3, t4>& that) {
            first   = that.first;   second = that.second;
            third   = that.third;   fourth = that.fourth;
            return *this;
        }
    };

    template<class type1, class type2, class type3, class type4>
    tuple<type1, type2, type3, type4>
    multival(
        const type1& val1, const type2& val2,
        const type3& val3, const type4& val4
    ) {
        return tuple<type1, type2, type3, type4>(
            val1, val2, val3, val4
        );
    }

    template<class type1, class type2, class type3, class type4>
    tuple<type1&, type2&, type3&, type4&>
    multiref(
        type1& ref1, type2& ref2,
        type3& ref3, type4& ref4
    ) {
        return tuple<type1&, type2&, type3&, type4&>(
            ref1, ref2, ref3, ref4
        );
    }

    template<class type1, class type2, class type3, class type4>
    size_t hash(const tuple<type1, type2, type3, type4>& tt) {
        return (
            1*hash(tt.first) + 3*hash(tt.second) + 5*hash(tt.third) + 7*hash(tt.fourth)
        );
    }

    template<
        class a1, class a2, class a3, class a4,
        class b1, class b2, class b3, class b4
    >
    bool operator ==(
        const tuple<a1, a2, a3, a4>& aa,
        const tuple<b1, b2, b3, b4>& bb
    ) {
        return (
            aa.first == bb.first && aa.second == bb.second && aa.third == bb.third && aa.fourth == bb.fourth
        );
    }

    //}}}
    //{{{ tuple 3
    template<class type1, class type2, class type3> 
    struct tuple<type1, type2, type3> {
        type1 first; type2 second; type3 third;

        tuple() {}

        tuple(type1 v1, type2 v2, type3 v3) :
            first(v1), second(v2), third(v3) {
        }

        template<class t1, class t2, class t3>
        tuple(const tuple<t1, t2, t3>& tt) :
            first(tt.first), second(tt.second), third(tt.third) {
        }

        template<class t1, class t2, class t3>
        tuple<type1, type2, type3>&
        operator =(const tuple<t1, t2, t3>& that) {
            first   = that.first;   second = that.second;
            third   = that.third;
            return *this;
        }
    };

    template<class type1, class type2, class type3>
    tuple<type1, type2, type3>
    multival(
        const type1& val1, const type2& val2,
        const type3& val3
    ) {
        return tuple<type1, type2, type3>(
            val1, val2, val3
        );
    }

    template<class type1, class type2, class type3>
    tuple<type1&, type2&, type3&>
    multiref(
        type1& ref1, type2& ref2,
        type3& ref3
    ) {
        return tuple<type1&, type2&, type3&>(
            ref1, ref2, ref3
        );
    }

    template<class type1, class type2, class type3>
    size_t hash(const tuple<type1, type2, type3>& tt) {
        return 1*hash(tt.first) + 3*hash(tt.second) + 5*hash(tt.third);
    }

    template<
        class a1, class a2, class a3,
        class b1, class b2, class b3
    >
    bool operator ==(
        const tuple<a1, a2, a3>& aa,
        const tuple<b1, b2, b3>& bb
    ) {
        return (
            aa.first == bb.first && aa.second == bb.second && aa.third == bb.third
        );
    }

    //}}}
    //{{{ tuple 2
    template<class type1, class type2> 
    struct tuple<type1, type2> {
        type1 first; type2 second;

        tuple() {}

        tuple(type1 v1, type2 v2) :
            first(v1), second(v2) {
        }

        template<class t1, class t2>
        tuple(const tuple<t1, t2>& tt) :
            first(tt.first), second(tt.second) {
        }

        template<class t1, class t2>
        tuple<type1, type2>&
        operator =(const tuple<t1, t2>& that) {
            first   = that.first;   second = that.second;
            return *this;
        }
    };

    template<class type1, class type2>
    tuple<type1, type2>
    multival(
        const type1& val1, const type2& val2
    ) {
        return tuple<type1, type2>(val1, val2);
    }

    template<class type1, class type2>
    tuple<type1&, type2&>
    multiref(
        type1& ref1, type2& ref2
    ) {
        return tuple<type1&, type2&>(ref1, ref2);
    }

    template<class type1, class type2>
    size_t hash(const tuple<type1, type2>& tt) {
        return 1*hash(tt.first) + 3*hash(tt.second);
    }

    template<
        class a1, class a2,
        class b1, class b2
    >
    bool operator ==(
        const tuple<a1, a2>& aa,
        const tuple<b1, b2>& bb
    ) {
        return (
            aa.first == bb.first && aa.second == bb.second
        );
    }

    //}}}
    //{{{ tuple 1
    template<class type1> 
    struct tuple<type1> {
        type1 first;

        tuple() {}

        tuple(type1 v1) : first(v1) {
        }

        template<class t1>
        tuple(const tuple<t1>& tt) : first(tt.first) {}

        template<class t1>
        tuple<type1>&
        operator =(const tuple<t1>& that) {
            first   = that.first;
            return *this;
        }
    };

    template<class type1>
    tuple<type1>
    multival(const type1& val1) {
        return tuple<type1>(val1);
    }

    template<class type1>
    tuple<type1&>
    multiref(type1& ref1) {
        return tuple<type1&>(ref1);
    }

    template<class type1>
    size_t hash(const tuple<type1>& tt) {
        return hash(tt.first);
    }

    template<class a1, class b1>
    bool operator ==(const tuple<a1>& aa, const tuple<b1>& bb) {
        return aa.first == bb.first;
    }

    //}}}
    //{{{ tuple 0
    template<> struct tuple<> {};

    tuple<> multival() {
        return tuple<>();
    }

    tuple<> multiref() {
        return tuple<>();
    }

    template<>
    size_t hash(const tuple<>& tt) {
        (void)tt;
        return 0;
    }

    template<>
    bool operator ==(const tuple<>& aa, const tuple<>& bb) {
        (void)aa; (void)bb;
        return true;
    }

    //}}}

}

#endif // XM_TUPLE_H_

