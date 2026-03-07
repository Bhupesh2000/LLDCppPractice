#ifndef ID_GENERATOR_H
#define ID_GENERATOR_H

#include <atomic>
#include <chrono>

class IdGenerator{
private:
    static std::atomic<unsigned long> counter; // static belongs to class not object, Only one copy exists for the entire program, Must be defined once outside the class:

public:
    static long long generate() { // Static - Can be called without creating an object. IdGenerator::generate(); Can only access static members of the class.
        auto now = std::chrono::system_clock::now()
            .time_since_epoch()
            .count();
        return now + (++counter);
    }
};

#endif