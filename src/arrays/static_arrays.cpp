#include <iostream>
#include <array>
#include <memory>
#include <cassert>
#include "utils.hpp"

// basics of raw static arrays
static void demo_array_basics() {
    hr("Array basics");

    // define a fixed-size array of 5 ints, on the stack
    int a[5] = {1, 2, 3, 4, 5};

    // indexing with [] gives element access
    std::cout << "a[0]=" << a[0] << ", a[4]=" << a[4] << "\n";

    // assign to an element
    a[2] = 42;
    std::cout << "a[2]=" << a[2] << "\n";

    // sizeof(a) = total bytes (5 * 4 = 20)
    // sizeof(a[0]) = element size (4)
    // division gives element count
    std::size_t count = sizeof(a) / sizeof(a[0]);
    std::cout << "count= " << count << "\n";

    // range-for with reference mutates array in place
    for (int &x : a) { x += 1; }

    // range-for by value prints elements
    for (int x : a) std::cout << x << " ";
    std::cout << "\n";
}


static void demo_examples_of_static_arrays() {
    hr("Examples of Static Arrays");

    // compiler infers length
    int a[] = {10, 20, 30, 40};

    // std::begin/end return pointers to first and one-past-last
    auto *first = std::begin(a);
    auto *last = std::end(a);

    std::cout << "first=" << *first << ", last=" << *(last - 1) << "\n";

    int sum = 0;
    // manual pointer iteration
    for (auto *p = first; p != last; ++p)
        sum += *p;

    std::cout << "sum=" << sum << "\n";
}

// helper: show pointer-decay when passing arrays to functions
// need to pass size as a separate parameter, because when array is passed as a parameter all size info is lost
static void inc_all_pointer_decay(int arr[], int n) {
    // inside function: "arr" is actually int*, not an array
    std::cout << "(inside function, arr passed as parameter) sizeof(arr)=" << sizeof(arr) << "\n"; //prints pointer size
    for (int i = 0; i < n; ++i) {
        arr[i] += 1;
    }
}

// arrays lose their array-ness when passed to functions - they automatically convert to pointers,
// losing size information. This explains why memcpy, qsort need size parameters
static void demo_array_as_function_argument() {
    hr("Array as Function Argument");

    int a[3] = {7, 8, 9};

    // in caller scope: sizeof gives full array size (12)
    std::cout << "outside sizeof(a)=" << sizeof(a) << "\n";

    // but inside the callee, "arr" is just a pointer
    inc_all_pointer_decay(a, 3);

    // array modified in-place
    // arrays are passed by reference (sort of)
    // even though the array decays to a pointer, changes inside the function affect the original array
    // because the pointer points to the same memory
    std::cout << a[0] << "," << a[1] << "," << a[2] << "\n";
}

// template helper: take array by reference to preserve size info
// the template doesn't prevent decay by itself, it just makes it work with an array of any size
// and deduces the size automatically, so you don't have to specify N manually
// reference / pointer reminder:
// reference: a nickname for the original object, and when using that nickname you're directly manipulating
// the original
// pointer: like an address card for where you can find the original object. You have to "go to that address"
// (dereference) to manipulate the original
template <std::size_t N>
// meaning of: int (&arr)[N])
// N = array of N integers
// &arr = reference to (prevents decay into pointer)
static void inc_all_by_ref(int (&arr)[N]) {
    // N is deduced automatically
    for (std::size_t i = 0; i < N; ++i) {
        arr[i] += 10;
    }
}

// this example shows how to preserve array size information when passing arrays to functions
static void demo_array_as_reference_argument() {
    hr("Array as Reference Argument");

    int a[4] = {1, 2, 3, 4};
    inc_all_by_ref(a); // adds 10 to each element

    for (int x : a) {
        std::cout << x << " ";
    }
    std::cout << "\n";
}

// using std::array wrapper


int main(int argc, char **argv)
{
    // Each {key, description, function}.
    std::vector<Demo> demos = {
            {"array_basics", "8. Array Basics", demo_array_basics},
            {"examples_of_static_arrays", "9. Examples", demo_examples_of_static_arrays},
            {"array_as_function_argument", "10. decay", demo_array_as_function_argument},
            {"array_as_reference_argument", "11. by-ref keeps size", demo_array_as_reference_argument},
//            {"intro_std_array", "12. std::array", demo_intro_std_array},
//            {"heap_based_arrays", "13. heap arrays", demo_heap_based_arrays},
//            {"multidimensional_arrays", "14. 2D static", demo_multidimensional_arrays},
//            {"multidimensional_arrays_on_heap", "15. 2D heap", demo_multidimensional_arrays_on_heap}
    };

//    demo_array_basics();
//    demo_examples_of_static_arrays();
    demo_array_as_function_argument();
    demo_array_as_reference_argument();

    // run_cli = utility that parses argv, lists/runs demos by name.
    return run_cli("Section 2: Static Arrays", demos, argc, argv);
}




















