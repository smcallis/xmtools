#ifndef XM_QUEUE_H_
#define XM_QUEUE_H_ 1

#include <time.h>
#include <pthread.h>

#include "basics.h"

namespace xm {

    template<class type>
    struct queue {
        queue();
        ~queue();

        void give(type& item);
        void push(const type& item);
        type pull();
        bool pull(type& item, double seconds);

        private:
            queue(const queue&); // deleted
            queue& operator =(const queue&); // deleted

            list<type> data;     
            pthread_mutex_t mutex;
            pthread_cond_t condvar;
    };

    template<class type>
    queue<type>::queue() {
        pthread_condattr_t attr;
        pthread_condattr_init(&attr);
        pthread_condattr_setclock(&attr, CLOCK_MONOTONIC);
        pthread_mutex_init(&mutex, 0);
        pthread_cond_init(&condvar, &attr);
    }

    template<class type>
    queue<type>::~queue() {
        pthread_mutex_destroy(&mutex);
        pthread_cond_destroy(&condvar);
    }

    template<class type>
    void queue<type>::give(type& item) {
        pthread_mutex_lock(&mutex);

        data.append(type());
        swap(data[data.size() - 1], item);

        pthread_cond_signal(&condvar);
        pthread_mutex_unlock(&mutex);
    }

    template<class type>
    void queue<type>::push(const type& item) {
        pthread_mutex_lock(&mutex);

        data.append(item);

        pthread_cond_signal(&condvar);
        pthread_mutex_unlock(&mutex);
    }

    template<class type>
    type queue<type>::pull() {
        type item;
        pthread_mutex_lock(&mutex);

        while (data.size() == 0) {
            pthread_cond_wait(&condvar, &mutex);
        }

        swap(item, data[0]);
        data.remove(0);

        pthread_mutex_unlock(&mutex);

        return item;
    }

    template<class type>
    bool queue<type>::pull(type& item, double seconds) {
        struct timespec tv;
        clock_gettime(CLOCK_MONOTONIC, &tv);
        int64 nanos = tv.tv_sec*1000000000LL + tv.tv_nsec;

        nanos += llrint(seconds*1e9);

        tv.tv_sec  = nanos/1000000000LL;
        tv.tv_nsec = nanos%1000000000LL;

        pthread_mutex_lock(&mutex);

        while (data.size() == 0) {
            int result = pthread_cond_timedwait(&condvar, &mutex, &tv);
            if (result == ETIMEDOUT) {
                pthread_mutex_unlock(&mutex);
                return false;
            }
        }

        swap(item, data[0]);
        data.remove(0);

        pthread_mutex_unlock(&mutex);

        return true;
    }

}

#endif // XM_QUEUE_H_

