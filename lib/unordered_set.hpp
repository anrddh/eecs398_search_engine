//#pragma once

#include <functional>
#include <vector>

#define INITIAL_SIZE 1024

namespace fb {

    // A bucket's status tells you whether it's filled, empty, or contains a ghost.
    enum class Set_Status {
        Empty,
        Filled,
        Ghost
    };

    template<typename K, typename Hasher = std::hash<K>, typename Pred = std::equal_to<K>>
    class unordered_set {
    public:
        // A bucket has a status and a key
        struct Bucket {
            Set_Status status = Set_Status::Empty;
            K key;
        };

        //Exception type
        class out_of_range {};

        //Iterator type
        class Iterator {
        public:
            friend class unordered_set;
            Iterator() : current(nullptr), index(-1) {}
            Iterator(size_t index) : current(&buckets[index]), index(index) {
                if (index < buckets.size()) current = &buckets[index];
                else current = nullptr;
            }
            Iterator(const Iterator &other) : current(other.current), index(other.index) {}
            Iterator& operator=(const Iterator &rhs) {
                if (rhs != *this) {
                    current = rhs.current;
                    index = rhs.index;
                }
                return *this;
            }

            Iterator& operator++() {
                index++;
                if (index < buckets.size()) current = &buckets[index];
                else current = nullptr;
                return *this;
            }

            Iterator& operator--() {
                index--;
                current = &buckets[index];
                return *this;
            }

            K& operator*() {
                return current->key;
            }

            K* operator->() {
                return &current->key;
            }

            bool operator==(const Iterator &other) const {
                return !(current || other.current) || (current == other.current && index == other.index);
            }

            bool operator!=(const Iterator &other) const {
                return !(*this == other );
            }

        private:
            Bucket *current;
            size_t index;
        };

        //Default constructor
        unordered_set() {
            buckets.resize(INITIAL_SIZE);
        }

        //Constructor that takes custom size, hash, and predicate
        unordered_set(size_t sz, Hasher hash, Pred pred) : hash(hash), pred(pred) {
            buckets.resize(sz);
        }

        //Returns the number of elements in the map
        size_t size() const {
            return num_elements;
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
            if (buckets[desired_bucket].status != Set_Status::Empty) {
                //search until an empty bucket
                while (buckets[desired_bucket].status != Set_Status::Empty) {
                    //if a bucket has the key, return
                    if (buckets[desired_bucket].status == Set_Status::Filled && pred(buckets[desired_bucket].key, key)) {
                        return buckets[desired_bucket].key;
                    }
                    desired_bucket = (desired_bucket + 1) % buckets.size();
                }

                //key does not exist, so find first available bucket and add it
                while (true) {
                    if (buckets[original_hash].status == Set_Status::Filled) {
                        original_hash = (original_hash + 1) % buckets.size();
                    }
                    else {
                        if (buckets[original_hash].status == Set_Status::Ghost) {
                            num_ghosts--;
                        }
                        buckets[original_hash].key = key;
                        buckets[original_hash].status = Set_Status::Filled;
                        num_elements++;
                        return buckets[original_hash].key;
                    }
                }
            }
            else {
                //bucket is empty, so add key
                buckets[original_hash].key = key;
                buckets[original_hash].status = Set_Status::Filled;
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
            if (buckets[desired_bucket].status != Set_Status::Empty) {
                //search until an empty bucket
                while (buckets[desired_bucket].status != Set_Status::Empty) {
                    //if a bucket has the key, return
                    if (buckets[desired_bucket].status == Set_Status::Filled && pred(buckets[desired_bucket].key, key)) {
                        return buckets[desired_bucket].val;
                    }
                    desired_bucket = (desired_bucket + 1) % buckets.size();
                }
            }
            //key does not exist, so throw out of range exception
            throw out_of_range;
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
            if (buckets[desired_bucket].status != Set_Status::Empty) {
                //search until an empty bucket
                while (buckets[desired_bucket].status != Set_Status::Empty) {
                    //if a bucket has the key, return
                    if (buckets[desired_bucket].status == Set_Status::Filled && pred(buckets[desired_bucket].key, key)) {
                        return false;
                    }
                    desired_bucket = (desired_bucket + 1) % buckets.size();
                }

                //key does not exist, so find first available bucket and add it
                while (true) {
                    if (buckets[original_hash].status == Set_Status::Filled) {
                        original_hash = (original_hash + 1) % buckets.size();
                    }
                    else {
                        if (buckets[original_hash].status == Set_Status::Ghost) {
                            num_ghosts--;
                        }
                        buckets[original_hash].key = key;
                        buckets[original_hash].status = Set_Status::Filled;
                        num_elements++;
                        return true;
                    }
                }
            }
            else {
                //bucket is empty, so add key
                buckets[original_hash].key = key;
                buckets[original_hash].status = Set_Status::Filled;
                num_elements++;
                return true;
            }

        }
        // erase returns the number of items remove (0 or 1)
        size_t erase(const K& key) {
            size_t desired_bucket = hash(key) % buckets.size();
            //if the original bucket is empty, return
            if (buckets[desired_bucket].status == Set_Status::Empty) {
                return 0;
            }
            else {
                //if key at original bucket matches, remove and return
                if (buckets[desired_bucket].status == Set_Status::Filled && pred(buckets[desired_bucket].key, key)) {
                    buckets[desired_bucket].status = Set_Status::Ghost;
                    num_elements--;
                    num_ghosts++;
                    return 1;
                }
                else {
                    //search until an empty bucket
                    while (buckets[desired_bucket].status != Set_Status::Empty) {
                        //if a bucket has the key, remove and return
                        if (buckets[desired_bucket].status == Set_Status::Filled && pred(buckets[desired_bucket].key, key)) {
                            buckets[desired_bucket].status = Set_Status::Ghost;
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
                i.status = Set_Status::Empty;
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
            if (buckets[n].status == Set_Status::Filled) return 1;
            return 0;
        }
        //Returns the index of the bucket for the given key
        size_t bucket(const K k) {
            return hash(k) % buckets.size();
        }
        //Returns the current load factor
        float load_factor() {
            return bucket_count / num_elements;
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
        std::vector<Bucket> buckets;
        Hasher hash = std::hash<K>();
        Pred pred = std::equal_to<K>();

        void rehash_and_grow(size_t n) {
            std::vector<Bucket> temp = buckets;
            buckets.clear();
            num_elements = 0;
            num_ghosts = 0;
            buckets.resize(n);
            for (size_t i = 0; i < temp.size(); i++) {
                if (temp[i].status == Set_Status::Filled) {
                    insert(temp[i].key);
                }
            }
        }
    };

} //fb