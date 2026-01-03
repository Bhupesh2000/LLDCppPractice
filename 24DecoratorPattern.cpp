// Imagine you are at a sandwich shop.
// The Base: You start with a Plain Bread ($2).
// The Problem (Inheritance): If you wanted to prepare for every customer, you would need to create a new class for every possible sandwich: BreadWithCheese, BreadWithHam, BreadWithCheeseAndHam, BreadWithCheeseAndHamAndTomato... This is Class Explosion. You'd have 100 classes for a simple menu!
// The Solution (Decorator): You treat every topping as a Wrapper that is also a sandwich.
// You take the Plain Bread.
// You "wrap" it in a Cheese Wrapper. Now, when someone asks "What is the price?", the Cheese Wrapper asks the Bread (2) andaddsitsownprice(+1). Total: $3.
// You "wrap" that whole thing in a Ham Wrapper. When asked for the price, the Ham Wrapper asks the "Cheese-Bread" (3) andaddsitsownprice(+2). Total: $5.
// The "Magic" of the Decorator:
// To the customer (the code using the object), the final result just looks like a Sandwich. They don't know it's actually 3 objects wrapped inside each other. They just call getPrice().
// Why it's useful: You can add Cheese to anything—to Bread, to a Burger, to a Salad—without creating a "CheeseBurger" or "CheeseSalad" class. You just wrap the base object in the Cheese Decorator.

#include <iostream>
#include <memory>
#include <vector>
#include <string>


// The Common Interface (The "Sandwich" Contract) : Both the Bread and the Toppings must look like a "Sandwich" to the outside world.
class ISandwich{
public:
    virtual std::string getDescription() const = 0;
    virtual double getPrice() const = 0;
    virtual ~ISandwich() = default;
};

// The Base Object (The thing being wrapped)
class PlainBread : public ISandwich {
public:
    std::string getDescription() const override { return "Bread"; }
    double getPrice() const override { return 20.0; }
};

// The Decorator Base (The "Wrapper") : The Decorator is-a Sandwich (so it can be passed around) and has-a Sandwich (the thing it's wrapping).
class ToppingDecorator : public ISandwich {
protected:
    std::unique_ptr<ISandwich> tempSandwich; // The "Inner" layer
public:
    ToppingDecorator(std::unique_ptr<ISandwich> s) : tempSandwich(std::move(s)) {}
};

// Concrete Decorators (The actual toppings) : These layers add their own logic and then call the "inner" layer.
class Cheese : public ToppingDecorator {
public:
    Cheese(std::unique_ptr<ISandwich> s) : ToppingDecorator(std::move(s)) {}

    std::string getDescription() const override {
        return tempSandwich->getDescription() + ", Cheese";
    }

    double getPrice() const override {
        return tempSandwich->getPrice() + 10.0; // Inner price + 10
    }
};

class Ham : public ToppingDecorator {
public:
    Ham(std::unique_ptr<ISandwich> s) : ToppingDecorator(std::move(s)) {}

    std::string getDescription() const override {
        return tempSandwich->getDescription() + ", Ham";
    }

    double getPrice() const override {
        return tempSandwich->getPrice() + 15.0; // Inner price + 15
    }
};


int main() {

    // 1. Start with bread
    std::unique_ptr<ISandwich> myOrder = std::make_unique<PlainBread>();

    // 2. Wrap it with Cheese
    myOrder = std::make_unique<Cheese>(std::move(myOrder));

    // 3. Wrap that "Cheese-Bread" with Ham
    myOrder = std::make_unique<Ham>(std::move(myOrder));

    // The Client doesn't know it's 3 objects! It just sees "ISandwich"
    std::cout << "Order: " << myOrder->getDescription() << "\n";
    std::cout << "Total: $" << myOrder->getPrice() << "\n";

    return 0;

    // If tomorrow the customer wants Double Cheese, you don't need a DoubleCheese class. You just wrap the order in the Cheese decorator twice.
    myOrder = std::make_unique<Cheese>(std::move(myOrder));
    myOrder = std::make_unique<Cheese>(std::move(myOrder));
}