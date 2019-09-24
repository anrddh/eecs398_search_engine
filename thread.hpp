#pragma once

#include <pthread.h>

namespace fb {
/* Wrapper around pthread
 * Provides a more modern interface
 */
class Thread {
   public:
      Thread( void* ( *func ) ( void* ), void* args ) 
      {
         pthread_create(&t, nullptr, func, args);
      }

      void join() noexcept 
      {
         pthread_join(&t, nullptr);
      }

      void detach() noexcept
      {
         pthread_detach(&t, nullptr);
      }
   private:
      pthread_t t;
};
};
