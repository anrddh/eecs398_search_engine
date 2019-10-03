#pragma once

#include <iostream>

// Simple exception class for reporting String errors
struct String_exception {
	String_exception(const char* msg_) : msg(msg_)
		{}
	const char* msg;
};


class String {
public:
	// Default initialization is to contain an empty string with no
    // allocation.  If a non-empty C-string is supplied, this String
    // gets minimum allocation.
	String(const char *cstr_ = "") : buf_size(std::strlen(cstr_)) {
        if (!buf_size)
            return;

        alloc_mem(buf_size + 1);
        std::strcpy(buf, cstr_);
    }

	// The copy constructor initializes this String with the
	// original's data, and gets minimum allocation.
	String(const String &original) : buf_size(original.size()) {
        if (!buf_size)
            return;

        alloc_mem(buf_size + 1);
        std::strcpy(buf, original.c_str());
    }

    // Move constructor - take original's data, and set the original
    // String member variables to the empty state (do not initialize
    // "this" String and swap).
    String(String &&original) noexcept
        : buf(original.buf),
          buf_size(original.buf_size),
          alloc(original.alloc)  {
        original.buf = &a_null_byte;
        original.buf_size = 0;
        original.alloc = 0;
    }

	// deallocate C-string memory
	~String() noexcept {
        if (alloc)
            delete[] buf;
    }

	// Assignment operators
	// Left-hand side gets a copy of rhs data and gets minimum
	// allocation.
	// This operator use the copy-swap idiom for assignment.
	String & operator= (const String &rhs) {
        String temp(rhs);
        swap(temp);
        return *this;
    }

 	// This operator creates a temporary String object from the rhs
 	// C-string, and swaps the contents
	String & operator= (const char *rhs) {
        String temp(rhs);
        swap(temp);
        return *this;
    }

   // Move assignment - simply swaps contents with rhs without any
   // copying
    String & operator= (String &&rhs) noexcept {
        swap(rhs);
        return *this;
    }

	// Accesssors
	// Return a pointer to the internal C-string
	const char * c_str() const {
        return buf;
    }

	// Return size (length) of internal C-string in this String
	int size() const {
        return buf_size;
    }

	// Return current allocation for this String
	int get_allocation() const {
        return alloc;
    }

	// Return a reference to character i in the string.  Throw
	// exception if 0 <= i < size is false.
	char & operator[] (int i) {
        if (i < 0 || i >= size())
            throw String_exception("Subscript out of range");
        return buf[i];
    }

	const char & operator[] (int i) const {	// const version for const Strings
        if (i < 0 || i >= size())
            throw String_exception("Subscript out of range");
        return buf[i];
    }


	/* Return a String starting with i and extending for len
	characters The substring must be contained within the string.
	Values of i and len for valid input are as follows:
	i >= 0 && len >= 0 && i <= size && (i + len) <= size.
	If both i = size and len = 0, the input is valid and the result is
	an empty string.  Throw exception if the input is invalid. */
	String substring(int i, int len) const {
        if (i < 0 || len < 0 || i > buf_size || (i + len) > buf_size)
            throw String_exception("Substring bounds invalid");

        String substr;
        substr.alloc_mem(len + 1);
        for (int j = 0; j < len; ++j)
            substr += buf[j + i];
        return substr;
    }

	// Modifiers Set to an empty string with minimum allocation by
	// create/swap with an empty string.
	void clear() {
        String temp("");
        swap(temp);
    }

	/* Remove the len characters starting at i; allocation is
	unchanged.  The removed characters must be contained within the
	String.  Valid values for i and len are the same as for
	substring. */
    void remove(int i, int len) {
        if (i < 0 || len < 0 || i > size() || (i + len) > size())
            throw String_exception("Remove bounds invalid");

        for (int j = i + len; j < size(); ++j)
            buf[j - len] = buf[j];
        buf_size -= len;
        buf[buf_size] = 0;
    }

        /* Insert the supplied source String before character i of this
	String, pushing the rest of the contents back, reallocating as
	needed.  If i == size, the inserted string is added to the end of
	this String.  This function does not create any temporary String
	objects.  It either directly inserts the new data into this
	String's space if it is big enough, or allocates new space and
	copies in the old data with the new data inserted.

	This String retains the final allocation.
	Throw exception if 0 <= i <= size is false.

	The behavior of inserting a String into itself is not specified. */
	void insert_before(int i, const String &src) {
        if (!src.size())
            return;

        if (i < 0 || i > size())
            throw String_exception("Insertion point out of range");

        realloc_mem(src.size());

        for (int j = size() - 1 + src.size(); j > i; --j)
            buf[j] = buf[j - src.size()];

        for (int j = 0; j < src.size(); ++j)
            buf[i + j] = src[j];

        buf_size += src.size();
        buf[buf_size] = 0;
    }

