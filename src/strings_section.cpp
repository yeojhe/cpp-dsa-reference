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

int main(int argc, char **argv) {
    std::vector<Demo> demos = {
            {"basics", "Basics", demo_basics}
    };

    return run_cli("Strings", demos, argc, argv);
}