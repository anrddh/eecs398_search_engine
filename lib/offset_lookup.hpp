//Created by Chandler Meyers 11/6/2019
#pragma once

//This is essentially an unordered_map, but it does not store the keys in the buckets
//This is intended to look up a file offset, and comparisons will then be done using the lookup
#include "functional.hpp"
#include "url_pool.hpp"
//#include "string.hpp"
#include "vector.hpp"
#include <string>
#define String std::string

#define INITIAL_SIZE 1024

namespace fb {

// A bucket's status tells you whether it's filled, empty, or contains a ghost.
enum class OffsetStatus {
    Empty,
    Filled,
    Ghost
};

template<typename K = String, typename V = SizeT, typename Hasher = Hash<K>>
class OffsetLookupChunk {
    using Status = OffsetStatus;
public:
    friend class Iterator;
    // A bucket has a status, a key, and a value.
    struct Bucket {
        Status status = Status::Empty;
        V val;
    };

    //Exception type
    class out_of_range {};

    //Iterator type
    class Iterator {
    public:
        friend class OffsetLookupChunk;
        Iterator(Vector<Bucket> *owner) : owner(owner), index(owner->size()) {}
        Iterator(Vector<Bucket> *owner, size_t index) : owner(owner), index(index) {
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
        size_t index;
    };

    //Default constructor
    OffsetLookupChunk() {
        buckets.resize(INITIAL_SIZE);
    }

    //Returns the number of elements in the map
    size_t size() const {
        return num_elements;
    }

    //Returns an iterator to the first element in the map
    Iterator begin() {
        int count = 0;
        while (count < buckets.size() && buckets[count].status != Status::Filled) count++;
        return Iterator(&buckets, count);
    }

    //Return an iterator "off the end" of the map
    Iterator end() {
        return Iterator(&buckets);
    }

    //Takes in a key, hash pair where the hash is the hash of the key
    //It only looks at the hash, and finds that object. If it isnt found, it then
    //creates the object using the key. In either case, it returns an iterator to the
    //object.
    V& find(const K& key, SizeT hash){
        if(num_elements+num_ghosts > buckets.size() * max_load){
            rehash_and_grow(buckets.size() * 2);
        }
        size_t desired_bucket = hash % buckets.size();
        size_t original_hash = desired_bucket;
        //if the bucket is not empty
        if(buckets[desired_bucket].status != Status::Empty){
            //search until an empty bucket
            while(buckets[desired_bucket].status != Status::Empty){
                //if a bucket has the key, return
                if(buckets[desired_bucket].status == Status::Filled && (key == GlobalList->get_str(buckets[desired_bucket].val))){
                    return buckets[desired_bucket].val;
                }
                desired_bucket = (desired_bucket+1) % buckets.size();
            }

            //key does not exist, so find first available bucket and add it
            while(true){
                if(buckets[original_hash].status == Status::Filled){
                    original_hash = (original_hash+1) % buckets.size();
                }else{
                    if(buckets[original_hash].status == Status::Ghost){
                        num_ghosts--;
                    }
                    buckets[original_hash].val = GlobalList->add_str(key);
                    buckets[original_hash].status = Status::Filled;
                    num_elements++;
                    return buckets[original_hash].val;
                }
            }
        }else{
            //bucket is empty, so add key
            buckets[original_hash].val = GlobalList->add_str(key);
            buckets[original_hash].status = Status::Filled;
            num_elements++;
            return buckets[original_hash].val;
        }
    }

    // returns a reference to the value in the bucket with the key, if it
    // already exists. Otherwise, insert it with a default value, and return
    // a reference to the resulting bucket.
    V& operator[](const K& key) {
        if(num_elements+num_ghosts > buckets.size() * max_load){
            rehash_and_grow(buckets.size() * 2);
        }
        size_t desired_bucket = hash(key) % buckets.size();
        size_t original_hash = desired_bucket;
        //if the bucket is not empty
        if(buckets[desired_bucket].status != Status::Empty){
            //search until an empty bucket
            while(buckets[desired_bucket].status != Status::Empty){
                //if a bucket has the key, return
                if(buckets[desired_bucket].status == Status::Filled && (key == GlobalList->get_str(buckets[desired_bucket].val))){
                    return buckets[desired_bucket].val;
                }
                desired_bucket = (desired_bucket+1) % buckets.size();
            }

            //key does not exist, so find first available bucket and add it
            while(true){
                if(buckets[original_hash].status == Status::Filled){
                    original_hash = (original_hash+1) % buckets.size();
                }else{
                    if(buckets[original_hash].status == Status::Ghost){
                        num_ghosts--;
                    }
                    buckets[original_hash].val = GlobalList->add_str(key);
                    buckets[original_hash].status = Status::Filled;
                    num_elements++;
                    return buckets[original_hash].val;
                }
            }
        }else{
            //bucket is empty, so add key
            buckets[original_hash].val = GlobalList->add_str(key);
            buckets[original_hash].status = Status::Filled;
            num_elements++;
            return buckets[original_hash].val;
        }
    }

