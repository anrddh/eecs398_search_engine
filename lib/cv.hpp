#pragma once
#include "mutex.hpp"
#include <pthread.h>

namespace fb {

/* Wrapper around pthread cv to 
 * match c++ cond_var more closely
 */
class CV {
public:
   CV() noexcept
      {
      pthread_cond_init(&cv, nullptr);
      }

   inline void wait(Mutex& mtx) noexcept
   {
      pthread_cond_wait(&cv, &mtx.lock);
   }

   inline void signal() noexcept
   {
      pthread_cond_signal(&cv);
   }

   inline void broadcast() noexcept
   {
      pthread_cond_broadcast(&cv);
   }
private:
   pthread_cond_t cv;
};
};
