#include <iostream>
#include <memory>
#include <string>

class Project {
    std::string name;
public:
    Project(std::string n) : name(n) { std::cout << "Project " << name << " started.\n"; }
    ~Project() { std::cout << "Project " << name << " deleted from memory.\n"; }
    void show() { std::cout << "Working on: " << name << "\n"; }
};

void demoShared() {
    // make_shared is faster and safer than 'new'
    std::shared_ptr<Project> p1 = std::make_shared<Project>("Apollo 11"); 
    
    {
        std::shared_ptr<Project> p2 = p1; // Reference count is now 2
        std::cout << "Ref count: " << p1.use_count() << "\n";
        p2->show();
    } // p2 goes out of scope, count becomes 1. Memory NOT deleted.

    std::cout << "Ref count after scope: " << p1.use_count() << "\n";
} // p1 goes out of scope, count becomes 0. Memory DELETED.