#pragma once

#include <pthread.h>

namespace fb {

    class SharedMutex {
    public:
        SharedMutex(const pthread_rwlockattr_t *attr = nullptr) {
            pthread_rwlock_init(&rwlock, attr);
        }

        SharedMutex(const SharedMutex &) = delete;
        SharedMutex& operator=(const SharedMutex &) = delete;

        ~SharedMutex() {
            pthread_rwlock_destroy(&rwlock);
        }

        void lock() {
            pthread_rwlock_wrlock(&rwlock);
        }

        bool tryLock() {
            return !pthread_rwlock_trywrlock(&rwlock);
        }

        void unlock() {
            pthread_rwlock_unlock(&rwlock);
        }

        void lock_shared() {
            pthread_rwlock_rdlock(&rwlock);
        }

        bool try_lock_shared() {
            return !pthread_rwlock_tryrdlock(&rwlock);
        }

        void unlock_shared() {
            pthread_rwlock_unlock(&rwlock);
        }

    private:
        pthread_rwlock_t rwlock = PTHREAD_RWLOCK_INITIALIZER;
    };

};
