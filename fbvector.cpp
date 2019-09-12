//Sharon Ye 9-12-2019
//implementation of STL vector


namespace fb{
	template <typename T>

	class Vector{
		private:
			size_t capacity;
			size_t size;
			T *arr;
		public:
			struct out_of_bounds_exception{};

			//default constructor
			Vector(){
				arr = new T[1];
				size = 0;
				capacity = 1;
			}

			//fill constructor
			Vector(size_t n, const T val){
				arr = new T[n];
				size = n;
				capacity = n;
			}

			//range constructor
			typedef T* iterator;
			Vector(iterator first, iterator last){
				size = last-first;
				capacity = size;
				arr = new T[size];
				for(size_t i = 0; i < size; ++i){
					arr[i] = *(first + i);
				}
			}

			//copy constructor
			Vector(const Vector &v){
				arr = new T[v.size];
				size = v.size;
				capacity = v.size;
				for(size_t i = 0; i < v.size; ++i){
					arr[i] = v[i];
				}
			}

			//TO DO: move and initializer constructor?

			//copy assignment operator
			Vector& operator=(const Vector &v){
				if(&v == this){
					return *this;
				}
				delete[] arr;
				arr = new T[v.size];
				size = v.size;
				capacity = v.size;
				for(size_t i = 0; i < v.size; ++i){
					arr[i] = v[i];
				}
				return *this;
			}

			//TO DO: move and init list assignment operator

			//destructor
			~Vector(){
				delete[] arr;
			}


			//TO DO: iterators

			bool empty(){
				if(size == 0){
					return true;
				}
				return false;
			}

			size_t capacity(){
				return capacity;
			}

			size_t size(){
				return size;
			}

			//TO DO: max_size()

			T& operator[](size_t n){
				return arr[n];
			}

			T& at(size_t n){
				if(n >= size){
					throw out_of_bounds_exception;
				}
				return arr[n];
			}

			void pop_back(){
				--size;
			}

			void push_back(T val){
				if(size == capacity){
					capacity = 2*capacity;
					T* p = new[capacity];
					for(size_t i = 0; i < size; ++i){
						p[i] = arr[i];
					}
					p[size] = val;
					delete[] arr;
					arr = p;
					++size;
					return;
				}
				arr[size] = val;
				++size;
			}

			void reserve(size_t n){
				if(n > capacity){
					T* p = new[n];
					for(size_t i = 0; i < size; ++i){
						p[i] = arr[i];
					}
					capacity = n;
					delete[] arr;
					arr = p;
				}
			}

			void resize(size_t n, T val = T()){
				if(n < size){
					for(size_t i = 0; i <= size-n; ++i){
						pop_back();
					}
				}

				if(n > size && n <= capacity){
					for(size_t i = size; i < n; ++i){
						arr[i] = val;
					}
				}

				if(n > size && n > capacity){
					T* p = new T[n];
					for(size_t i = 0; i < size; ++i){
						p[i] = arr[i];
					}
					for(size_t i = size; i < n; ++i){
						p[i] = val;
					}
					size = n;
					capacity = n;
					delete[] arr;
					arr = p;
				}
			}

			void shrink_to_fit(){
				capacity = size;
			}

			T& front(){
				return arr[0];
			}

			T& back(){
				return arr[size-1];
			}

			T* data(){
				return arr;
			}

			void assign(size_t n, const T val){
				resize(n, val);
			}

			void assign(iterator first, iterator last){
				size = last-first;
				capacity = size;
				delete[] arr;
				arr = new T[size];
				for(size_t i = 0; i < size; ++i){
					arr[i] = *(first + i);
				}
			}

			void clear(){
				delete[] arr;
				size = 0;
				capacity = 0;
				arr = new T[1];
			}

			//TO DO: insert, erase, emplace, emplace_back

			void swap(Vector<T> &v){
				T* temp = v.arr;
				size_t temp_size = v.size;
				size_t temp_capacity = v.capacity;
				v.arr = arr;
				v.size = size;
				v.capacity = capacity;
				arr = temp;
				size = temp_size;
				capacity = temp_capacity;
			}
	};

	//relational operators
	bool operator==(const Vector<T> &lhs, const Vector<T> &rhs){
		if(lhs.size == rhs.size){
			for(size_t i = 0; i < lhs.size; ++i){
				if(lhs.arr[i] != rhs.arr[i]){
					return false;
				}
			}
			return true;
		}
		return false;
	}

	bool operator!=(const Vector<T> &lhs, const Vector<T> &rhs){
		return !(lhs == rhs);
	}

	bool operator<(const Vector<T> &lhs, const Vector<T> &rhs){
		size_t smaller;
		if(lhs.size < rhs.size){
			smaller = lhs.size;
		}else{
			smaller = rhs.size;
		}

		for(size_t i = 0; i < smaller; ++i){
			if(lhs.arr[i] >= rhs.arr[i]){
				return false;
			}
		}
		//will only exit loop if all elements are equal up to the smaller vector
		if(lhs.size <= rhs.size){
			return true;
		}

		return false;
	}

	bool operator>(const Vector<T> &lhs, const Vector<T> &rhs){
		return (rhs < lhs);
	}

	bool operator<=(const Vector<T> &lhs, const Vector<T> &rhs){
		return !(lhs > rhs);
	}

	bool operator>=(const Vector<T> &lhs, const Vector<T> &rhs){
		return !(lhs < rhs);
	}

	void swap(Vector<T> &lhs, Vector<T> &rhs){
		T* temp = lhs.arr;
		size_t temp_size = lhs.size;
		size_t temp_capacity = lhs.size;
		lhs.arr = rhs.arr;
		lhs.size = rhs.size;
		lhs.capacity = rhs.capacity;
		rhs.arr = temp;
		rhs.size = temp_size;
		rhs.capacity = temp_capacity;
	}
}