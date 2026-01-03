// "Objects of a superclass should be replaceable with objects of its subclasses without breaking the application."

#include <iostream>
#include <memory>
#include <vector>
#include <string>
using namespace std;

// Bad way
// In geometry, a Square is a Rectangle. So, logically, we might design:
// class Square : public Rectangle.
class Rectangle {
protected:
    int width, height;
public:
    virtual void setWidth(int w) { width = w; }
    virtual void setHeight(int h) { height = h; }
    int getArea() const { return width * height; }
};

class Square : public Rectangle {
public:
    // To keep it a square, we must override both to change both dimensions
    void setWidth(int w) override { width = height = w; }
    void setHeight(int h) override { width = height = h; }
};

// Imagine a function void process(Rectangle& r).
// The developer of this function assumes that if they call r.setWidth(10) and r.setHeight(5), the area will be 50.
// But if you pass a Square object into this function, the area becomes 25.
// The program is technically valid C++, but logically broken. The Square cannot be substituted for the Rectangle without changing the correctness of the program.

// Correct Way
// Instead of forcing all Birds to fly, we recognize that "Flying" is a capability (behavior), not a requirement for being a bird.
class Bird {
public:
    virtual void eat() { /* common bird eating logic */ }
    virtual ~Bird() = default;
};

class IFlyable {
public:
    virtual void fly() = 0;
    virtual ~IFlyable() = default;
};

// Eagle "Is-a" Bird and "Has-the-capability-of" Flying
class Eagle : public Bird, public IFlyable {
public:
    void fly() override { /* Soaring high */ }
};

// Ostrich "Is-a" Bird but NOT Flyable
class Ostrich : public Bird {
public:
    // No fly() method to implement!
};