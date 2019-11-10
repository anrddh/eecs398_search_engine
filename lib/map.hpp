#include "set.hpp"
#include "utilities.hpp"
#include "functional.hpp"

namespace fb
   {
   template<typename K, typename V, typename Comp>
   class MapCompare {
   public:
      bool operator()(const Pair<K, V> &lhs, const Pair<K, V> &rhs)
         {
         return Comp(lhs, rhs);
         }

   private:
      Comp comp;
   };

   template<typename K, typename V, typename Comp = Less < K > >
   class Map : public Set<Pair<K, V>, MapCompare<K, V, Comp> >
      {
      public:
         Iterator find(const K &key) const
            {
            Pair<K, V> p = make_pair(key, V());
            return find(p);
            }

         Iterator erase(const K &key)
            {
            Pair<K, V> p = make_pair(key, V());
            return erase(p);
            }

         V& operator[](const K &key)
            {
            Pair<K, V> p = make_pair(key, V());
            Iterator iter = find(p);
            if(iter == end())
               {
               iter = insert(p);
               }
            return iter->second;
            }
      };

   }
