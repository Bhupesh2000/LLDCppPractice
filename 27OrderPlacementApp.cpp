#include <iostream>
#include <memory>
#include <vector>
#include <string>
#include <algorithm>

class Order;

class IOrderState{
public:
    virtual void processOrderState(Order* ord) = 0;
    virtual std::string getCurrentState() const = 0;
    virtual ~IOrderState() = default;
};

class OrderValidationState : public IOrderState{
public:
    std::string getCurrentState() const override{ return "Validating"; }
    void processOrderState(Order* ord) override; // Transition logic defined below
};

class OrderPlacedState : public IOrderState{
public:
    std::string getCurrentState() const override{ return "PLACED"; }
    void processOrderState(Order* ord) override; // Transition logic defined below
};

class OrderPreparingState : public IOrderState{
public:
    std::string getCurrentState() const override{ return "Preparing"; }
    void processOrderState(Order* ord) override; // Transition logic defined below
};

class OrderOfdState : public IOrderState{
public:
    std::string getCurrentState() const override{ return "Out for Delivery"; }
    void processOrderState(Order* ord) override; // Transition logic defined below
};

class OrderDeliveredState : public IOrderState{
public:
    std::string getCurrentState() const override{ return "Delivered"; }
    void processOrderState(Order* ord) override; // Transition logic defined below
};


class ISubscriber{
    const std::string name;
    std::string emailId;
public:
    ISubscriber(const std::string name, std::string emailId) : name(name), emailId(emailId){}

    const std::string getName() const {return name;}
    std::string getEmailId() const {return emailId;}

    virtual void update(const int OrderId, const std::string OrderState) const = 0;

    virtual ~ISubscriber() = default;
};

class Customer : public ISubscriber{
    const int custId;
public:
    Customer(const int custId, const std::string name, std::string emailId) : ISubscriber(name, emailId), custId(custId){}
    const int getCustomerId() const {return custId;}

    void update(const int OrderId, const std::string OrderState) const override{
        std::cout << "Customer Notification : Order " << OrderId << " moved to " << OrderState << std::endl;
    }
};

class RestaurantManager : public ISubscriber{
    const int managerId;
public:
    RestaurantManager(const int managerId, const std::string name, std::string emailId) : ISubscriber(name, emailId), managerId(managerId){}
    const int getManagerId() const {return managerId;}

    void update(const int OrderId, const std::string OrderState) const override{
        std::cout << "Manager Notification : Order " << OrderId << " moved to " << OrderState << std::endl;
    }
};

class NotificationService{
private:
    std::vector<std::weak_ptr<ISubscriber>> subscribersList;
public:
    void addSubscriber(std::weak_ptr<ISubscriber> sub){subscribersList.push_back(sub);}

    // very tricky
    void removeSubscriber(std::shared_ptr<ISubscriber> sub) {
        subscribersList.erase(
            std::remove_if(subscribersList.begin(), subscribersList.end(),
                [&sub](const std::weak_ptr<ISubscriber>& wp) {
                    // If the weak_ptr is expired OR it points to our 'sub'
                    return wp.expired() || wp.lock() == sub;
                }),
            subscribersList.end());
    }

    void notifySubscribers(const int OrderId, const std::string& nextState);
};

class IPricingStrategy{
protected:
    int price;
public:
    IPricingStrategy(const int initialPrice):price(initialPrice){}
    void updatePrice(const int newPrice) {this ->price = newPrice;}
    virtual int getPrice() const = 0;
    ~IPricingStrategy() = default;
};

class NormalPricing : public IPricingStrategy{
public:
    NormalPricing(const int price) : IPricingStrategy(price){}
    int getPrice() const override{return price;}
};
class SurgePricing : public IPricingStrategy{
public:
    SurgePricing(const int price) : IPricingStrategy(price){}
    int getPrice() const override{return price * 1.5;}
};

class IValidationHandler{
protected:
    std::shared_ptr<IValidationHandler> nextValidator;
public:
    void setNextValidator(std::shared_ptr<IValidationHandler> validator){
        this -> nextValidator = validator;
    }

    virtual bool validate() const = 0;
    
    virtual ~IValidationHandler() = default;
};

class StockCheckHandler : public IValidationHandler{
public:
    bool validate() const override{
        // Logic for stock check
        // return false if failed
        // if passed, then check for next Validator
        if(nextValidator){
            std::cout << "Stock availability checked\n";
            return nextValidator -> validate();
        }
        // since no next validator return true
        else return true;
    }
};

