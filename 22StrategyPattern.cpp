#include <iostream>
#include <memory>
#include <vector>
#include <string>

// Bad Design
class Checkout {
public:
    void processPayment(double amount, std::string type) {
        if (type == "CreditCard") { /* CC Logic */ }
        else if (type == "PayPal") { /* PayPal Logic */ }
        // Violates OCP!
    }
};

// Good Design

// 1. The Strategy Interface
class IPaymentStrategy {
public:
    virtual void pay(double amount) const = 0;
    virtual ~IPaymentStrategy() = default;
};

// 2. Concrete Strategies
class CreditCardPayment : public IPaymentStrategy {
public:
    void pay(double amount) const override {
        std::cout << "Paying " << amount << " using Credit Card.\n";
    }
};

class BitcoinPayment : public IPaymentStrategy {
public:
    void pay(double amount) const override {
        std::cout << "Paying " << amount << " using Bitcoin.\n";
    }
};

// 3. The Context (The class that uses the strategy)
class ShoppingCart{
    std::unique_ptr<IPaymentStrategy> strategy;

public:
    // We can "Set" or "Inject" the strategy
    void setPaymentMethod(std::unique_ptr<IPaymentStrategy> s) {
        strategy = std::move(s);
    }

    void checkout(double amount) {
        if (!strategy) {
            std::cout << "Please select a payment method!\n";
            return;
        }
        strategy->pay(amount); // Delegating the behavior
    }
};

int main() {

    

    return 0;
}