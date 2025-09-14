// c-string utils
#pragma once
#include <cstddef> // for std::size_t
#include <cassert> // for assert macro

// re-implementation of standard library functions
// precondition on inputs is asserted - not undefined behaviour guarded for production
namespace cstr { // to prevent collision with standard library
    // manual strlen implementation
    inline std::size_t StringLength(const char* s) {
        // debug-mode check - program aborts if s is null
        assert(s != nullptr);

        const char* p = s; // pointer to walk through the string
        while (*p != '\0') { // while current character is not null character
            ++p; // move to next character
        }
        // when loop ends, p points to the '\0'
        // pointer arithmetic: p - s gives number of characters between them
        return static_cast<std::size_t>(p - s);
    }

    // manual strcpy
    inline char* StringCopy(char* dst, const char* src) {
        assert(dst != nullptr && src != nullptr); // validate both pointers

        // remember original destination for return value
        char* out = dst;

        // copy characters one by one until we copy the null terminator
        while((*dst++ = *src++) != '\0') {
            /* everything is happening in the condition, so the body is empty
             * 1. *src gets the current source character
             * 2. that character is assigned to *dst
             * 3. both pointers are incremented (postfix ++)
             * 4. the assigned character is compared to '\0'
             * 5. loop continues until we copy the '\0' character
             * */
        }

        return out; // return original destination pointer
    }

    // manual strcat
    // appends src string to the end of dst string
    // assumes dst has enough space for the concatenated result
    inline char* StringCat(char* dst, const char* src) {
        assert(dst != nullptr && src != nullptr);

        char* p = dst;

        // first, find the end of the destination string
        while (*p != '\0') { // walk until we find the null terminator
            ++p;
        }
        // now p points to the '\0' at the end of dst

        // append src starting at the terminator position
        while ((*p++ = *src++) != '\0') {
            // same trick as StringCopy - copy until we copy the '\0'
        }

        return dst; // return original destination pointer
    }

    // manual strdup
    // creates a heap-allocated copy of the input string
    // caller is responsible for delete[]ing the returned pointer
    inline char* StringDuplicate(const char* src) {
        assert(src != nullptr);

        std::size_t n = StringLength(src); // get length of source
        char* p = new char[n + 1]; // allocate space for copy + terminator
        StringCopy(p, src); // copy the string
        return p; // return pointer to heap-allocated copy
    }
} // namespace cstr
