#pragma once

#include "stddef.hpp"
#include "iterator.hpp"
#include "memory/unique_ptr.hpp"

namespace fb {

    template <typename T,
              typename Allocator = >
    class Vector {
    public:
        using ValueType = T;
        using SizeType = SizeT;
        using DifferenceType = PtrDiffT;
        using Reference = T &;
        using ConstReference = const Reference;
        using Pointer = T *;
        using ConstPointer = const Pointer;
        using Iterator = Pointer;
        using ConstIterator = const Iterator;
        using ReverseIterator = fb::ReverseIterator<Iterator>;
        using ConstReverseIterator = const ReverseIterator;

        // Default Constructor
        // REQUIRES: Nothing
        // MODIFIES: *this
        // EFFECTS: Constructs an empty vector with capacity 0
        Vector( ) { }

        Vector(SizeT n, const T &val) : cap(n), _size(n), arr(new T[n]) {
            for (SizeT i = 0; i < _size; ++i)
                arr[ i ] = val;
         }

        Vector(SizeT n)
            : cap(n), _size(n), arr(new T[n]()) {}

        // Copy Constructor
        // REQUIRES: Nothing
      // MODIFIES: *this
      // EFFECTS: Creates a clone of the vector v
      Vector( const Vector<T>& v )
      {
         arr = new T[v._size];
         _size = v._size;
         cap = v._size;
         for (SizeT i = 0; i < v._size; ++i) {
            arr[i] = v[i];
         }
      }

      // Assignment operator
      // REQUIRES: Nothing
      // MODIFIES: *this
      // EFFECTS: Duplicates the state of v to *this
      Vector operator=( const Vector<T>& v )
         {
            if(&v == this){
               return *this;
            }
            delete[] arr;
            arr = new T[v._size];
            _size = v._size;
            cap = v._size;
            for (SizeT i = 0; i < v._size; ++i) {
               arr[i] = v[i];
            }
            return *this;
         }

      // Move Constructor
      // REQUIRES: Nothing
      // MODIFIES: *this, leaves v in a default constructed state
      // EFFECTS: Takes the data from v into a newly constructed vector
      Vector( Vector<T>&& v )
         {
            _size = v._size;
            cap = v.cap;
            arr = v.arr;
            v.arr = nullptr;
            v._size = 0;
            v.cap = 0;
         }

      // Move Assignment Operator
      // REQUIRES: Nothing
      // MODIFIES: *this, leaves vin a default constructed state
      // EFFECTS: Takes the data from v in constant time
      Vector operator=( Vector<T>&& v )
         {
            if(arr){
               delete[] arr;
            }
            _size = v._size;
            cap = v.cap;
            arr = v.arr;
            v.arr = nullptr;
            v._size = 0;
            v.cap = 0;
         }

        ~Vector() = default;


      // REQUIRES: new_capacity > capacity()
      // MODIFIES: capacity()
      // EFFECTS: Ensures that the vector can contain size() = new_capacity
      //    elements before having to reallocate
      void reserve( SizeT n )
         {
         if(n > cap) {
            T* p = new T[n];
            for (SizeT i = 0; i < _size; ++i) {
               p[i] = arr[i];
            }
            cap = n;
            delete[] arr;
            arr = p;
         }
         }

      // REQUIRES: new_capacity > capacity()
      // MODIFIES: capacity()
      // EFFECTS: Ensures that the vector can contain size() = new_capacity
      //    elements before having to reallocate
      void resize( SizeT n )
         {
         if ( n <= cap )
            {
            _size = n;
            return;
            }
         else
            {
            T* p = new T[ n ];

            for( SizeT i = 0;  i < _size;  ++i )
               p[ i ] = arr[ i ];

            cap = n;
            _size = n;
            delete[ ] arr;
            arr = p;
            }
         }

      // REQUIRES: Nothing
      // MODIFIES: Nothing
      // EFFECTS: Returns the number of elements in the vector
      [[nodiscard]] constexpr SizeType size( ) const noexcept
         {
         return size_;
         }

      // REQUIRES: Nothing
      // MODIFIES: Nothing
      // EFFECTS: Returns the maximum size the vector can attain before resizing
      [[nodiscard]] constexpr SizeType capacity( ) const noexcept
         {
         return cap;
         }

      [[nodiscard]] T& operator[ ] ( SizeT i )
         {
         return data()[i];
         }

      const T& operator[ ] ( SizeT i ) const
         {
         return data()[i];
         }

      // REQUIRES: Nothing
      // MODIFIES: this, size(), capacity()
      // EFFECTS: Appends the element x to the vector, allocating
      //    additional space if neccesary
      void pushBack( const T& val )
         {
            alloc_mem(size() + 1);
            if(_size == cap){
               if(cap == 0){
                  ++cap;
               }
               cap = 2*cap;
               T* p = new T[cap];
               for (SizeT i = 0; i < _size; ++i) {
                  p[i] = arr[i];
               }
               p[_size] = val;
               delete[] arr;
               arr = p;
               ++_size;
               return;
            }
            arr[_size] = val;
            ++_size;
         }

      // REQUIRES: Nothing
      // MODIFIES: this, size()
      // EFFECTS: Removes the last element of the vector,
      //    leaving capacity unchanged
      void popBack( )
         {
         --_size;
         }

      T* begin( )
         {
         return data();
         }

      T* end( )
         {
         return data() + size();
         }

      const T* begin( ) const
         {
         return data();
         }

      const T* end( ) const
         {
         return data() + size();
         }
    private:
        UniquePtr<T[]> buf;
        SizeType size_ = 0;
        SizeType cap_ = 0;

        void alloc_mem(SizeT mem) {
            if (cap_ >= mem)
                return;

            auto newCap = cap_ ? 3 * cap_ / 2 : mem;
            auto new_buf = makeUnique<char[]>(newCap * sizeof(T));
        }
   };

}
