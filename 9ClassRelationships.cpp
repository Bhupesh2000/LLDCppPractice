#include <iostream>
#include <vector>
#include <memory>

// 1. Generalization (Inheritance)
class Engine {
public:
    virtual void start() = 0;
    virtual ~Engine() = default;
};

class V8Engine : public Engine {
public:
    void start() override { std::cout << "V8 Roaring!\n"; }
};

// 2. Composition (Strong Ownership)
// The Car OWNERS the Chassis. If Car dies, Chassis dies.
class Chassis { };

// 3. Aggregation (Weak Ownership)
// The Car HAS a Driver. If Car is crushed, Driver walks away.
class Driver { };

class Car {
private:
    Chassis body;                      // Composition (Value type)
    std::unique_ptr<Engine> engine;    // Composition (Ownership)
    Driver* driver;                    // Aggregation (Pointer to external object)

public:
    Car(std::unique_ptr<Engine> e, Driver* d) 
        : engine(std::move(e)), driver(d) {}
};

// 1. Association (The "Knows" Relationship)
// This is the weakest link. One object uses another. In C++, this is often just a pointer or reference passed as a function argument.
// Example: A Manager uses a Printer. The printer exists independently of the manager.
// 2. Aggregation (The "Has-a" Relationship - Weak Ownership)
// Class A contains Class B, but B can outlive A.
// The Bridge: Think of a std::vector<Employee*>. If the vector (the Department) is deleted, the Employees still exist elsewhere in memory.
// C++ Implementation: Usually a std::shared_ptr or a raw pointer if lifetime is managed externally.
// 3. Composition (The "Part-of" Relationship - Strong Ownership)
// Class A contains Class B, and B cannot exist without A. If A is destroyed, B is destroyed.
// The Bridge: Think of a struct Node inside a LinkedList class. If the list is deleted, the nodes must die.
// C++ Implementation: Usually a member variable by value or a std::unique_ptr.
// 4. Generalization (The "Is-a" Relationship)
// This is standard Inheritance.
// Example: Dog is a Canine.