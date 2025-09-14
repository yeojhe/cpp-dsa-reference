#pragma once

#include <cstddef>     // For std::size_t
#include <cstring>     // For std::strlen, std::memcpy
#include <stdexcept>   // For std::invalid_argument, std::out_of_range
#include <utility>     // For std::swap

// "Rule of Five" - when you define one of these five
// special member functions, you usually need to define all of them:
// 1. Destructor, 2. Copy constructor, 3. Copy assignment, 4. Move constructor, 5. Move assignment
//
// Key design decisions:
// - Null-terminated invariant: m_data[m_size] == '\0' (like C strings)
//      - an invariant is a condition that must always be true for the class to
//      work correctly
//      - it sets initial values that will be used if the constructor doesn't explicitly
//      initialize them
// - Rule of Five implemented for proper resource management
// - Strong exception safety on copy/assign via copy-and-swap idiom
/*
 // BAD: Not exception safe
String& operator=(const String& other) {
    delete[] m_data;           // Delete our old data first
    m_size = other.m_size;     // What if this line throws?
    m_data = new char[m_size]; // now in broken state
    // If new throws, we've lost our data and have invalid state
}

// GOOD: Strong exception safety with copy-and-swap
String& operator=(String other) noexcept {  // Parameter copied (might throw)
    swap(other);     // This cannot throw - just swapping pointers
    return *this;    // 'other' destructor cleans up our old data
}
If the copy (parameter creation) throws, our object is unchanged
If copy succeeds, swap cannot fail, so we're guaranteed to succeed
 */
// - Functions throw std::bad_alloc on allocation failure (standard behavior)

class String {
public:
//    Constructors and destructors - rule of five

// default constructor - creates empty string
// note: removed noexcept - calls new which might throw
    String()
            : m_data(new char[1]{'\0'}) // allocate 1 byte for just null terminator
            , m_size(0) // emptry string has 0 characters
            , m_capacity(0) {

    } // capacity 0 (excluding the terminator slot)
    // why allocate 1 byte for empty string? to maintain invariant that m_data is
    // never null and always points to a valid null-terminating string

// version with noexcept handling:
//    String() noexcept
//            : m_data(nullptr), m_size(0), m_capacity(0)
//    {
//        try {
//            m_data = new char[1]{'\0'};
//        } catch (...) {
//            // Can't throw from noexcept function, so terminate
//            std::terminate();
//        }
//    }

// constructor from c-string
// explicit prevents implicit conversion from const char* to String
    explicit String(const char *s) {
        if (!s) throw std::invalid_argument("String(const char*): null");

        // get length of input string
        const std::size_t n = std::strlen(s);
        // allocate exactly n+1 bytes (n chars + terminator)
        allocate_exact(n);
        // copy n characters (faster than strcpy for known length)
        std::memcpy(m_data, s, n);
        // add null terminator
        m_data[n] = '\0';
        // set size (m_capacity set by allocate_exact)
        m_size = n;
    }

    // copy constructor
    // creates a new String as a copy of another String
    String(const String &other) {
        // allocate exactly what we need
        allocate_exact(other.m_size);
        // copy data including null terminator
        std::memcpy(m_data, other.m_data, other.m_size + 1);
        // m_capacity set by allocate_exact
        m_size = other.m_size;
    }

    // move constructor
    // "steals" resources from another String, leaving it in valid but empty state
    // noexcept is crucial for container optimizations (std::vector will use copy if move can throw)
    String(String &&other) noexcept
            : m_data(other.m_data) // take ownership of other's buffer
            , m_size(other.m_size) // take other's size
            , m_capacity(other.m_capacity) // take other's capacity
    {
        // leave 'other' in a valid state (empty string with 1-byte buffer)
        other.m_data = new char[1]{'\0'};
        other.m_size = 0;
        other.m_capacity = 0;
        // note: other can still be destroyed safely, and methods can be called on it
    }

    // copy assignment operator
    // uses "copy-and-swap" idiom for strong exception safety
    // parameter passed *by value* (not by reference) - this creates a copy
    String &operator=(String other) noexcept {
        swap(other); // swap our resources with the copy's resources
        return *this; // the copy (now containing our old data) will be destroyed automatically
        // this technique provides strong exception safety:
        // - if copy construction fails, our object is unchanged
        // - if copy succeeds, swap cannot fail (noexcept)
    }

    // destructor
    // automatically called when object goes out of scope or is explicitly deleted
    ~String() {
        delete[] m_data; // free allocated memory
        // note: delete[] handles nullptr safely, and destructors should never throw
    }

    // observers (const methods that don't modify the object)

    // nodiscard: tells compiler to warn if the caller ignores the return value of a function
    [[nodiscard]] std::size_t size() const noexcept { return m_size; } // number of characters (excluding terminator)
    [[nodiscard]] std::size_t
    capacity() const noexcept { return m_capacity; } // max chars we can hold without reallocation
    [[nodiscard]] bool empty() const noexcept { return m_size == 0; } // check if string is empty
    [[nodiscard]] const char *c_str() const noexcept { return m_data; } // get C-string representation

    // element access

    // unchecked access - no bounds checking (like std::string)
    // non-const version allows modification: str[0] = 'X';
    // but also need const overloads for const correctness - the compiler prevents you from calling non-const
    // methods on const objects, even if you're not actually modifying anything
    char &operator[](std::size_t i) noexcept { return m_data[i]; }

