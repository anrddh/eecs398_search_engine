#pragma once
#include <pthread.h>

namespace fb {

//Exception type (since we dont have assert)
class MutexExcept{};

/* Wrapper around the pthread mutex
 * provides a more modern c++ interface
 */
class Mutex
{
public:
   Mutex()
      {
         if ( pthread_mutex_init(&Lock, NULL) != 0)
            throw MutexExcept();
      }

   void lock() noexcept
      {
      pthread_mutex_lock(&Lock);
      }

   void unlock() noexcept
      {
      pthread_mutex_unlock(&Lock);
      }
private:
   pthread_mutex_t Lock;
};

/* AutoLock is a synchronization tool that takes advantage of RAII
 * The given mutex is lock during ctor and mutex is unlocked
 * during dtor
 */
template <typename L>
class AutoLock
{
public:
   AutoLock(L& mutex_) : mutex(mutex_)
      {
      mutex.lock();
      }

   ~AutoLock()
      {
      mutex.unlock();
      }
private:
   L mutex;
};
}; // Namespace fb
