//Edited by Chandler Meyers on 11/6/2019
#pragma once

#include "functional.hpp"
#include "vector.hpp"

#define INITIAL_SIZE 1024

namespace fb {

// A bucket's status tells you whether it's filled, empty, or contains a ghost.
enum class SetStatus {
    Empty,
    Filled,
    Ghost
};

template<typename K, typename Hasher = Hash<K>, typename Pred = EqualTo<K>>
class UnorderedSet {
    using Status = SetStatus;
public:
    friend class Iterator;
    // A bucket has a status and a key
    struct Bucket {
        SetStatus status = SetStatus::Empty;
        K key;
    };

    //Exception type
    class out_of_range {};

    //Iterator type
    class Iterator {
    public:
        friend class UnorderedSet;
        Iterator(Vector<Bucket> *owner) : owner(owner), index(owner->size()) {}
        Iterator(Vector<Bucket> *owner, size_t index_) : owner(owner), index(index_) {
            //if (index > owner->size()) index = owner->size();
            if (owner->empty()) return;
            index_ = index_ % owner->size();
            index = index_;
            //counter protects us from infinitely looping through an empty set
            int counter = 0;
            while((*owner)[index].status != Status::Filled && counter < owner->size()) {
                counter++;
                index++;
                index = index % owner->size();
            }
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

        K& operator*() {
            return (*owner)[index].key;
        }

        K* operator->() {
            return &(*owner)[index].key;
        }

        bool operator==(const Iterator &other) const {
            return !(owner || other.owner) || (owner == other.owner && index == other.index);
        }

        bool operator!=(const Iterator &other) const {
            return !(*this == other );
        }

    private:
        Vector<Bucket> *owner;
        size_t index;
    };

    //Default constructor
    UnorderedSet() {
        buckets.resize(INITIAL_SIZE);
    }

    //Constructor that takes custom size, hash, and predicate
    UnorderedSet(size_t sz, Hasher hash, Pred pred) : hash(hash), pred(pred) {
        buckets.resize(sz);
    }

    //Returns the number of elements in the map
    size_t size() const {
        return num_elements;
    }

    //Returns an iterator to the first element in the set
    Iterator begin() {
        int count = 0;
        while (count < buckets.size() && buckets[count].status != Status::Filled) count++;
        return Iterator(&buckets, count);
    }

    //Return an iterator "off the end" of the set
    Iterator end() {
        return Iterator(&buckets);
    }

    //Return an iterator to a given offset (really the next full bucket from the offset)
    Iterator Jaeyoon(SizeT offset){
        return Iterator(&buckets, offset);
    }


    // returns a reference to the value in the bucket with the key, if it
    // already exists. Otherwise, insert it with a default value, and return
    // a reference to the resulting bucket.
    K& operator[](const K& key) {
        if (num_elements + num_ghosts > buckets.size() * max_load) {
            rehash_and_grow(buckets.size() * 2);
        }
        size_t desired_bucket = hash(key) % buckets.size();
        size_t original_hash = desired_bucket;
        //if the bucket is not empty
        if (buckets[desired_bucket].status != SetStatus::Empty) {
            //search until an empty bucket
            while (buckets[desired_bucket].status != SetStatus::Empty) {
                //if a bucket has the key, return
                if (buckets[desired_bucket].status == SetStatus::Filled && pred(buckets[desired_bucket].key, key)) {
                    return buckets[desired_bucket].key;
                }
                desired_bucket = (desired_bucket + 1) % buckets.size();
            }

            //key does not exist, so find first available bucket and add it
            while (true) {
                if (buckets[original_hash].status == SetStatus::Filled) {
                    original_hash = (original_hash + 1) % buckets.size();
                }
                else {
                    if (buckets[original_hash].status == SetStatus::Ghost) {
                        num_ghosts--;
                    }
                    buckets[original_hash].key = key;
                    buckets[original_hash].status = SetStatus::Filled;
                    num_elements++;
                    return buckets[original_hash].key;
                }
            }
        }
        else {
            //bucket is empty, so add key
            buckets[original_hash].key = key;
            buckets[original_hash].status = SetStatus::Filled;
            num_elements++;
            return buckets[original_hash].key;
        }
    }

