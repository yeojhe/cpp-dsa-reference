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

int main(int argc, char **argv) {
    std::vector<Demo> demos = {
            {"basics", "Basics", demo_basics}
    };

    demo_string_length();

    return run_cli("Strings", demos, argc, argv);
}