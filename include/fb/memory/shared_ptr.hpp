//  SharedPtr.hpp
//  SharedPtr
//  Similar to Shared pointer from the stl
//  Allows multiple pointers to an object.
//  When all pointers go out of scope, the object
//  pointed by the shared pointers will be deleted.
//
//  To check the if an object is still alive, one
//  can create a weak_ptr to keep track of if the
//  shared object is still alive.
//
//  Created by Jaeyoon Kim on 9/10/19.

#ifndef SHARED_PTR_HPP
#define SHARED_PTR_HPP

#include "../utility.hpp"

namespace fb {

template <typename T>
class SharedPtr; // forward declration

template <typename T>
class WeakPtr {
public:
   inline void operator=( const SharedPtr<T>& s ) noexcept
      {
      obj = s.obj;
      counter = s.counter;
      ++counter->second;
      }

   inline bool expired( ) const noexcept
      {
      if (!counter)
         return true;

      return !counter->first;
      }

   constexpr inline SharedPtr<T> lock( ) const noexcept
      {
      return SharedPtr<T>(obj, counter);
      }

private:
   T* obj = nullptr;
   Pair<int, int>* counter = nullptr;
};

template <typename T>
class SharedPtr {
public:
   constexpr inline SharedPtr( T* obj_ ) : SharedPtr( obj_, new Pair<int, int>{0,0} )
      {
      }

   inline SharedPtr( const SharedPtr<T>& other ) noexcept
      {
      obj = other.obj;
      counter = other.counter;
      ++counter->first;
      }

   inline SharedPtr( SharedPtr<T>&& other ) noexcept
      {
      obj = other.obj;
      other.obj = nullptr;
      counter = other.counter;
      other.counter = nullptr;
      }

   inline T& operator*( )
      {
      return *obj;
      }

   inline T* operator->( )
      {
      return obj;
      }

   void reset( ) noexcept
      {
      if ( !counter ) {
         // Someone already deleted my objects for me (move ctor)
         return;
      }
      if ( --counter->first == 0 )
         {
         delete obj;
         obj = nullptr;
         if ( counter->second == 0 )
            {
            delete counter;
            counter = nullptr;
            }
         }
      }

   inline ~SharedPtr( ) noexcept
      {
      reset( );
      }

   inline T* get( ) const noexcept
      {
      return obj;
      }
private:
   T* obj;
   Pair<int, int>* counter; // first counts num shared ptrs, second counts num weak ptrs

   inline SharedPtr( T* obj_, Pair<int, int>* counter_ ) : obj( obj_ ), counter( counter_ )
      {
      ++counter->first;
      }

   friend class WeakPtr<T>;
};

template <typename T, typename... Args>
SharedPtr<T> MakeShared( Args... args )
   {
   return SharedPtr<T>( new T{ args... } ); // TODO optimize manager / object caching.
   }

}; // namespace fb
#endif /* SharedPtr_hpp */
