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