    K& at(const K& key) {
        if (num_elements + num_ghosts > buckets.size() * max_load) {
            rehash_and_grow();
        }
        size_t desired_bucket = hash(key) % buckets.size();
        size_t original_hash = desired_bucket;
        //if the bucket is not empty
        if (buckets[desired_bucket].status != SetStatus::Empty) {
            //search until an empty bucket
            while (buckets[desired_bucket].status != SetStatus::Empty) {
                //if a bucket has the key, return
                if (buckets[desired_bucket].status == SetStatus::Filled && pred(buckets[desired_bucket].key, key)) {
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
    bool insert(const K& key) {
        if (num_elements + num_ghosts > buckets.size() * max_load) {
            rehash_and_grow(buckets.size() * 2);
        }
        size_t desired_bucket = hash(key) % buckets.size();
        size_t original_hash = desired_bucket;
        //if the bucket is not empty
        if (buckets[desired_bucket].status != SetStatus::Empty) {
            //search until an empty bucket
            while (buckets[desired_bucket].status != SetStatus::Empty) {
                //if a bucket has the key, return
                if (buckets[desired_bucket].status == SetStatus::Filled && pred(buckets[desired_bucket].key, key)) {
                    return false;
                }
                desired_bucket = (desired_bucket + 1) % buckets.size();
            }

            //key does not exist, so find first available bucket and add it
            while (true) {
                if (buckets[original_hash].status == SetStatus::Filled) {
                    original_hash = (original_hash + 1) % buckets.size();
                }
                else {
                    if (buckets[original_hash].status == SetStatus::Ghost) {
                        num_ghosts--;
                    }
                    buckets[original_hash].key = key;
                    buckets[original_hash].status = SetStatus::Filled;
                    num_elements++;
                    return true;
                }
            }
        }
        else {
            //bucket is empty, so add key
            buckets[original_hash].key = key;
            buckets[original_hash].status = SetStatus::Filled;
            num_elements++;
            return true;
        }

    }
    // erase returns the number of items remove (0 or 1)
    size_t erase(const K& key) {
        size_t desired_bucket = hash(key) % buckets.size();
        //if the original bucket is empty, return
        if (buckets[desired_bucket].status == SetStatus::Empty) {
            return 0;
        }
        else {
            //if key at original bucket matches, remove and return
            if (buckets[desired_bucket].status == SetStatus::Filled && pred(buckets[desired_bucket].key, key)) {
                buckets[desired_bucket].status = SetStatus::Ghost;
                num_elements--;
                num_ghosts++;
                return 1;
            }
            else {
                //search until an empty bucket
                while (buckets[desired_bucket].status != SetStatus::Empty) {
                    //if a bucket has the key, remove and return
                    if (buckets[desired_bucket].status == SetStatus::Filled && pred(buckets[desired_bucket].key, key)) {
                        buckets[desired_bucket].status = SetStatus::Ghost;
                        num_elements--;
                        num_ghosts++;
                        return 1;
                    }
                    desired_bucket = (desired_bucket + 1) % buckets.size();
                }

                //no matching key found
                return 0;
            }
        }
    }
    //Sets all buckets to empty, leaving map with size of 0
    void clear() {
        for (auto& i : buckets) {
            i.status = SetStatus::Empty;
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
        if (buckets[n].status == SetStatus::Filled) return 1;
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
    //Returns the predicate function
    Pred key_eq() {
        return pred;
    }
private:
    size_t num_elements = 0;
    size_t num_ghosts = 0;
    float max_load = 0.5;
    Vector<Bucket> buckets;
    Hasher hash = Hash<K>();
    Pred pred = EqualTo<K>();

    void rehash_and_grow(size_t n) {
        Vector<Bucket> temp = buckets;
        buckets.clear();
        num_elements = 0;
        num_ghosts = 0;
        buckets.resize(n);
        for (size_t i = 0; i < temp.size(); i++) {
            if (temp[i].status == SetStatus::Filled) {
                insert(temp[i].key);
            }
        }
    }
};

} //fb