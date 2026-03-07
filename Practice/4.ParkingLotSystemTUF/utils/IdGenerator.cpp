#include <IdGenerator.h>

std::atomic<unsigned long> IdGenerator::counter(0);

// ## Why `IdGenerator.cpp` Is Required (C++11)

// ### 1. Declaration vs Definition

// In the header:

// ```cpp
// static std::atomic<unsigned long> counter;
// ```

// This is only a **declaration** (tells compiler it exists).

// In one `.cpp` file:

// ```cpp
// std::atomic<unsigned long> IdGenerator::counter(0);
// ```

// This is the **definition** (allocates memory + initializes it).

// C++ requires exactly one definition for every static data member (One Definition Rule).

// ---

// ### 2. Why It Cannot Stay Only in Header (C++11)

// Headers are included in multiple `.cpp` files.
// If the definition is in the header, every file gets its own copy → linker error (multiple definitions).

// So the definition must exist in **one single `.cpp` file**.

// ---

// ### 3. When Is It Initialized?

// Static members are initialized:

// * Before `main()` runs
// * During program startup

// You do NOT call it manually.
// It is automatically loaded and initialized when the program starts.

// ---

// ### 4. Build Process (Simple View)

// 1. Each `.cpp` file is compiled separately.
// 2. Linker combines them.
// 3. Linker needs exactly one definition of `IdGenerator::counter`.
// 4. That definition comes from `IdGenerator.cpp`.

// ---

// ### Final Concept

// `.h` → declaration
// `.cpp` → definition
// Static data members must follow this rule in C++11.
