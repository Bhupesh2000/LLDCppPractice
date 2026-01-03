/*
    ENUM vs ENUM CLASS — ALL IN ONE (WITH LLD-STYLE EXAMPLE)

    This file shows:
    1) Problems with old enum
    2) How enum class fixes them
    3) How enum class is used in a clean LLD-style design
*/

#include <iostream>
#include <cstdint>
using namespace std;

/*====================================================
  1) OLD ENUM (PROBLEMS)
====================================================*/

// ❌ Old-style enum (unscoped, unsafe)
enum Status {
    OPEN,
    CLOSED
};

enum Color {
    RED,
    GREEN
};

void oldEnumProblems() {
    Status s = OPEN;

    // Color c = OPEN;   // ❌ allowed (wrong logically)
    int x = OPEN;     // ❌ implicit conversion to int

    cout << "Old enum allows unsafe behavior\n";
}

/*====================================================
  2) MODERN ENUM CLASS (SOLUTION)
====================================================*/

// ✅ Scoped, strongly typed
enum class NewStatus {
    Open,
    Closed
};

enum class NewColor {
    Red,
    Green
};

void enumClassSafety() {
    NewStatus s = NewStatus::Open;

    // NewColor c = NewStatus::Open;   // ❌ compile-time error
    // int x = NewStatus::Open;        // ❌ compile-time error

    int x = static_cast<int>(NewStatus::Open); // ✅ explicit conversion

    cout << "Enum class is type-safe\n";
}

/*====================================================
  3) ENUM CLASS WITH UNDERLYING TYPE
====================================================*/

// Useful for memory / serialization
enum class MediaType : uint8_t {
    Book,
    Movie,
    Music
};

/*====================================================
  4) LLD-STYLE DESIGN USING ENUM CLASS
====================================================*/

class Media {
public:
    enum class Category {
        Book,
        Movie,
        Music
    };

private:
    Category category;

public:
    explicit Media(Category c) : category(c) {}

    void printType() const {
        switch (category) {
            case Category::Book:
                cout << "Media is a Book\n";
                break;
            case Category::Movie:
                cout << "Media is a Movie\n";
                break;
            case Category::Music:
                cout << "Media is Music\n";
                break;
        }
    }
};

/*====================================================
  5) MAIN
====================================================*/

int main() {
    oldEnumProblems();
    enumClassSafety();

    Media m(Media::Category::Book);
    m.printType();

    return 0;
}

/*
=====================================================
KEY TAKEAWAYS (INTERVIEW-READY)
=====================================================

- enum:
  ❌ Unscoped
  ❌ Implicit int conversion
  ❌ Easy to misuse

- enum class:
  ✅ Scoped (Enum::Value)
  ✅ Strongly typed
  ✅ No implicit conversions
  ✅ Preferred in modern C++

- In LLD:
  Use enum class inside classes to model states/types safely.
*/
