// High-level modules should not depend on low-level modules. Both should depend on abstractions (interfaces).
// Abstractions should not depend on details. Details should depend on abstractions.

#include <iostream>
#include <memory>
#include <vector>
#include <string>
using namespace std;

// Bad code
class MySQLDatabase {
public:
    void save(string data) { /* logic */ }
};

class UserProfile {
    MySQLDatabase db; // Hard dependency on a concrete detail!
public:
    void save() { db.save("user_data"); }
};

// If you want to switch to MongoDB, you have to change the UserProfile class. The "Higher" class (User logic) is a slave to the "Lower" class (Database logic).


// Correct code
class IDatabase {
public:
    virtual void save(string data) = 0;
    virtual ~IDatabase() = default;
};

class UserProfile {
    IDatabase& database; // Depends on abstraction
public:
    // The dependency is "injected" from the outside
    UserProfile(IDatabase& db) : database(db) {}
    
    void save() { database.save("user_data"); }
};

int main() {

    

    return 0;
}