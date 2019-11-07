#pragma once
#include <pthread.h>

namespace fb {

/* Wrapper around the pthread mutex
 * provides a more modern c++ interface
 */
class Mutex 
{
public:
   Mutex() 
      {
         if ( pthread_mutex_init(&lock, NULL) != 0)
            assert(false);
      }

   void lock() noexcept
      {
      pthread_mutex_lock(&lock);
      }

   void unlock() noexcept
      {
      pthread_mutex_unlock(&lock);
      }
private:
   pthread_mutex_t lock;
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
