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

int main(int argc, char **argv) {
    std::vector<Demo> demos = {
            {"basics", "Basics", demo_basics},
            {"demo_string_length", "Demo String length", demo_string_length},
            {"demo_copy_join", "Demo Copy Join", demo_copy_join}

    };

    demo_copy_join_with_explicit_pointers();

    return run_cli("Strings", demos, argc, argv);
}