    // const version for const String objects
    const char &operator[](std::size_t i) const noexcept { return m_data[i]; }

    // checked access - throws exception if index out of bounds
    char &at(std::size_t i) {
        if (i >= m_size) throw std::out_of_range("String::at");
        return m_data[i];
    }

    [[nodiscard]] const char &at(std::size_t i) const {
        if (i >= m_size) throw std::out_of_range("String::at");
        return m_data[i];
    }

    // modifiers

    // clear - make string empty but keep allocated buffer
    void clear() noexcept {
        m_size = 0; // reset size to 0
        m_data[0] = '\0'; // put null terminator at beginning
        // note: capacity unchanged - buffer is reusable
    }

    // assign - replace contents with new C-string
    void assign(const char *s) {
        if (!s) throw std::invalid_argument("assign: null");

        const std::size_t n = std::strlen(s);

        if (n > m_capacity) {
            // need more space - use strong exception safety
            String tmp(s); // create temporary String with new content (with the copy constructor)
            swap(tmp); // swap our resources with temporary
            return; // tmp (with our old data) destroyed automatically
        }

        // reuse existing buffer - fits in current capacity
        std::memcpy(m_data, s, n); // copy new content
        m_data[n] = '\0'; // add terminator
        m_size = n;
    }

    // push back - append single character
    void push_back(char c) {
        ensure_capacity_for(m_size + 1); // make sure we have space for one more char
        m_data[m_size++] = c; // add character and increase size
        m_data[m_size] = '\0'; // maintain null termination
    }

    // append - append C-string
    void append(const char *s) {
        if (!s) throw std::invalid_argument("append: null");

        const std::size_t n = std::strlen(s);
        // ensure capacity for current + new content
        ensure_capacity_for(m_size + n);
        // copy at the end of current content
        std::memcpy(m_data + m_size, s, n);
        // update size
        m_size += n;
        // maintain null termination
        m_data[m_size] = '\0';
    }

    // reserve - ensure capacity for at least new_cap characters
    void reserve(std::size_t new_cap) {
        // already have enough capacity
        if (new_cap <= m_capacity) return;

        // allocate new buffer with extra space for null terminator
        char *newbuf = new char[new_cap + 1];
        std::memcpy(newbuf, m_data, m_size + 1); // copy existing content + terminator
        delete[] m_data; // free old buffer
        m_data = newbuf; // update pointer
        m_capacity = new_cap; // update capacity
    }

    // shrink to fit = reduce capacity to match size (free unused memory)
    void shrink_to_fit() {
        if (m_capacity == m_size) return; // already minimal
        char *newbuf = new char[m_size + 1]; // allocate exactly what we need
        std::memcpy(newbuf, m_data, m_size + 1); // copy content + terminator
        delete[] m_data; // free old buffer
        m_data = newbuf; // update pointer
        m_capacity = m_size; // capacity now equals size
    }

    // swap - exchange contents with another String (noexcept crucial for copy-and-swap)
    // none of the operations throw exceptions, that means that any functions that use swap() can also be noexcept
    void swap(String &other) noexcept {
        using std::swap; // ADL (argument dependent lookup) - finds best swap
        swap(m_data, other.m_data); // swap pointers
        swap(m_size, other.m_size); // swap sizes
        swap(m_capacity, other.m_capacity); // swap capacities
    }

private:
    // pointer to allocated buffer (always null-terminated)
    char *m_data{}; // = char* m_data = nullptr;
    // number of characters in string (excluding terminator)
    std::size_t m_size{0}; // = std::size_t m_size = 0;
    // number of characters we can store (excluding terminator slot)
    std::size_t m_capacity{0}; // = std::size_t m_capacity = 0;

    // private helper functions

    // allocate buffer for exactly n characters (plus terminator)
    void allocate_exact(std::size_t n) {
        m_data = new char[n + 1]; // +1 for null terminator
        m_capacity = n; // capacity is usable character count
    }

    // ensure capacity for at least desired_size characters
    void ensure_capacity_for(std::size_t desired_size) {
        if (desired_size <= m_capacity) return; // already have enough space

        // growth strategy: ~1.5x + 8
        // this amortizes the cost of repeated push_back operations
        std::size_t new_cap;
        if (m_capacity == 0) {
            // first allocation - exactly what's needed
            new_cap = desired_size;
        } else {
            new_cap = (m_capacity * 3) / 2 + 8; //grow by ~50% plus small constant
        }

        if (new_cap < desired_size) {
            new_cap = desired_size; // ensure we meet minimum requirement
        }

        reserve(new_cap);
    }

};

// non-member operator overloads

// string + string concatenation
inline String operator+(const String& lhs, const String& rhs) {
    String out(lhs); // copy left-hand side
    out.append(rhs.c_str()); // append right hand side
    return out; // return by value (move constructor will be used)
    // when you return a local object by value, the compiler recognizes that the object is about to
    // be destroyed anyway, so it automatically treats it as an rvalue (movable) rather than requiring a copy
}

// string + c-string concatenation
inline String operator+(const String& lhs, const char* rhs) {
    String out(lhs); // copy left hand side
    out.append(rhs); // append C-string
    return out; // return by value (move constructor will also be used)
}