    V& at(const K& key) {
        if (num_elements + num_ghosts > buckets.size() * max_load) {
            rehash_and_grow();
        }
        size_t desired_bucket = hash(key) % buckets.size();
        size_t original_hash = desired_bucket;
        //if the bucket is not empty
        if (buckets[desired_bucket].status != Status::Empty) {
            //search until an empty bucket
            while (buckets[desired_bucket].status != Status::Empty) {
                //if a bucket has the key, return
                if (buckets[desired_bucket].status == Status::Filled && (key == GlobalList->get_str(buckets[desired_bucket].val))) {
                    return buckets[desired_bucket].val;
                }
                desired_bucket = (desired_bucket + 1) % buckets.size();
            }
        }
        //key does not exist, so throw out of range exception
        throw out_of_range();
    }

    // insert returns whether inserted successfully
    // (if the key already exists in the table, do nothing and return false).
    bool insert(const K& key, const V& val) {
        if(num_elements+num_ghosts > buckets.size() * max_load){
            rehash_and_grow(buckets.size() * 2);
        }
        size_t desired_bucket = hash(key) % buckets.size();
        size_t original_hash = desired_bucket;
        //if the bucket is not empty
        if(buckets[desired_bucket].status != Status::Empty){
            //search until an empty bucket
            while(buckets[desired_bucket].status != Status::Empty){
                //if a bucket has the key, return
                if(buckets[desired_bucket].status == Status::Filled && (key == GlobalList->get_str(buckets[desired_bucket].val))){
                    return false;
                }
                desired_bucket = (desired_bucket+1) % buckets.size();
            }

            //key does not exist, so find first available bucket and add it
            while(true){
                if(buckets[original_hash].status == Status::Filled){
                    original_hash = (original_hash+1) % buckets.size();
                }else{
                    if(buckets[original_hash].status == Status::Ghost){
                        num_ghosts--;
                    }
                    buckets[original_hash].val = val;
                    buckets[original_hash].status = Status::Filled;
                    num_elements++;
                    return true;
                }
            }
        }else{
            //bucket is empty, so add key
            buckets[original_hash].val = val;
            buckets[original_hash].status = Status::Filled;
            num_elements++;
            return true;
        }

    }
    // erase returns the number of items remove (0 or 1)
    size_t erase(const K& key) {
        size_t desired_bucket = hash(key) % buckets.size();
        //if the original bucket is empty, return
        if(buckets[desired_bucket].status == Status::Empty){
            return 0;
        }else{
            //if key at original bucket matches, remove and return
            if(buckets[desired_bucket].status == Status::Filled && (key == GlobalList->get_str(buckets[desired_bucket].val))){
                buckets[desired_bucket].status = Status::Ghost;
                num_elements--;
                num_ghosts++;
                return 1;
            }else{
                //search until an empty bucket
                while(buckets[desired_bucket].status != Status::Empty){
                    //if a bucket has the key, remove and return
                    if(buckets[desired_bucket].status == Status::Filled && (key == GlobalList->get_str(buckets[desired_bucket].val))){
                        buckets[desired_bucket].status = Status::Ghost;
                        num_elements--;
                        num_ghosts++;
                        return 1;
                    }
                    desired_bucket = (desired_bucket+1) % buckets.size();
                }

                //no matching key found
                return 0;
            }
        }
    }
    //Sets all buckets to empty, leaving map with size of 0
    void clear() {
        for (auto& i : buckets) {
            i.status = Status::Empty;
        }
        num_elements = 0;
        num_ghosts = 0;
    }
    //Returns the number of buckets
    size_t bucket_count() {
        return buckets.size();
    }
    //Returns the number of elements in bucket n
    size_t bucket_size(size_t n) {
        if (buckets[n].status == Status::Filled) return 1;
        return 0;
    }
    //Returns the index of the bucket for the given key
    size_t bucket(const K k) {
        return hash(k) % buckets.size();
    }
    //Returns the current load factor
    float load_factor() {
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
    void rehash(size_t n) {
        rehash_and_grow(n);
    }
    //Set the number of buckets to contain n elements
    void reserve(size_t n) {
        rehash_and_grow(n);
    }
    //Returns the hash function
    Hasher hash_function() {
        return hash;
    }
private:
    size_t num_elements = 0;
    size_t num_ghosts = 0;
    float max_load = 0.5;
    Vector<Bucket> buckets;
    Hasher hash = fb::Hash<K>();
    SavedStrings *GlobalList;

    void rehash_and_grow(size_t n) {
        Vector<Bucket> temp = buckets;
        buckets.clear();
        num_elements = 0;
        num_ghosts = 0;
        buckets.resize(n);
        for(size_t i = 0; i < temp.size(); i++){
            if(temp[i].status == Status::Filled){
                insert(GlobalList->get_str(temp[i].val), temp[i].val);
            }
        }
    }
};

} //fb
