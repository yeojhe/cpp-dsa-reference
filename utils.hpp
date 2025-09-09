#include <iostream>
#include <vector>
#include <string_view>
#include <functional>

struct Demo {
    std::string name;
    std::string description;
    std::function<void()> fn;
};

inline int run_cli(const char* section, const std::vector<Demo>& demos, int argc, char** argv) {

    // Lambda function - captures surrounding variables by reference [&]
    // This is more advanced C++ - creates anonymous function inline
    auto print_list = [&]{  // [&] = capture all variables by reference
        std::cout << "Section: " << section << "\nAvailable demos:\n";

        // Range-based for loop (C++11) - cleaner than traditional for loops
        // 'const auto&' = compiler deduces type, const reference (no copying)
        for (const auto& d : demos)
            std::cout << "  - " << d.name << "  : " << d.description << "\n";
    };

    // Command line argument parsing
    if (argc <= 1 || (argc >= 2 &&
                      (std::string_view(argv[1]) == "--list" || std::string_view(argv[1]) == "-l"))) {

        print_list();  // Call the lambda function
        std::cout << "\nRun: " << argv[0] << " <demo-name>\n";
        return 0;
    }

    // Find and run the requested demo
    std::string chosen = argv[1];
    for (const auto& d : demos) {
        if (d.name == chosen) {
            d.fn();    // Call the stored function through std::function wrapper
            return 0;
        }
    }

    // Error case - demo not found
    std::cerr << "Unknown demo '" << chosen << "'. Use --list to see options.\n";
    return 1;
}

// Simple utility to print section headers
inline void hr(std::string_view label = {}) {  // Default parameter = empty string_view
    std::cout << "\n";
    if (!label.empty()) std::cout << "---- " << label << " ";
    std::cout << "-------------------------------------------\n";
}

// this class helps you to see when constructors, destructors, and operators are
// called - helps to understand object lifetimes and copy/move semantics
// object lifetimes: period from when an object is created until it's destroyed. In C++ this happens automatically
// for stack objects
// copy semantics: making a duplicate of an object
// move semantics: transferring ownership of resources from one object to another. Moving is faster because it
// avoids expensive copying of large resources like strings or vectors
struct Tracer {
    // member initialization with {} - sets default values. Prevents narrowing conversions - eg accidentally
    // converting a double to an int and losing precision
    int value{0};
    // string to identify this object in output
    std::string tag;

    // DEFAULT CONSTRUCTOR - called when you write: Tracer t;
    Tracer() : value(0), tag("default") {
        std::cout << "Tracer() default constructor\n";
    }

    // EXPLICIT constructor - prevents implicit conversions
    // explicit means you can't accidentally convert int to a Tracer
    // called when you write: Tracer t(42, "my_tracer");
    // std::move(t) by itself doesn't move anything, it's a cast that tells the compiler "I'm done with this
    // object, you can steal its resources". It converts an lvalue to an rvalue reference
    // TODO: lvalue and rvalue
    // in the constructor, tag(std::move(t)) efficiently transfers the string's memory instead of copying all the
    // characters
    // TODO: in the constructor is "value" a default value that will be overwritten by a specified string? if it's
    // being specified by a string passed in as a parameter, is it ok for that input string to be moved? should the
    // caller of the function be aware that the string they pass this constructor is going to be moved?
    explicit Tracer(int v, std::string t = "value") : value(v), tag(std::move(t)) {
        std::cout << "Tracer(int) constructor [" << tag << "] value=" << value << "\n";
    }

    // COPY constructor - called when copying an object
    // Called when: Tracer t2 = t1; or Tracer t2(t1);
    // the main work of the constructor is being done in the constructor signature
    // the part after ":" is called the member initialization list
    // this is where the members are initialized: value(other.value), tag(other.tag + " copy")
    // the constructor body {} runs after initialization
    // the initialization list is more efficient because it constructs members directly, rather than
    // default constructing them and then assigning values
    Tracer(const Tracer& other) : value(other.value), tag(other.tag + " copy") {
        std::cout << "Tracer COPY constructor\n";
    }

    // MOVE constructor - called when moving an object (C++11)
    // called when: Tracer t2 = std::move(t1);
    // 'noexcept' promises this won't throw exceptions (important for containers)
    // containers (like std::vector) need to move objects when they resize. If a move operation might throw an
    // exception, the container can't guarantee it won't lose data during the move. So:
    // - if a move constructor is noexcept -> container will use efficient move operations
    // - if move constructor might throw -> container falls back to slower copy operations
    /*
        std::vector<Tracer> vec;
        vec.push_back(Tracer(1));  // Will use move if noexcept, copy if not
    */
    // moving means transferring of expensive resources (like dynamically allocated memory) from one object
    // to another, instead of copying them. eg:
    // std::string c = std::move(a);
    // transfers pointer to existing memory. 'a' is now empty, 'c' owns the original string's memory
    Tracer(Tracer&& other) noexcept : value(other.value), tag(std::move(other.tag) + " moved") {
        std::cout << "Tracer MOVE constructor\n";
        // leave 'other' in a valid but unspecified state
        other.value = 0;
    }

    // COPY assignment operator - called when assigning to existing object
    // called when: t2 = t1; (where t2 already exists)
    // in the constructor signature there are two objects:
    // - o : the source object (right side of =)
    // - *this : the target object (left side of =) - the object whose method is being called
    // so o.value gets the value from the source object, value = o.value assigns to the current object's value
    // member, and return *this returns reference to the current object
    Tracer& operator=(const Tracer& o) {
        std::cout << "Tracer COPY assignment\n";
        value = o.value;
        tag = o.tag;
        return *this; // return reference to self for chaining: a = b = c;
    }

    // MOVE assignment operator - called when move-assigning
    // called when: t2 = std::move(t1); (where t2 already exists)
    Tracer& operator=(Tracer&& o) noexcept {
        std::cout << "Tracer MOVE assignment\n";
        value = o.value;
        tag = std::move(o.tag); // move the string content
        o.value = 0; // reset source object
        return *this;
    }

    // DESTRUCTOR - called when the object goes out of scope or is deleted
    // the ~ symbol indicates destructor
    // actual destruction happens automatically after the destructor body finishes:
    // 1. destructor body runs (the custom cleanup code)
    // 2. member destructors are run automatically (string destructor cleans up tag)
    // 3. memory is reclaimed
    // for simple classes like Tracer, the automatic cleanup is sufficient. For classes that manage resources
    // (files, memory, network connections), you'd put cleanup code in the destructor body
    ~Tracer() {
        std::cout << "~Tracer destructor [" << tag << "]\n";
    }

};
























