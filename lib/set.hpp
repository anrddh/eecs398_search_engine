#pragma once

#include "functional.hpp"
#include <queue>
#include <iostream>

namespace FBL
   {
   namespace detail
      {
      enum class Color : unsigned char
         {
         BLACK = 0,
         RED = 1
         };
      template< typename T >
      struct Node
         {
         public:
            template< class... Args >
            Node( Args&&... args ) : val(std::forward<Args>(args)...), left(nullptr), right(nullptr), parent(nullptr), color(Color::RED) { }
            Node( T t ) : val( t ), left( nullptr ), right( nullptr ), parent( nullptr ), color(Color::RED) { }
            Node( ) : left( nullptr ), right( nullptr ), parent( nullptr ), color(Color::RED) { }


            T val;
            Node< T > * left, * right, * parent;
            Color color;
         };

      template< typename T >
      Node< T > * findNext( Node< T > * n )
         {
         if ( n )
            {
            if ( n->right )
               {
               return findFirst( n->right );
               }
            else
               {
               return findRightAncestor( n );
               }
            }
         else
            {
            return n;
            }
         }

      template< typename T >
      Node< T > * findPrevious( Node< T > * n )
         {
         if ( n )
            {

            if ( n->left )
               {
               return findLast( n->left );
               }
            else
               {
               return findLeftAncestor( n );
               }
            }
         else
            {
            return n;
            }
         }

      template< typename T >
      Node< T > * findFirst( Node< T > * n )
         {
         if( !n->left )
            {
            return n;
            }
         else
            {
            return findFirst( n->left );
            }
         }

      template< typename T >
      Node< T > * findLast( Node< T > * n )
         {
         if( !n->right )
            {
            return n;
            }
         else
            {
            return findLast( n->right );
            }
         }

      template< typename T >
      Node< T > * findRightAncestor( Node< T > * n )
         {
         if( !n->parent )
            {
            return nullptr;
            }
         if ( n == n->parent->left )
            {
            return n->parent;
            }
         else
            {
            return findRightAncestor( n->parent );
            }
         }

      template<typename T>
      Node<T> * findLeftAncestor( Node<T> * n )
         {
         if( !n->parent )
            {
            return nullptr;
            }
         if ( n == n->parent->right )
            {
            return n->parent;
            }
         else
            {
            return findLeftAncestor( n->parent );
            }
         }
      }

   template< typename T, typename Compare = Less< T > > // Compare(a, b) returns true if a should come before b
   class Set
      {
      public:
         class Iterator
            {
            public:
               friend class Set;
               Iterator( detail::Node< T > * root_, detail::Node< T > * current_ ) : root( root_ ), current( current_ ) { }
               Iterator( ) : root( nullptr ), current( nullptr ) { }
               Iterator( const Iterator &other) : root( other.root ), current( other.current ) { }
               Iterator& operator=( const Iterator &rhs )
                  {
                  if ( rhs != *this )
                     {
                     root = rhs.root;
                     current = rhs.current;
                     }
                  return *this;
                  }

               Iterator& operator++( )
                  {
                  current = detail::findNext( current );
                  return  *this;
                  }

               Iterator& operator++( int )
                  {
                  Iterator& old = *this;
                  this->operator++();
                  return old;
                  }

               Iterator& operator--( )
                  {
                  current = detail::findPrevious( current );
                  return *this;
                  }

               Iterator& operator--( int )
                  {
                  Iterator& old = *this;
                  this->operator--();
                  return old;
                  }

               T& operator*( )
                  {
                  return current->val;
                  }
               T* operator->( )
                  {
                  return &current->val;
                  }

               bool operator==( const Iterator& other ) const
                  {
                  return root == other.root && current == other.current;
                  }

               bool operator!=( const Iterator& other ) const
                  {
                  return !( *this == other );
                  }

            private:
               detail::Node<T> * root;
               detail::Node<T> * current;
            };

         Set( ) : sz(0), root(nullptr)  { }
         Set( Set &other ) : sz(0), root(nullptr)
            {
            for ( T val : other )
               {
               insert( val );
               }
            }

         Set& operator=( Set &rhs )
            {
               clear( );
               for ( T val : rhs )
                  {
                  insert( val );
                  }
            }

         bool empty( )
            {
            return !sz;
            }
         int size( )
            {
            return size;
            }

         Iterator insert( T val )
            {
               if ( !root )
                  {
                  ++sz;
                  root = new detail::Node< T >( val );
                  root->color = detail::Color::BLACK;
                  return Iterator(root, root);
                  }
               else
                  {
                  return insertImpl(root, val);
                  }
            }

         template< class... Args >
         std::pair< Iterator, bool > emplace( Args&&... args )
            {
               detail::Node< T > * emplNode = new detail::Node< T >( std::forward< Args >( args )... );
               if ( !root )
                  {
                  ++sz;
                  root = emplNode;
                  return std::make_pair( Iterator( root, root ), true );
                  }
               else
                  {
                  std::pair< Iterator, bool > result = emplaceImpl( root, emplNode );
                  if( !result.second )
                     {
                     delete emplNode;
                     }
                  return result;
                  }
            }

         Iterator erase( T val )
            {
            if( !root  )
               {
               return Iterator( nullptr, nullptr );
               }

            if( !comp( val, root->val ) && !comp( root->val, val ) )
               {
               root = eraseNode( root );
               root->parent = nullptr;
               return Iterator( root, detail::findNext( root ) );
               }

            if( comp( val, root->val ) )
               {
               return eraseImpl(root, val, true);
               }
            else
               {
               return eraseImpl(root, val, false);
               }
            }

         Iterator erase( const Iterator &iter )
         {
         detail::Node< T > * erase = iter.current;
         if(erase == root)
            {
            root = eraseNode( root );
            root->parent = nullptr;
            return Iterator( root, detail::findNext( root ) );
            }
         else
            {
            detail::Node< T > * parent = erase->parent;
            if ( parent->left == erase )
               {
               parent->left = eraseNode( erase );
               if ( parent->left )
                  {
                  parent->left->parent = parent;
                  }
               return Iterator( root, parent );
               }
            else
               {
               parent->right = eraseNode( erase );
               if ( parent->right )
                  {
                  parent->right->parent = parent;
                  }
               return Iterator( root, detail::findNext( parent ) );
               }
            }
         }

         Iterator find( T val )
            {
            if( !root )
               {
               return Iterator( nullptr, nullptr );
               }
            else
               {
               return findImpl( root, val );
               }
            }

         Iterator begin( )
            {
            return Iterator( root, detail::findFirst( root ) );
            }

         Iterator end( )
            {
            return Iterator( root, nullptr );
            }

         void swap( Set &other )
            {
            Set temp = other;
            other = *this;
            *this = other;
            }

         void clear( )
            {
               deleteTree( );
               sz = 0;
               root = nullptr;
            }

      private:
         int sz;
         detail::Node< T > * root;
         Compare comp;

         Iterator insertImpl( detail::Node< T > * node, T val )
            {
            if( !comp( val, node->val ) && !comp( node->val, val ) )
               {
               return Iterator( root, node );
               }
            else
               {
               if ( comp( val, node->val ) )
                  {
                  if ( node->left )
                     {
                     return insertImpl( node->left, val );
                     }
                  else
                     {
                     return Iterator( root, insertNode( node, val, true ) );
                     }
                  }
               else
                  {
                  if( node->right )
                     {
                     return insertImpl( node->right, val );
                     }
                  else
                     {
                     return Iterator( root, insertNode( node, val, false ) );
                     }
                  }
               }
            }

         detail::Node< T > * insertNode( detail::Node< T > * node, T val, bool left )
            {
            ++sz;
            detail::Node< T > * newNode = new detail::Node< T >( val );
            newNode->parent = node;
            if ( left )
               {
               node->left = newNode;
               }
            else
               {
               node->right = newNode;
               }
            return newNode;
            }

         std::pair<Iterator, bool> emplaceImpl( detail::Node< T > * node, detail::Node< T > * emplNode )
            {
            if( !comp( emplNode->val, node->val ) && !comp( node->val, emplNode->val ) )
               {
               return std::make_pair( Iterator( root, node ), false );
               }
            else
               {
               if ( comp( emplNode->val, node->val ) )
                  {
                  if ( node->left )
                     {
                     return emplaceImpl( node->left, emplNode );
                     }
                  else
                     {
                     return std::make_pair( Iterator( root, emplaceNode( node, emplNode, true ) ), true);
                     }
                  }
               else
                  {
                  if( node->right )
                     {
                     return emplaceImpl( node->right, emplNode );
                     }
                  else
                     {
                     return std::make_pair( Iterator( root, emplaceNode( node, emplNode, false ) ), true);
                     }
                  }
               }
            }

            detail::Node< T > * emplaceNode( detail::Node< T > * node, detail::Node< T > * emplNode, bool left )
            {
            ++sz;
            emplNode->parent = node;
            if ( left )
               {
               node->left = emplNode;
               }
            else
               {
               node->right = emplNode;
               }
            return emplNode;
            }


         Iterator eraseImpl( detail::Node< T > * node, T val, bool left )
            {
            if( left )
               {
               if( !node->left )
                  {
                  // no value to erase, find correct Iterator to return
                  return Iterator( root, node );
                  }
               if ( !comp( val, node->left->val ) && !comp( node->left->val, val ) )
                  {
                  // do routine to erase this value
                  node->left = eraseNode( node->left );
                  if ( node->left )
                     {
                     node->left->parent = node;
                     }
                  return Iterator( root, node ) ;
                  }
               if ( comp( val, node->left->val ) )
                  {
                  return eraseImpl( node->left, val, true );
                  }
               else
                  {
                  return eraseImpl( node->left, val, false );
                  }
               }
            else
               {
               if( !node->right )
                  {
                  // no value to erase, find correct Iterator to return
                  return Iterator(root, detail::findNext( node ) );
                  }
               if( !comp( val, node->right->val ) && !comp( node->right->val, val ) )
                  {
                  // do routine to erase this value
                  node->right = eraseNode( node->right );
                  if(node->right)
                     {
                     node->right->parent = node;
                     }
                  return Iterator( root, detail::findNext( node ) );
                  }
               if ( comp( val, node->right->val ) )
                  {
                  return eraseImpl( node->right, val, true );
                  }
               else
                  {
                  return eraseImpl( node->right, val, false );
                  }
               }

            }

         detail::Node< T > * eraseNode( detail::Node< T > * node)
            {
            --sz;
            detail::Node< T > * output;
            if ( !node->left || !node->right )
               {
               if( node->left )
                  {
                  output = node->left;
                  }
               else
                  {
                  output = node->right;
                  }
               }
            else
               {
               output = detail::findPrevious( node );
               detail::Node< T > * parent = output->parent;
               if( parent == node )
                  {
                  output->right = node->right;
                  node->right->parent = output;
                  }
               else
                  {
                  parent->right = output->left;
                  if(parent->right)
                     {
                     parent->right->parent = parent;
                     }
                  output->left = node->left;
                  output->right = node->right;
                  node->left->parent = output;
                  node->right->parent = output;
                  }
               }

            delete node;
            return output;
            }

         Iterator findImpl( detail::Node< T > * node, T val)
            {
            if( !comp( val, node->val ) && !comp( node->val, val ) )
               {
               return Iterator( root, node );
               }
            else
               {
               if ( comp( val, node->val ) )
                  {
                  if ( node->left )
                     {
                     return findImpl( node->left, val );
                     }
                  else
                     {
                     return Iterator( root, nullptr );
                     }
                  }
               else
                  {
                  if( node->right )
                     {
                     return findImpl( node->right, val );
                     }
                  else
                     {
                     return Iterator( root, nullptr );
                     }
                  }
               }
            }

         void deleteTree( )
            {
            std::queue < detail::Node< T > * > nodes;
            nodes.push( root );
            while( !nodes.empty( ) )
               {
               detail::Node< T > * node = nodes.front( );
               nodes.pop();
               if( node )
                  {
                  nodes.push( node->left );
                  nodes.push( node->right );
                  delete node;
                  }
               }
            }
      };

   }
