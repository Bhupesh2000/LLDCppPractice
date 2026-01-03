// If Object A has a shared_ptr to B, and B has a shared_ptr to A, they will never be deleted (a Memory Leak).
// We use weak_ptr to "observe" an object without owning it. To use it, you must convert it back to a shared_ptr temporarily using .lock().
// code

#include <iostream>
#include <memory>
#include <vector>
using namespace std;

class Employee;

class Department{
public:
    // Department OWNS the employees
    vector<shared_ptr<Employee>> staff;
    ~Department(){cout << "Department deleted" << endl;}
};

class Employee{
public:
    // If this was shared_ptr, we'd have a cycle: Dept -> Emp -> Dept
    // We use weak_ptr because an employee "belongs" to a dept but doesn't "own" it.
    weak_ptr<Department> myDept;
    ~Employee() { std::cout << "Employee deleted.\n"; }

    void work(){
        if(auto deptShared = myDept.lock()){
            std::cout << "Working for a valid department.\n";
        } else {
            std::cout << "My department no longer exists!\n";
        }
    }
};

void demoCycleFix() {
    auto engineering = std::make_shared<Department>();
    auto alice = std::make_shared<Employee>();

    engineering->staff.push_back(alice);
    alice->myDept = engineering; // Point weak_ptr to shared_ptr

    std::cout << "Ref count of Dept: " << engineering.use_count() << "\n"; // Still 1! 
    // Because weak_ptr doesn't increase count.
} // Both deleted safely here.


int main() {

    

    return 0;
}