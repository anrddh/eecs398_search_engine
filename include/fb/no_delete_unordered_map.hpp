//  Edited by Jaeyoon Kim and Jin Soo Ihm
#pragma once

#include "functional.hpp"
#include "vector.hpp"
#include <utility>

#define INITIAL_SIZE 1024

namespace fb {

// A bucket's status tells you whether it's filled, empty, or contains a ghost.
enum class MapStatus {
    Empty,
    Filled
};

template<typename K, typename V, typename Hasher = Hash<K>, typename Pred = EqualTo<K>>
class NoDeleteUnorderedMap {
    using Status = MapStatus;
public:
    friend class Iterator;
    // A bucket has a status, a key, and a value.
    struct Bucket {
        Status status = Status::Empty;
        K key;
        V val;
    };

    //Exception type
    class out_of_range {};

    //Iterator type
    class Iterator {
    public:
        friend class NoDeleteUnorderedMap;
        Iterator(Vector<Bucket> *owner) : owner(owner), index(owner->size()) {}
        Iterator(Vector<Bucket> *owner, SizeT index) : owner(owner), index(index) {
            if (index > owner->size()) index = owner->size();
        }
        Iterator& operator=(const Iterator &rhs) {
            if (rhs != *this) {
                owner = rhs.owner;
                index = rhs.index;
            }
            return *this;
        }

        Iterator& operator++() {
            while(index++, index < owner->size() && (*owner)[index].status != Status::Filled) ;
            return *this;
        }

        Iterator& operator--() {
            while(index--, index >= 0 && (*owner)[index].status != Status::Filled) ;
            return *this;
        }

        V& operator*() {
            return (*owner)[index].val;
        }

        const K& key() {
            return (*owner)[index].key;
        }

        V* operator->() {
            return &(*owner)[index].val;
        }

        bool operator==(const Iterator &other) const {
            return !(owner || other.owner) || (owner == other.owner && index == other.index);
        }

        bool operator!=(const Iterator &other) const {
            return !(*this == other);
        }

    private:
        Vector<Bucket> *owner;
        SizeT index;
    };

    //Default constructor
    NoDeleteUnorderedMap() {
        buckets.resize(INITIAL_SIZE);
    }

    //Constructor that takes custom size, hash, and predicate
    NoDeleteUnorderedMap(SizeT sz, Hasher hash, Pred pred) : hash(hash), pred(pred) {
        buckets.resize(sz);
    }

    //Returns the number of elements in the map
    SizeT size() const {
        return num_elements;
    }

    //Returns an iterator to the first element in the map
    Iterator begin() {
        SizeT count = 0;
        while (count < buckets.size() && buckets[count].status != Status::Filled)
            ++count;
        return Iterator(&buckets, count);
    }

    //Return an iterator "off the end" of the map
    Iterator end() {
        return Iterator(&buckets);
    }

    Iterator find(const K& key) {
        if(num_elements > buckets.size() * max_load){
            rehash_and_grow(buckets.size() * 2);
        }
        SizeT index = findPlaceToInsert( key );
        if ( buckets[index].status == Status::Empty )
            return end( );

        return Iterator( &buckets, index );
    }

    Iterator insert(K& key, V& val) {
        if(num_elements > buckets.size() * max_load){
            rehash_and_grow(buckets.size() * 2);
        }

        SizeT index = findPlaceToInsert( key );
        if ( buckets[index].status == Status::Empty )
        {
            buckets[ index ].key = key;
            buckets[ index ].val = val;
            buckets[ index ].status = Status::Filled;
            num_elements++;
        }

        return Iterator( &buckets, index );
     }

    //This function is for brave souls (or naive souls) only. It gives you the
    //power to change a key inside the hash table. This is likely foolish, and
    //you should make sure you have a very good reason for doing so
    Bucket &functionThatIsOnlyForJIaeyoonInThatOneSpecialCase(K& key){
        if(num_elements > buckets.size() * max_load){
            rehash_and_grow(buckets.size() * 2);
        }

        SizeT index = findPlaceToInsert( key );
        if( buckets[ index ].status == Status::Empty )
            {
            ++num_elements;
            }
        return buckets[ index ];
    }

    // returns a reference to the value in the bucket with the key, if it
    // already exists. Otherwise, insert it with a default value, and return
    // a reference to the resulting bucket.
    V& operator[](const K& key) 
        {
        if ( num_elements > buckets.size() * max_load )
            {
            rehash_and_grow(buckets.size() * 2);
            }

        SizeT index = findPlaceToInsert( key );
        if ( buckets[ index ].status == Status::Empty )
            {
            buckets[ index ].key = key;
            buckets[ index ].val = V{ };
            buckets[ index ].status = Status::Filled;
            num_elements++;
            }
        return buckets[ index ].val;
        }

    //Returns the number of buckets
    SizeT bucket_count() {
        return buckets.size();
    }
    //Returns the number of elements in bucket n
    SizeT bucket_size(SizeT n) {
        if (buckets[n].status == Status::Filled) return 1;
        return 0;
    }
    //Returns the index of the bucket for the given key
    SizeT bucket(const K k) {
        return hash(k) % buckets.size();
    }
    //Returns the current load factor
    float load_factor() {
        //LEAVE THIS VERSION WITH THE PARENTHESIS, WHY DOES IT KEEP REVERTING????
        return bucket_count() / num_elements;
    }
    //Returns the max load factor
    float max_load_factor() {
        return max_load;
    }
    //Sets the max load factor
    void max_load_factor(float z) {
        max_load = z;
    }
    //Sets the number of buckets in the container to n
    void rehash(SizeT n) {
        rehash_and_grow(n);
    }
    //Set the number of buckets to contain n elements
    void reserve(SizeT n) {
        rehash_and_grow(n);
    }
    //Returns the hash function
    Hasher hash_function() {
        return hash;
    }
    //Returns the predicate function
    Pred key_eq() {
        return pred;
    }

    // Moves this unordered map to vector
    // invalidates this object
    Vector<Pair<K, V>> convert_to_vector() {
       Vector<Pair<K, V>> vec;
       for (SizeT i = 0; i < buckets.size(); ++i) {
          if ( buckets[i].status == Status::Filled ) {
            vec.emplaceBack( std::move( buckets[i].key ), std::move( buckets[i].val ) );
          }
       }

       return vec;
    }
private:
    SizeT num_elements = 0;
    float max_load = 0.5;
    Vector<Bucket> buckets;
    Hasher hash = fb::Hash<K>();
    Pred pred = fb::EqualTo<K>();

    void rehash_and_grow(SizeT n) {
        Vector<Bucket> temp = std::move(buckets);
        buckets.clear();
        num_elements = 0;
        buckets.resize(n);
        for(SizeT i = 0; i < temp.size(); i++){
            if(temp[i].status == Status::Filled){
                insert(temp[i].key, temp[i].val);
            }
        }
    }

    SizeT findPlaceToInsert( const K& key )
        {
        SizeT desired_bucket = hash(key) % buckets.size();
         while(buckets[desired_bucket].status == Status::Filled){
             //if a bucket has the key, return
             if( pred(buckets[desired_bucket].key, key) )
               {
               return desired_bucket;
               }
             desired_bucket = (desired_bucket+1) % buckets.size();
             }
        return  desired_bucket;
        }
};

} //fb
