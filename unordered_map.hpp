//#pragma once

#include <functional>
#include <vector>

#define INITIAL_SIZE 4

namespace fb {
   
/*
 * An implementation of the unordered_map type from the C++ Standard
 * Template Library.
 */
 
// An element in the unordered_map. Implemented as a node in a linked list
template < class Key, class T > class unordered_map_element 
{
public:
   Key key;
   T value;
   bool in_use;
   unordered_map_element< Key, T > *next;

   unordered_map_element( ) : in_use(false), next(nullptr) { }
   unordered_map_element( Key key, T value ) : key(key), value(value), in_use(true), next(nullptr) { }
};

template < class Key, class T, class Hash = std::hash< Key >,
class Pred = std::equal_to< Key > > class unordered_map 
{
private:
   int numBuckets;
   int numElements;
   Hash hash;
   Pred pred;
   std::vector< unordered_map_element< Key, T > * > data;
   
   int getIndex( Key key )
   {
      return hash( key ) % numBuckets;
   }
   
public:   
   //default constructor
   explicit unordered_map( ) 
   {
      numBuckets = INITIAL_SIZE;
      numElements = 0;
      data.resize( numBuckets ); 
      hash = std::hash< Key >( );
      pred = std::equal_to< Key >( );
   }
      
   //constructor that takes initial size
   explicit unordered_map( int sz ) 
   {
      numBuckets = sz;
      numElements = 0;
      data.resize( numBuckets );
      hash = std::hash< Key >( );
      pred = std::equal_to< Key >( );
   }
   
   explicit unordered_map( int sz, Hash hash, Pred pred )
   {
      numBuckets = sz;
      numElements = 0;
      data.resize( numBuckets );
      hash = hash;
      pred = pred;
   }
   
   explicit unordered_map( Hash hash, Pred pred )
   {
      numBuckets = INITIAL_SIZE;
      numElements = 0;
      data.resize( numBuckets );
      hash = hash;
      pred = pred;
   }
   
   ~unordered_map( )
   {
      for ( int i = 0;  i < numBuckets;  ++i )
      {
         unordered_map_element< Key, T > *current = data[ i ];
         while ( current )
         {
            unordered_map_element< Key, T > *next = current->next;
            delete current;
            current = next;
         }
      }
   }
   
   // Returns the number of elements in the unordered_map
   int size( ) { return numElements; } 
   
   // Returns whether the unordered_map is empty
   bool isEmpty( ) { return numElements == 0; }
   
   // Removes an element from the map, returns value
   // If key not found, returns default value
   T remove( Key key )
   {
      int index = getIndex( key );
      unordered_map_element< Key, T > *current = data[ index ];
      //if ( !current->in_use ) current = nullptr;
      unordered_map_element< Key, T > *previous = nullptr;
      while ( current )
      {
         if ( pred( current->key, key ) ) break;
         previous = current;
         current = current->next;
      }
      if ( !current ) return T( );
      --numElements;
      if ( previous ) previous->next = current->next;
      else data[ index ] = current->next;
      
      T return_value = current->value;
      delete current;
      return return_value;
   }
   
   // Helper for subscript operator, returns a pointer to the value for the given key
   // Should probably become private, that's a TODO
   T *get ( Key key )
   {
      int index = getIndex( key );
      unordered_map_element< Key, T > *current = data[ index ];
      while ( current )
      {
         if ( pred( current->key, key ) ) return &(current->value);
         current = current->next;
      }
      return nullptr;
   }
   
   // Adds an element to the unordered_map
   T &add ( Key key, T value )
   {
      //If gets too big, resize. For now 0.75
      if ((float)numElements / numBuckets >= 0.75)
      {
         std::vector< unordered_map_element< Key, T > * > temp = data;
         data = std::vector< unordered_map_element< Key, T > * >();
         numBuckets = 2 * numBuckets;
         numElements = 0;
         data.resize(numBuckets);
         for (int i = 0; i < (int)temp.size(); ++i)
         {
            unordered_map_element< Key, T > *current = temp[i];
            while (current)
            {
               add(current->key, current->value);
               unordered_map_element< Key, T > *next = current->next;
               delete current;
               current = next;
            }
         }
      }

      int index = getIndex( key );
      unordered_map_element< Key, T > *current = data[ index ];
      //if ( !current->in_use ) current = nullptr;
      while ( current )
      {
         if ( pred( current->key, key ) )
         {
            current->value = value;
            return current->value;
         }
         current = current->next;
      }
      ++numElements;
      current = data[ index ];
      //if ( !current->in_use ) current = nullptr;
      unordered_map_element< Key, T > *new_element = new unordered_map_element< Key, T >( key, value );
      new_element->next = current;
      data[ index ] = new_element;
      return new_element->value;
   }
   
   // Returns a reference to the value associated to a key
   // If key is not found, adds it, then returns the reference
   T &operator [ ]( Key key )
   {
      T *current = get( key );
      if ( current ) return *current;
      
      return add( key, T( ) );
   }
   
   //The below lines are not valid
   //inline iterator begin() noexcept { return data.begin(); }
   //inline const_iterator cbegin() const noexcept { return data.cbegin(); }
   //inline iterator end() noexcept { return data.end(); }
   //inline const_iterator cend() const noexcept { return data.cend(); }

      
};

} //fb