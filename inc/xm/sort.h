#ifndef XM_SORT_H_
#define XM_SORT_H_ 1

namespace xm {
    //{{{ internal
    namespace internal {
        template<class type, class compare>
        void dualpivot(type* lo, type* hi, type*& lt, type*& gt, compare& lessthan) {
            // before loop, L and H are chosen as pivots
            //
            //     +---+---+---+---+---+---+---+---+---+---+---+
            //     | L | ? | ? | ? | ? | ? | ? | ? | ? | ? | H |
            //     +---+---+---+---+---+---+---+---+---+---+---+
            //     ^lo ^lt,ii                              ^gt,hi
            //
            int64 third = (hi - lo)/3;
            if (lessthan(lo[+third], hi[-third])) {
                swap(*lo, lo[+third]);
                swap(*hi, hi[-third]);
            } else {
                swap(*lo, hi[-third]);
                swap(*hi, lo[+third]);
            }

            // during loop, unknowns are organized
            //
            //     +---+---+---+---+---+---+---+---+---+---+---+
            //     | L | < | < | ~ | ~ | ? | ? | ? | > | > | H |
            //     +---+---+---+---+---+---+---+---+---+---+---+
            //     ^lo         ^lt     ^ii         ^gt     ^hi
            //
            lt = lo + 1;
            gt = hi;
            type* ii = lt;
            while (ii < gt) {
                if (lessthan(*ii, *lo)) {
                    swap(*lt++, *ii++);
                    continue;
                }
                if (lessthan(*hi, *ii)) {
                    swap(*ii, *--gt);
                    continue;
                }
                ++ii;
            }

            // after loop, no more unknowns
            //
            //     +---+---+---+---+---+---+---+---+---+---+---+
            //     | L | < | < | < | ~ | ~ | ~ | > | > | > | H |
            //     +---+---+---+---+---+---+---+---+---+---+---+
            //     ^lo             ^lt         ^gt         ^hi
            //
            // swapping the pivots, data is partitioned:
            //
            //          lowers < L <= centers <= H < uppers
            //     +---+---+---+---+---+---+---+---+---+---+---+
            //     | < | < | < | L | ~ | ~ | ~ | H | > | > | > |
            //     +---+---+---+---+---+---+---+---+---+---+---+
            //     ^lo         ^lt                 ^gt     ^hi
            //
            swap(*lo, *--lt);
            swap(*hi, *gt++);
        }

        // This heapsifts in the opposite order than you would expect
        // so that heapsort below can remove the LARGEST element first...
        // XXX: Should figure out a way to re-use this for a heap<T> container.
        template<class type, class compare>
        void heapsift(type* ptr, int64 root, int64 count, compare& lessthan) {
            while (root*2 + 1 < count) {
                int64 child = root*2 + 1;
                if (child < count - 1) {
                    if (lessthan(ptr[child], ptr[child + 1])) {
                        child = child + 1;
                    }
                }
                if (lessthan(ptr[root], ptr[child])) {
                    swap(ptr[root], ptr[child]);
                    root = child;
                } else {
                    return;
                }
            }
        }

        template<class type, class compare>
        void heapsort(type* ptr, int64 len, compare& lessthan) {
            int64 start = len/2 - 1;
            int64 end = len - 1;

            while (start >= 0) {
                heapsift(ptr, start, len, lessthan);
                start = start - 1;
            }

            while (end > 0) {
                swap(ptr[end], *ptr);
                heapsift(ptr, 0, end, lessthan);
                end = end - 1;
            }
        }

        template<class type, class compare>
        void recursort(type* lo, type* hi, int64 depth, compare& lessthan) {
            int64 span = hi - lo;
            if (span <= 0) return;
            if (span <= 8) {
                // insertion sort
                for (type* ii = lo + 1; ii <= hi; ++ii) {
                    for (type* jj = ii; jj > lo; --jj) {
                        if (lessthan(*jj, jj[-1])) {
                            swap(*jj, jj[-1]);
                        } else {
                            break;
                        }
                    }
                }
                return;
            }

            if (depth < 0) {
                // recursed too deep, fallback to heapsort
                heapsort(lo, span + 1, lessthan);
                return;
            }

            // quicksort
            type *lt, *gt;
            dualpivot(lo, hi, lt, gt, lessthan);

            // recurse as needed
            recursort(lo, lt - 1, depth - 1, lessthan);
            if (lessthan(*lt, gt[-1])) {
                recursort(lt + 1, gt - 2, depth - 1, lessthan);
            }
            recursort(gt, hi, depth - 1, lessthan);
        }
    }
    //}}}

    template<class type, class compare>
    void introsort(type* data, int64 len, compare& lessthan) {
        using namespace internal;
        // this is a fast integer logarithm base two
        int64 depth = 8*sizeof(long long) - __builtin_clzll(len);
        recursort(data, data + len - 1, depth, lessthan);
    }

    template<class type>
    void introsort(type* data, int64 len) {
        introsort(data, len, compare_lt<type>);
    }

    /* XXX: do these for vector<> and list<> ?
    template<class type, int64 size, class compare>
    void introsort(vector<type, size>& vv, compare& lessthan) {
        introsort(vv.data(), vv.size(), lessthan);
    }

    template<class type, int64 size>
    void introsort(vector<type, size>& vv) {
        introsort(vv.data(), vv.size(), compare_lt<type>);
    }
    */

}

#endif // XM_SORT_H_
