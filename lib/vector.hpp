typedef unsigned long size_t;
template<typename T>
class Vector
   {
   private:
      size_t cap = 0;
      size_t _size = 0;
      T *arr = nullptr;
   public:
      // Default Constructor
      // REQUIRES: Nothing
      // MODIFIES: *this
      // EFFECTS: Constructs an empty vector with capacity 0
      Vector( ) { }

      // Destructor
      // REQUIRES: Nothing
      // MODIFIES: Destroys *this
      // EFFECTS: Performs any neccessary clean up operations
      ~Vector( )
         {
         delete[ ] arr;
         }

      // Resize Constructor
      // REQUIRES: Nothing
      // MODIFIES: *this
      // EFFECTS: Constructs a vector with size n,
      //    all default constructed
      Vector( size_t n ) : cap( n ), _size( n ), arr( new T[ n ]( ) )
         {}

      // Fill Constructor
      // REQUIRES: Capacity > 0
      // MODIFIES: *this
      // EFFECTS: Creates a vector with size n, all assigned to val
      Vector( size_t n, const T& val )
            : cap( n ), _size ( n ), arr ( new T[ n ] )
         {
         for ( size_t i = 0;  i < _size;  ++i )
            arr[ i ] = val;
         }

      // Copy Constructor
      // REQUIRES: Nothing
      // MODIFIES: *this
      // EFFECTS: Creates a clone of the vector v
      Vector( const Vector<T>& v )
      {
         arr = new T[v._size];
         _size = v._size;
         cap = v._size;
         for(size_t i = 0; i < v._size; ++i){
            arr[i] = v[i];
         }
      }

      // Assignment operator
      // REQUIRES: Nothing
      // MODIFIES: *this
      // EFFECTS: Duplicates the state of v to *this
      Vector operator=( const Vector<T>& v )
         {
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

      // Move Constructor
      // REQUIRES: Nothing
      // MODIFIES: *this, leaves v in a default constructed state
      // EFFECTS: Takes the data from v into a newly constructed vector
      Vector( Vector<T>&& v )
         {
            _size = v._size;
            cap = v.cap;
            arr = v.arr;
            v.arr = nullptr;
            v._size = 0;
            v.cap = 0;
         }

      // Move Assignment Operator
      // REQUIRES: Nothing
      // MODIFIES: *this, leaves vin a default constructed state
      // EFFECTS: Takes the data from v in constant time
      Vector operator=( Vector<T>&& v )
         {
            if(arr){
               delete[] arr;
            }
            _size = v._size;
            cap = v.cap;
            arr = v.arr;
            v.arr = nullptr;
            v._size = 0;
            v.cap = 0;
         }

      // REQUIRES: new_capacity > capacity()
      // MODIFIES: capacity()
      // EFFECTS: Ensures that the vector can contain size() = new_capacity
      //    elements before having to reallocate
      void reserve( size_t n )
         {
         if(n > cap) {
            T* p = new T[n];
            for(size_t i = 0; i < _size; ++i){
               p[i] = arr[i];
            }
            cap = n;
            delete[] arr;
            arr = p;
         }
         }

      // REQUIRES: new_capacity > capacity()
      // MODIFIES: capacity()
      // EFFECTS: Ensures that the vector can contain size() = new_capacity
      //    elements before having to reallocate
      void resize( size_t n )
         {
         if ( n <= cap )
            {
            _size = n;
            return;
            }
         else
            {
            T* p = new T[ n ];

            for( size_t i = 0;  i < _size;  ++i )
               p[ i ] = arr[ i ];

            cap = n;
            _size = n;
            delete[ ] arr;
            arr = p;
            }
         }

      // REQUIRES: Nothing
      // MODIFIES: Nothing
      // EFFECTS: Returns the number of elements in the vector
      size_t size( ) const
         {
         return _size;
         }

      // REQUIRES: Nothing
      // MODIFIES: Nothing
      // EFFECTS: Returns the maximum size the vector can attain before resizing
      size_t capacity( ) const
         {
         return cap;
         }

      // REQUIRES: 0 <= i < size()
      // MODIFIES: Allows modification of data[i]
      // EFFECTS: Returns a mutable reference to the i'th element
      T& operator[ ] ( size_t i )
         {
            return arr[i];
         }

      // REQUIRES: 0 <= i < size()
      // MODIFIES: Nothing
      // EFFECTS: Get a const reference to the ith element
      const T& operator[ ] ( size_t i ) const
         {
            return arr[i];
         }

      // REQUIRES: Nothing
      // MODIFIES: this, size(), capacity()
      // EFFECTS: Appends the element x to the vector, allocating
      //    additional space if neccesary
      void pushBack( const T& val )
         {
            if(_size == cap){
               if(cap == 0){
                  ++cap;
               }
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

      // REQUIRES: Nothing
      // MODIFIES: this, size()
      // EFFECTS: Removes the last element of the vector,
      //    leaving capacity unchanged
      void popBack( )
         {
         --_size;
         }

      // REQUIRES: Nothing
      // MODIFIES: Allows mutable access to the vector's contents
      // EFFECTS: Returns a mutable random access iterator to the
      //    first element of the vector
      T* begin( )
         {
         return arr;
         }

      // REQUIRES: Nothing
      // MODIFIES: Allows mutable access to the vector's contents
      // EFFECTS: Returns a mutable random access iterator to
      //    one past the last valid element of the vector
      T* end( )
         {
         return arr+_size;
         }

      // REQUIRES: Nothing
      // MODIFIES: Nothing
      // EFFECTS: Returns a random access iterator to the first element of the vector
      const T* begin( ) const
         {
         return arr;
         }

      // REQUIRES: Nothing
      // MODIFIES: Nothing
      // EFFECTS: Returns a random access iterator to
      //    one past the last valid element of the vector
      const T* end( ) const
         {
         return arr+_size;
         }
   };
