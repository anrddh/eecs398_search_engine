//Edited by Chandler Meyers on 11/6/2019
#pragma once

#include <fb/functional.hpp>
#include <fb/vector.hpp>
#include <iostream>

#define USET_INITIAL_SIZE 65536

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
        Iterator(Vector<Bucket> *owner, SizeT index_) : owner(owner), index(index_) {
            //if (index > owner->size()) index = owner->size();
            if (owner->empty()) return;
            index_ = index_ % owner->size();
            index = index_;
            //counter protects us from infinitely looping through an empty set
            SizeT counter = 0;
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
        SizeT index;
    };

    //Default constructor
    UnorderedSet() {
        buckets.resize(USET_INITIAL_SIZE);
    }

    //Constructor that takes custom size, hash, and predicate
    UnorderedSet(SizeT sz, Hasher hash, Pred pred) : hash(hash), pred(pred) {
        buckets.resize(sz);
    }

    //Returns the number of elements in the map
    SizeT size() const {
        return num_elements;
    }

    //Returns an iterator to the first element in the set
    Iterator begin() {
        fb::SizeT count = 0;
        while (count < buckets.size() && buckets[count].status != Status::Filled) count++;
        return Iterator(&buckets, count);
    }

    //Return an iterator "off the end" of the set
    Iterator end() {
        return Iterator(&buckets);
    }

    Iterator find(const K& key) {
        if(num_elements+num_ghosts > buckets.size() * max_load){
            rehash_and_grow(buckets.size() * 2);
        }
        SizeT desired_bucket = hash(key) % buckets.size();
        //if the bucket is not empty
        if(buckets[desired_bucket].status != Status::Empty){
            //search until an empty bucket
            while(buckets[desired_bucket].status != Status::Empty){
                //if a bucket has the key, return
                if(buckets[desired_bucket].status == Status::Filled && pred(buckets[desired_bucket].key, key)){
                    return Iterator(&buckets, desired_bucket);
                }
                desired_bucket = (desired_bucket+1) % buckets.size();
            }
        }
        //bucket is empty, so return end
        return end();
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
        SizeT desired_bucket = hash(key) % buckets.size();
        SizeT original_hash = desired_bucket;

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
            rehash_and_grow(buckets.size() * 2);
        }

        SizeT desired_bucket = hash(key) % buckets.size();
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

        SizeT desired_bucket = hash(key) % buckets.size();
        SizeT original_hash = desired_bucket;
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
    SizeT erase(const K& key) {
        SizeT desired_bucket = hash(key) % buckets.size();
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
    SizeT bucket_count() {
        return buckets.size();
    }
    //Returns the number of elements in bucket n
    SizeT bucket_size(SizeT n) {
        if (buckets[n].status == SetStatus::Filled) return 1;
        return 0;
    }
    //Returns the index of the bucket for the given key
    SizeT bucket(const K k) {
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
private:
    SizeT num_elements = 0;
    SizeT num_ghosts = 0;
    float max_load = 0.5;
    Vector<Bucket> buckets;
    Hasher hash = Hash<K>();
    Pred pred = EqualTo<K>();

    void rehash_and_grow(SizeT n) {
        std::cout << "rehash_and_grow" << std::endl;
        Vector<Bucket> temp = buckets;
        buckets.clear();
        num_elements = 0;
        num_ghosts = 0;
        buckets.resize(n);

        for (SizeT i = 0; i < temp.size(); i++) {
            if (temp[i].status == SetStatus::Filled) {
                insert(temp[i].key);
            }
        }
    }
};

} //fb
