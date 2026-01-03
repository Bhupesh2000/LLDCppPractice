// It allows a request to be passed along a chain of handlers. Each handler either processes the request or passes it to the next handler in the chain.

// A request enters at the Head.
// If the Head knows how to solve it, it solves it and stops.
// If not, it calls the Next node.
// If you reach the end of the list and no one solved it, the request is unhandled.

#include <iostream>
#include <memory>
#include <vector>
#include <string>

// 1. The Handler Interface
class ISupportHandler{
protected:
    std::shared_ptr<ISupportHandler> nextHandler;

public:
    ISupportHandler() = default;

    void setNext(std::shared_ptr<ISupportHandler> nextHandler){
        this -> nextHandler = nextHandler;
    }

    virtual void handleRequest(const std::string& issueType) const = 0;

    virtual ~ISupportHandler() = default;
};

// 2. Concrete Handlers (Specialists)
class Level1Support : public ISupportHandler{
public:
    void handleRequest(const std::string& issueType) const override{
        if(issueType == "Basic") std::cout << "Handled at level 1\n";
        else if(nextHandler){
            std::cout << "Can't at level 1, pasing to next level\n";
            nextHandler -> handleRequest(issueType);
        }
        else std::cout << "Unhandled query\n";
    }
};

class Level2Support : public ISupportHandler{
public:
    void handleRequest(const std::string& issueType) const override{
        if(issueType == "Advanced") std::cout << "Handled at level 2\n";
        else if(nextHandler){
            std::cout << "Can't at level 2, pasing to next level\n";
            nextHandler -> handleRequest(issueType);
        }
        else std::cout << "Unhandled query\n";
    }
};

int main() {

    auto l1 = std::make_shared<Level1Support>();
    auto l2 = std::make_shared<Level2Support>();

    l1->setNext(l2); // Building the linked list

    l1->handleRequest("Advanced"); // Starts at L1, gets passed to L2
    return 0;
}