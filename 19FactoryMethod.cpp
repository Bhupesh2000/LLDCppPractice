// Since Singletons are rigid and hardcoding new EmailService() is bad, how do we create objects? We use a Factory.

#include <iostream>
#include <memory>
#include <vector>
#include <string>

class INotificationChannel{};
class SMSNotification:public INotificationChannel{};
class EmailNotification:public INotificationChannel{};

class NotificationFactory {
public:
    // This is the "Factory Method"
    static std::unique_ptr<INotificationChannel> createChannel(const std::string& type) {
        if (type == "SMS") {
            return std::make_unique<SMSNotification>();
        } else if (type == "Email") {
            return std::make_unique<EmailNotification>();
        }
        return nullptr;
    }
};

// Usage:
auto myChannel = NotificationFactory::createChannel("Email");

// This is better because main() doesn't need to know about SMSNotification or EmailNotification. It only knows about the Factory.
// But this if-else block,violates OCP (Open/Closed Principle)! If we add "WhatsApp", we have to modify the Factory.

// Instead of one big "Menu" (if-else), we create a "Factory Interface".
// The Abstract Creator
class ChannelFactory{
public:
    virtual std::unique_ptr<INotificationChannel> create() = 0;
    virtual ~ChannelFactory() = default;
};

// Specific Factories
class EmailChannel : public ChannelFactory{
public:
    std::unique_ptr<INotificationChannel> create() override{
        return std::make_unique<EmailNotification>();
    }
};

class SMSChannel : public ChannelFactory{
public:
    std::unique_ptr<INotificationChannel> create() override{
        return std::make_unique<SMSNotification>();
    }
};


int main() {

    

    return 0;
}