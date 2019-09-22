#include "set.hpp"

template<typename K, typename V, typename Comp>
class MapCompare {
public:
   bool operator()(const std::pair<K, V> &lhs, const std::pair<K, V> &rhs) 
      {
      return Comp(lhs, rhs);
      }

private:
   Comp comp;
};

template<typename K, typename V, typename Comp = std::less < K > >
class Map : public Set<std::pair<K, v>, MapCompare<K, V, Comp> > 
   {
   public:
      Iterator find(const K &key) const
         {
         std::pair<K, V> p = make_pair(key, V());
         return find(p);
         }

      Iterator erase(const K &key) 
         {
         std::pair<K, V> p = make_pair(key, V());
         return erase(p);
         }

      V& operator[](const K &key) 
         {
         std::pair<K, V> p = make_pair(key, V());
         Iterator iter = find(p);
         if(iter == end()) 
            {
            iter = insert(p);
            }
         return iter->second;
         }
   };