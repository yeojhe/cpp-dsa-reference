#include <iostream>
#include <cstring>
#include <memory>
#include <string>
#include <vector>
#include "utils.hpp"

// static function - only visible within this translation unit (file scope)
static void demo_basics() {
    hr("C-string basics");

    // create a C-string (character array) on the stack
    // this is a 6 element array
    // the '\0' is the null terminator - ESSENTIAL for C-strings
    char s[6] = {'H', 'e', 'l', 'l', 'o', '\0'};

    // print the string and its length
    // std::strlen counts characters until it hits '\0' (doesn't include the null terminator)
    std::cout << s << " (len=" << std::strlen(s) << ")\n";
}

static void demo_string_length() {
    hr("String length");

    // pointer to a string literal stored in read-only memory
    // the embedded '\0' terminates the string early - "withjunk" won't be counted
    const char *msg = "Hello\0withjunk";

    // what the pointer actually points to:
    // Memory layout (read-only section):
    // Address:   0x1000  0x1001  0x1002  0x1003  0x1004  0x1005  0x1006  0x1007  ...
    // Content:   'H'     'e'     'l'     'l'     'o'     '\0'    'w'     'i'     ...
    //            ^
    //            msg = 0x1000


    // strlen stops at the first '\0', so it only counts "Hello"
    std::cout << "strlen(msg)=" << std::strlen(msg) << "\n";
    // this will print "withjunk" - as the pointer now stars after the \0
    std::cout << msg + 6 << "\n";
}

static void demo_copy_join() {
    hr("Copy & Join");

    // create a 32-byte buffer on the stack, intialized to all zeros
    // {} initialization sets all elements to 0, ensuring null termination
    char dst[32] = {};

    // copy "Hello" into dst buffer (including null terminator)
    // SAFE because dst is large enough (32 bytes > 6 bytes needed)
    // signature of
    std::strcpy(dst, "Hello");

    // concatenate ", world" to the end of dst
    // strcat finds the end of dst and appends the new string
    std::strcat(dst, ", world");

    // print the result: "Hello, world"
    std::cout << dst << "\n";

    // modern C++ way using std::string class
    std::string a = "Hello";
    std::string b = a + ", world"; // overloaded + operator
    std::cout << b << "\n"; // same output but safer and more convenient
}

static void demo_copy_join_with_explicit_pointers() {
    hr("Copy & Join - explicit pointers");

    char dst[32] = {};

    // manual strcpy using pointers - what strcpy does internally

    // src is a pointer to the rvalue "Hello", so it starts off pointing to the first character: 'H'
    // dest_ptr points to the first positing in the array dst to being with
    // then in the while loop until reaching the null terminator:
    // - *destr_ptr represents the thing that is being pointed to by destr_ptr, which is the first
    // position of the dst array
    // - that first position of the dst array is then set to the thing that is being pointed to by src,
    // represented by *src
    // this is dereferencing
    const char* src = "Hello";
    // point to the start of the destination
    char* dest_ptr = dst;
    // while not at the null terminator:
    while (*src != '\0') {
        // copy character through pointers
        *dest_ptr = *src;
        dest_ptr++;
        src++;
    }
    *dest_ptr = '\0';

    // manual strcat using pointeres (what strcat does internally)
    const char* append_src = ", world";

    // find end of existing string
    while (*dest_ptr != '\0') {
        dest_ptr++;
    }

    // append new string
    while (*append_src != '\0') {
        *dest_ptr = *append_src;
        dest_ptr++;
        append_src++;
    }
    // add final null terminator
    *dest_ptr = '\0';

    std::cout << dst << "\n";
}

static void demo_copy_through_allocation() {
    hr("Copy through Allocation");

    // pointer to string literal in read-only memory
    const char *src = "allocate me";

    // get the length of source string (11 characters)
    std::size_t n = std::strlen(src);

    // allocate memory on the heap for n+1 characters (extra 1 for null terminator)
    // new[] allocates an array of char - returns pointer to first element
    char *heap = new char[n + 1];

    // copy n+1 bytes from src to heap (includes the null terminator)
    // memcpy copies raw bytes - doesn't stop at null terminators like strcpy
    std::memcpy(heap, src, n + 1);

    // print the copied string
    std::cout << "heap=" << heap << "\n";

    // CRITICAL: free the dynamically allocated memory
    // must use delete[] for arrays allocated with new[]
    // failure to do this causes memory leaks
    delete[] heap;

    // to avoid a dangling pointer:
    heap = nullptr;
}

static void demo_copy_through_allocation_modern() {
    hr("Copy through allocation - modern");

    const char *src = "allocate me";
    std::size_t n = std::strlen(src);

    // smart pointer automatically allocates memory
    // BUT if this line throws an exception AFTER new but BEFORE unique_ptr construction,
    // a memory leak is possible
    std::unique_ptr<char[]> heap(new char[n + 1]);

    std::memcpy(heap.get(), src, n + 1);
    std::cout << "heap=" << heap.get() << "\n";

    // no manual delete needed and no dangling pointer possible
}

static void demo_copy_through_allocation_more_modern() {
    hr("Copy through allocation - very modern");

    const char *src = "allocate me";
    std::size_t n = std::strlen(src);

    // make unique is the preferred way (C++ 14)
    // safe compared to above because there is no gap between allocation and smart pointer construction
    // NOTE: unique_ptr is the type, make_unique creates a unique_ptr
    auto heap = std::make_unique<char[]>(n + 1);

    std::memcpy(heap.get(), src, n + 1);
    std::cout << "heap=" << heap.get() << "\n";
}

static void demo_string_class() {
    hr("std::string");

    // create a std::string object - automatically manages memory
    std::string s = "abc";

    // add single character to the end of a string
    s.push_back('d');

    // concatenate string literal using += operator
    s += "\"ef\"";

    // print string and its size (number of characters, excluding null terminator)
    std::cout << s << " size=" << s.size() << "\n";

    // extract substring starting at index 1, length 3
    // substr(1,3) gets characters at positions 1, 2, 3
    std::cout << "substr(1, 3)=" << s.substr(1, 3) << "\n";

    // find() returns position of substring, or std::string::npos if not found
    std::cout << "find(\"cd\")=" << s.find("cd") << "\n";

    // get C-string representation (const char*) of std::string
    // this returns a pointer to an internal buffer - don't delete it!
    const char *cview = s.c_str();
    std::cout << "c_str()->" << cview << "\n";


}

int main(int argc, char **argv) {
    std::vector<Demo> demos = {
            {"basics", "Basics", demo_basics},
            {"demo_string_length", "Demo String length", demo_string_length},
            {"demo_copy_join", "Demo Copy Join", demo_copy_join},
            {"demo_copy_join_with_explicit_pointers", "Demo Copy Join with explicit pointers", demo_copy_join_with_explicit_pointers},
            {"demo_copy_through_allocation", "Demo Copy through allocation", demo_copy_through_allocation},
            {"demo_string_class", "Demo string class", demo_string_class}

    };

//    demo_string_class();

    return run_cli("Strings", demos, argc, argv);
}