	/* These concatenation operators add the rhs string data to the
	lhs object.  They do not create any temporary String objects. They
	either directly copy the rhs data into the lhs space if it is big
	enough to hold the rhs, or allocate new space and copy the old lhs
	data into it followed by the rhs data. The lhs object retains the
	final memory allocation. If the rhs is a null byte or an empty
	C-string or String, no change is made to lhs String. */
	String & operator += (char rhs) {
        realloc_mem(1);
        buf[buf_size++] = rhs;
        buf[buf_size] = 0;
        return *this;
    }

	String & operator += (const char *rhs) {
        realloc_mem(1);
        buf[buf_size++] = rhs;
        buf[buf_size] = 0;
        return *this;
    }

    String & operator += (const String &rhs) {
        return *this += rhs.c_str();
    }

	/* Swap the contents of this String with another one.  The member
	variable values are interchanged, along with the pointers to the
	allocated C-strings, but the two C-strings are neither copied nor
	modified. No memory allocation/deallocation is done. */
	void swap(String &other) noexcept {
        std::swap(buf_size, other.buf_size);
        std::swap(alloc, other.alloc);
        std::swap(buf, other.buf);
    }

private:
	static char a_null_byte;	// to hold a null byte for empty string representation

    // default alloc to null byte
    char *buf    { &a_null_byte };
    int buf_size { 0 };
    int alloc    { 0 };

    // Reallocate memory if we need to.
    void realloc_mem(int size = 0) {
        if (!size ||  alloc >= buf_size + size + 1)
            return;

        alloc_mem(2 * (buf_size + size + 1));
    }

    void alloc_mem(int new_alloc = 0) {
        char *new_buf = new char[new_alloc];

        if (alloc) {
            for (int i = 0; i < buf_size; ++i)
                new_buf[i] = buf[i];

            delete[] buf;
        }

        buf = new_buf;
        alloc = new_alloc;
    }
};

// non-member overloaded operators

// compare lhs and rhs strings; constructor will convert a C-string literal to a String.
// comparison is based on std::strcmp result compared to 0
bool operator== (const String& lhs, const String& rhs) {
    return std::strcmp(lhs.c_str(), rhs.c_str()) == 0;
}

bool operator!= (const String& lhs, const String& rhs) {
    return std::strcmp(lhs.c_str(), rhs.c_str()) != 0;
}

bool operator< (const String& lhs, const String& rhs) {
    return std::strcmp(lhs.c_str(), rhs.c_str()) < 0;
}

bool operator> (const String& lhs, const String& rhs) {
    return std::strcmp(lhs.c_str(), rhs.c_str()) > 0;
}

/* Concatenate a String with another String.  If one of the arguments
 is a C-string, the String constructor will automatically create a
 temporary String for it to match this function (inefficient, but
 instructive).  This automatic behavior would be disabled if the
 String constructor was declared "explicit".  This function constructs
 a copy of the lhs in a local String variable, then concatenates the
 rhs to it with operator +=, and returns it. */
String operator+ (const String &lhs, const String &rhs) {
    String temp(lhs);
    temp += rhs;
    return temp;
}

// Input and output operators and functions The output operator writes
// the contents of the String to the stream
std::ostream & operator<< (std::ostream &os, const String &str) {
    os << str.c_str();
    return os;
}

/* The input operator clears the supplied String, then starts reading
the stream.  It skips initial whitespace, then copies characters into
the supplied str until whitespace is encountered again. The
terminating whitespace remains in the input stream, analogous to how
string input normally works.  str is expanded as needed, and retains
the final allocation.  If the input stream fails, str contains
whatever characters were read. */
std::istream & operator>> (std::istream &is, String &str) {
    str.clear();

    // skip initial whitespace
    char c;
    while (is && (c = is.peek()) && isspace(c))
        is.get();

    while (is && (c = is.peek()) && !isspace(c))
        str += is.get();

    return is;
}

/* getline for String clears str to an empty String, then reads
characters into str until it finds a '\n'.  Similar to std::getline,
the newline character is consumed, but not stored in the String.
str's allocation is expanded as needed, and it retains the final
allocation.  If the input stream fails, str contains whatever
characters were read. */
std::istream & getline(std::istream &is, String &str) {
    str.clear();

    char c;
    while (is && (c = is.peek()) && c != '\n') {
        c = is.get();
        str += c;
    }

    if (c == '\n')
        is.get(); // read in last newline

    return is;
}

#endif
