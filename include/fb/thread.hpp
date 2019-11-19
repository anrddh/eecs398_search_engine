#pragma once

#include "memory.hpp"
#include <pthread.h>

namespace fb {
/* Wrapper around pthread
 * Provides a more modern interface
 */
class Thread {
   public:
      Thread() = default; // Invalid default thread
      Thread( void* ( *func ) ( void* ), void* args ) : t(makeUniqueDefaultInit<pthread_t>())
      {
         pthread_create(t.get(), nullptr, func, args);
      }

      Thread( Thread&& other) {
         t.swap(other.t);
      }

      Thread& operator=( Thread&& other ) noexcept {
         t.swap(other.t);
         return *this;
      }

      void join() noexcept
      {
         pthread_join(*t.get(), nullptr);
      }

      void detach() noexcept
      {
         pthread_detach(*t.get());
      }
   private:
      UniquePtr<pthread_t> t;
};
};
