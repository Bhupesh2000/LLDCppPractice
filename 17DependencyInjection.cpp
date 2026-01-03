// Dependency Injection (DI) is just the practice of passing an object its needs, rather than letting the object create them itself.

#include <iostream>
#include <memory>
#include <vector>
#include <string>

// Bad Design
class MySqlDatabase {
public:
    std::string getData() { return "MySQL Data"; }
};

class ReportGenerator {
    MySqlDatabase* db; // Dependency on a CONCRETE class
public:
    ReportGenerator() {
        // BAD: The object creates its own dependency.
        // It is now stuck with MySqlDatabase forever.
        db = new MySqlDatabase(); 
    }
    
    void generate() {
        std::cout << "Report using: " << db->getData() << std::endl;
    }
};

// Rigid: If you want to use PostgreSql, you have to rewrite ReportGenerator.
// Untestable: You cannot test ReportGenerator without a real MySQL database running.


// Good Design
// 1. Create an Abstraction (The "Supplier")
class IDatabase {
public:
    virtual std::string getData() = 0;
    virtual ~IDatabase() = default;
};

// 2. Concrete implementations
class MySqlDatabase1 : public IDatabase {
    std::string getData() override { return "MySQL Data"; }
};

class MongoDatabase : public IDatabase {
    std::string getData() override { return "Mongo Data"; }
};

// 3. The Class uses the Interface (The "Builder") -> Interface Injection
class ReportGenerator2 {
    std::shared_ptr<IDatabase> db; // Depends on Abstraction
public:
    // GOOD: The dependency is INJECTED from outside.
    // We don't care which DB it is, as long as it follows IDatabase.
    ReportGenerator2(std::shared_ptr<IDatabase> externalDb) : db(externalDb) {}

    void generate() {
        std::cout << "Report using: " << db->getData() << std::endl;
    }
};

// Constructor Injection (Most recommended)
// auto myDb = std::make_shared<MySqlDatabase>();
// ReportGenerator report(myDb); // Passed at birth


// Setter Injection (The "Runtime Switch")
// class ReportGenerator {
//     std::shared_ptr<IDatabase> db;
// public:
//     void setDatabase(std::shared_ptr<IDatabase> newDb) { db = newDb; }
// };

int main() {


    return 0;
}