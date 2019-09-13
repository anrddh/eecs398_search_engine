//Sharon Ye 9-12-2019
//implementation of STL vector
namespace fb{
	typedef unsigned long size_t;
	template <typename T>

	class Vector{
		private:
			size_t cap;
			size_t _size;
			T *arr;
		public:
			struct out_of_bounds_exception{};

			//default constructor
			Vector(){
				arr = new T[1];
				_size = 0;
				cap = 1;
			}

			//fill constructor
			Vector(size_t n, const T val){
				arr = new T[n];
				_size = n;
				cap = n;
			}

			//range constructor
			typedef T* iterator;
			Vector(iterator first, iterator last){
				_size = last-first;
				cap = _size;
				arr = new T[_size];
				for(size_t i = 0; i < _size; ++i){
					arr[i] = *(first + i);
				}
			}

			//copy constructor
			Vector(const Vector &v){
				arr = new T[v._size];
				_size = v._size;
				cap = v._size;
				for(size_t i = 0; i < v._size; ++i){
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
				arr = new T[v._size];
				_size = v._size;
				cap = v._size;
				for(size_t i = 0; i < v._size; ++i){
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
				if(_size == 0){
					return true;
				}
				return false;
			}

			size_t capacity(){
				return cap;
			}

			size_t size(){
				return _size;
			}

			//TO DO: max__size()

			T& operator[](size_t n){
				return arr[n];
			}

			T& at(size_t n){
				if(n >= _size){
					throw out_of_bounds_exception();
				}
				return arr[n];
			}

			void pop_back(){
				--_size;
			}

			//TO DO: fix initialized memory
			void push_back(T val){
				if(_size == cap){
					cap = 2*cap;
					T* p = new T[cap];
					for(size_t i = 0; i < _size; ++i){
						p[i] = arr[i];
					}
					p[_size] = val;
					delete[] arr;
					arr = p;
					++_size;
					return;
				}
				arr[_size] = val;
				++_size;
			}

			//TO DO: fix initialized memory
			void reserve(size_t n){
				if(n > cap){
					T* p = new T[n];
					for(size_t i = 0; i < _size; ++i){
						p[i] = arr[i];
					}
					cap = n;
					delete[] arr;
					arr = p;
				}
			}

			void re_size(size_t n, T val = T()){
				if(n < _size){
					for(size_t i = 0; i <= _size-n; ++i){
						pop_back();
					}
				}

				if(n > _size && n <= cap){
					for(size_t i = _size; i < n; ++i){
						arr[i] = val;
					}
				}

				if(n > _size && n > cap){
					T* p = new T[n];
					for(size_t i = 0; i < _size; ++i){
						p[i] = arr[i];
					}
					for(size_t i = _size; i < n; ++i){
						p[i] = val;
					}
					_size = n;
					cap = n;
					delete[] arr;
					arr = p;
				}
			}

			void shrink_to_fit(){
				cap = _size;
			}

			T& front(){
				return arr[0];
			}

			T& back(){
				return arr[_size-1];
			}

			T* data(){
				return arr;
			}

			void assign(size_t n, const T val){
				re_size(n, val);
			}

			void assign(iterator first, iterator last){
				_size = last-first;
				cap = _size;
				delete[] arr;
				arr = new T[_size];
				for(size_t i = 0; i < _size; ++i){
					arr[i] = *(first + i);
				}
			}

			void clear(){
				delete[] arr;
				_size = 0;
				cap = 0;
				arr = new T[1];
			}

			//TO DO: insert, erase, emplace, emplace_back

			void swap(Vector<T> &v){
				T* temp = v.arr;
				size_t temp__size = v._size;
				size_t temp_cap = v.cap;
				v.arr = arr;
				v._size = _size;
				v.cap = cap;
				arr = temp;
				_size = temp__size;
				cap = temp_cap;
			}
	};

	template <typename T>
	//relational operators
	bool operator==(const Vector<T> &lhs, const Vector<T> &rhs){
		if(lhs._size == rhs._size){
			for(size_t i = 0; i < lhs._size; ++i){
				if(lhs.arr[i] != rhs.arr[i]){
					return false;
				}
			}
			return true;
		}
		return false;
	}

	template <typename T>
	bool operator!=(const Vector<T> &lhs, const Vector<T> &rhs){
		return !(lhs == rhs);
	}

	template <typename T>
	bool operator<(const Vector<T> &lhs, const Vector<T> &rhs){
		size_t smaller;
		if(lhs._size < rhs._size){
			smaller = lhs._size;
		}else{
			smaller = rhs._size;
		}

		for(size_t i = 0; i < smaller; ++i){
			if(lhs.arr[i] >= rhs.arr[i]){
				return false;
			}
		}
		//will only exit loop if all elements are equal up to the smaller vector
		if(lhs._size <= rhs._size){
			return true;
		}

		return false;
	}

	template <typename T>
	bool operator>(const Vector<T> &lhs, const Vector<T> &rhs){
		return (rhs < lhs);
	}

	template <typename T>
	bool operator<=(const Vector<T> &lhs, const Vector<T> &rhs){
		return !(lhs > rhs);
	}

	template <typename T>
	bool operator>=(const Vector<T> &lhs, const Vector<T> &rhs){
		return !(lhs < rhs);
	}

	template <typename T>
	void swap(Vector<T> &lhs, Vector<T> &rhs){
		T* temp = lhs.arr;
		size_t temp__size = lhs._size;
		size_t temp_cap = lhs._size;
		lhs.arr = rhs.arr;
		lhs._size = rhs._size;
		lhs.cap = rhs.cap;
		rhs.arr = temp;
		rhs._size = temp__size;
		rhs.cap = temp_cap;
	}
}