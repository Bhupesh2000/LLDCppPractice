#include <iostream>
#include <memory>

class Engine {
public:
    void start() { std::cout << "Engine started\n"; }
    ~Engine() { std::cout << "Engine destroyed\n"; } // Destructor
};

class Car {
    std::unique_ptr<Engine> engine; // Car "Owns" the engine
public:
    Car(std::unique_ptr<Engine> e) : engine(std::move(e)) {}
};