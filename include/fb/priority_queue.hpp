// Jin Soo Ihm September 10, 2019
#pragma once

//#include "vector.hpp"
#include "functional.hpp"
#include "vector.hpp"
#include <functional> //std::swap, std::move TODO

namespace fb {
template< typename T, typename Container = Vector< T >, typename Compare = Less< typename Container::ValueType > >
class PriorityQueue
{
private:
   Container data;
   Compare compare;

   void heapify(int i)
      {
      while( 2 * i + 1 < data.size() )
         {
         int left = i * 2 + 1;
         if ( left + 1 < data.size() && this->compare( data[ left ], data[ left + 1 ] ) )
            ++left;
         if( this->compare( data[ left ], data[ i ] ) )
            return;
         std::swap( data [ left ], data[ i ] );
         i = left;
         }
      }

   void push_heap()
      {
      int i = data.size() - 1;
      while( i >= 0 && this->compare( data[ (i - 1) / 2 ], data[ i ]))
         {
         std::swap( data[ i ], data[ ( i - 1 ) / 2 ] );
         i = ( i - 1 ) / 2;
         }
      return;
      }

public:

   typedef Container ContainerType;
   typedef typename Container::ValueType ValueType;
   typedef typename Container::SizeType SizeType;
   typedef typename Container::Reference Reference;
   typedef typename Container::ConstReference ConstReference;

   PriorityQueue ( )
      : data ( Container{ } ), compare( Compare{ } )
      {
      }

   PriorityQueue ( Compare comp )
      : data( Container{ } ), compare(comp)
      {
      }

   const ConstReference top() const
      {
      return data[0];
      }

   bool empty() const
      {
      return data.empty();
      }

   SizeType size() const
      {
      return data.size();
      }

   void push( const T& value )
      {
      data.push_back( value );
      push_heap();
      }

   void push( T && value )
      {
      data.pushBack( std::move( value ) );
      push_heap();
      }

   void pop()
      {
      if ( !empty() )
         {
         data.front( ) = data.back( );
         data.popBack( );
         heapify(0);
         }
      else
         {
         throw;
         }
      }

   void swap( PriorityQueue& other ) noexcept
      {
      std::swap( data, other.data );
      std::swap( compare, other.compare );
      }
};
}