class PaymentCheckHandler : public IValidationHandler{
public:
    bool validate() const override{
        // Logic for Payment method check
        // return false if failed
        // if passed, then check for next Validator
        if(nextValidator){
            std::cout << "Payment method checked\n";
            return nextValidator -> validate();
        }
        // since no next validator return true
        else return true;
    }
};

class Order{
    const int OrderId;
    const std::string item;
    std::unique_ptr<IOrderState> currentState;
    std::unique_ptr<IPricingStrategy> pricingStartegy;
    std::unique_ptr<NotificationService> notificationService;
    std::weak_ptr<ISubscriber> customer;
    std::weak_ptr<ISubscriber> manager;
public:
    Order(const int OrderId, std::string& item, std::shared_ptr<Customer> customer, std::shared_ptr<RestaurantManager> manager) : OrderId(OrderId), item(item), pricingStartegy(std::make_unique<NormalPricing>(5)), currentState(std::make_unique<OrderValidationState>()), notificationService(std::make_unique<NotificationService>()), customer(customer), manager(manager){}
    
    Order(const int OrderId, std::string& item, std::unique_ptr<IPricingStrategy> pricingStartegy, std::shared_ptr<Customer> customer, std::shared_ptr<RestaurantManager> manager) : OrderId(OrderId), item(item), pricingStartegy(std::move(pricingStartegy)),currentState(std::make_unique<OrderValidationState>()), notificationService(std::make_unique<NotificationService>()), customer(customer), manager(manager){}

    void setPricingStrategy(std::unique_ptr<IPricingStrategy> strat){
        this -> pricingStartegy = std::move(strat);
    }

    bool validateOrder() const{
        auto stockCheckHandler = std::make_shared<StockCheckHandler>();
        auto paymentCheckHandler = std::make_shared<PaymentCheckHandler>();
        stockCheckHandler -> setNextValidator(paymentCheckHandler);

        if(stockCheckHandler -> validate()) return true;
        else return false;
    }

    int getPriceForOrder() const{
        return pricingStartegy -> getPrice();
    }

    void setState(std::unique_ptr<IOrderState> nextState){
        currentState = std::move(nextState);
    }

    void processOrderState(){
        std::string currOrderState = currentState -> getCurrentState();
        currentState -> processOrderState(this);
        std::string newOrderState = currentState -> getCurrentState();
        notificationService -> notifySubscribers(OrderId, newOrderState);
    }

    void processOrder(){
        notificationService -> addSubscriber(customer);
        notificationService -> addSubscriber(manager);
        std::cout << currentState -> getCurrentState() << std::endl;
        if(validateOrder()){
            const int orderPrice = getPriceForOrder();
            std::cout << "Order is of " << orderPrice << std::endl;
            processOrderState(); // Validating -> Placed
            std::cout << currentState -> getCurrentState() << std::endl;
            processOrderState(); // Placed -> Preparing
            std::cout << currentState -> getCurrentState() << std::endl;
            processOrderState(); // Preparing -> Out for Delivery
            std::cout << currentState -> getCurrentState() << std::endl;
            processOrderState(); // Out for Delivery -> Delivered
            std::cout << currentState -> getCurrentState() << std::endl;
        }
        else std::cout << "Invalid Order \n";
    }

};

// To implement
void NotificationService::notifySubscribers(const int OrderId, const std::string& nextState){
    for(auto& subscriber : subscribersList){
        if(auto subsLock = subscriber.lock()){
            subsLock -> update(OrderId, nextState);
        }
    }
}

void OrderValidationState::processOrderState(Order* ord){
    std::cout << "Validating Order\n";
    ord -> setState(std::make_unique<OrderPlacedState>());
}
void OrderPlacedState::processOrderState(Order* ord){
    std::cout << "Order Placed\n";
    ord -> setState(std::make_unique<OrderPreparingState>());
}
void OrderPreparingState::processOrderState(Order* ord){
    std::cout << "Preparing Order\n";
    ord -> setState(std::make_unique<OrderOfdState>());
}
void OrderOfdState::processOrderState(Order* ord){
    std::cout << "Order out for delivery\n";
    ord -> setState(std::make_unique<OrderDeliveredState>());
}
void OrderDeliveredState::processOrderState(Order* ord){
    std::cout << "Order Delivered\n";
}

int main() {

    

    return 0;
}