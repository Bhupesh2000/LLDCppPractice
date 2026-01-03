// explicit is a C++ keyword used with constructors (and conversion operators) to prevent unintended implicit type conversions.

// -> Without Explicit
#include <iostream>
using namespace std;

class Library {
public:
    Library(int books) {
        cout << "Library created with " << books << " books\n";
    }
};

void process(Library lib) {}

int main() {
    process(5);   // ❌ implicit conversion: int → Library
}


// -> Fix Using Explicit
class Library {
public:
    explicit Library(int books) {
        cout << "Library created with " << books << " books\n";
    }
};

void process(Library lib) {}

int main() {
    // process(5);     // ❌ compile-time error
    process(Library(5));  // ✅ explicit and clear
}
