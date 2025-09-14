// ============================================================================
// DEMO CODE - strings_handrolled_demo.cpp
// ============================================================================

#include <iostream>
#include "String.hpp"
#include "string_utils.h"
#include "utils.hpp"    // For hr() and Demo struct

// RULE OF FIVE DEMONSTRATION
static void demo_rule_of_five() {
    hr("Rule of Five sanity");

    String a("hello");          // Constructor from C-string
    String b = a;               // Copy constructor (creates new object)
    String c = std::move(a);    // Move constructor (steals from a, a becomes empty)

    String d;                   // Default constructor (empty string)
    d = b;                      // Copy assignment (d already exists, copies from b)

    String e;                   // Default constructor
    e = std::move(b);           // Move assignment (steals from b, b becomes empty)

    // After these operations:
    // a is empty (moved from)
    // b is empty (moved from)
    // c contains "hello" (moved from a)
    // d contains "hello" (copied from b before b was moved)
    // e contains "hello" (moved from b)

    std::cout << "c: " << c.c_str() << "\n";  // Should print "hello"
    std::cout << "d: " << d.c_str() << "\n";  // Should print "hello"
    std::cout << "e: " << e.c_str() << "\n";  // Should print "hello"
}

// CAPACITY AND GROWTH DEMONSTRATION
static void demo_append_and_capacity() {
    hr("append & capacity");

    String s("ab");       // Start with "ab" (size=2, capacity=2)
    s.reserve(8);         // Pre-allocate space for 8 characters (avoids reallocation)
    s.append("cd");       // Append "cd" -> "abcd" (size=4, capacity=8)
    s.push_back('e');     // Append 'e' -> "abcde" (size=5, capacity=8)

    std::cout << s.c_str() << " (size=" << s.size()
              << ", cap=" << s.capacity() << ")\n";

    s.shrink_to_fit();    // Reduce capacity to exactly match size
    std::cout << "after shrink cap=" << s.capacity() << "\n";  // Should be 5
}

// C-STRING UTILITIES DEMONSTRATION
static void demo_cstr_utils() {
    hr("cstr utils");
    using namespace cstr;  // Bring cstr namespace functions into scope

    const char* src = "World";            // Source string
    char* dup = StringDuplicate(src);     // Create heap copy of "World"

    char buf[32] = {};                    // Stack buffer, zero-initialized
    StringCopy(buf, "Hello ");           // Copy "Hello " into buf
    StringCat(buf, dup);                 // Append "World" -> "Hello World"

    std::cout << buf << " (len=" << StringLength(buf) << ")\n";

    delete[] dup;  // CRITICAL: Free the duplicated string to avoid memory leak
}

// MAIN FUNCTION - sets up CLI demo system
int main(int argc, char** argv) {
    // Array of demo functions with metadata
    std::vector<Demo> demos = {
            {"rule_of_five", "Copy/move/assign correctness", demo_rule_of_five},
            {"append_capacity", "append/push_back/reserve/shrink_to_fit", demo_append_and_capacity},
            {"cstr_utils", "StringCopy/StringCat/StringLength/StringDuplicate", demo_cstr_utils},
    };

    // Use utility CLI runner to select and execute demos
    return run_cli("Hand-rolled String & C-string utils", demos, argc, argv);
}

/*
WHAT THIS CODE TEACHES:

1. **Memory Management Patterns**:
   - RAII: Resources acquired in constructor, released in destructor
   - Copy-and-swap idiom for exception safety
   - Manual memory management with new[]/delete[]

2. **The Rule of Five**:
   - When you manage resources, you need all five special member functions
   - Default versions provided by compiler are often wrong for resource-owning classes
   - Move semantics (C++11) for efficiency

3. **String Implementation Details**:
   - How dynamic strings grow and manage capacity
   - Null termination invariant for C compatibility
   - Growth strategies to amortize allocation costs

4. **C-String Fundamentals**:
   - How strlen, strcpy, strcat work internally
   - Pointer arithmetic and dereferencing
   - Manual memory management pitfalls

5. **Exception Safety**:
   - Strong guarantee: operations either succeed completely or leave object unchanged
   - Basic guarantee: object remains in valid state even if operation fails
   - noexcept specifications for performance and correctness

6. **Performance Considerations**:
   - Why std::string pre-allocates capacity
   - Cost of reallocation vs. wasted memory
   - Move semantics eliminate unnecessary copying

COMMON PITFALLS THIS CODE AVOIDS:

- Null pointer dereference (asserts check for nullptr)
- Memory leaks (RAII ensures cleanup)
- Buffer overflows (capacity tracking)
- Double-delete (move constructors properly transfer ownership)
- Self-assignment bugs (copy-and-swap handles this naturally)
- Exception safety (strong guarantees where possible)

COMPARISON TO std::string:

This implementation demonstrates the core concepts behind std::string but is
simplified for educational purposes. std::string has additional optimizations:
- Small String Optimization (SSO) for short strings
- More sophisticated growth strategies
- Better exception specifications
- Additional member functions and overloads
- Optimized algorithms and platform-specific implementations

The key insight is that std::string is "just" a class like this one, but with
20+ years of optimization and edge-case handling!
*/