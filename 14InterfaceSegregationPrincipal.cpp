// Clients should not be forced to depend on methods they do not use.

#include <iostream>
#include <memory>
#include <vector>
#include <string>
using namespace std;

// Bad Code
class ISmartDevice {
public:
    virtual void print() = 0;
    virtual void scan() = 0;
    virtual void fax() = 0;
    virtual ~ISmartDevice() = default;
};

// If you want to create a SimplePrinter class, you are forced to implement scan() and fax(), even if they do nothing or throw an error.

// Correct Code
class IPrinter {
public:
    virtual void print() = 0;
    virtual ~IPrinter() = default;
};

class IScanner {
public:
    virtual void scan() = 0;
    virtual ~IScanner() = default;
};

// A Simple Printer only implements what it needs
class SimplePrinter : public IPrinter {
public:
    void print() override { /* printing logic */ }
};

// An All-In-One Printer implements multiple interfaces
class AllInOneMachine : public IPrinter, public IScanner {
public:
    void print() override { /* ... */ }
    void scan() override { /* ... */ }
};

int main() {

    

    return 0;
}