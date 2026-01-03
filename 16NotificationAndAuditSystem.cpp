#include <iostream>
#include <memory>
#include <vector>
#include <string>

enum class MessageType{Marketing, Transactional};
class Message{
protected :
    const std::string content;
    const std::string receiver;
    const MessageType type;
    const bool isAuditWorthy;
public:
    Message(const std::string& content, const std::string& receiver, MessageType type, const bool isAuditWorthy) : content(content), receiver(receiver), type(type), isAuditWorthy(isAuditWorthy){};
    std::string getContent() const{
        return content;
    }
    std::string getreceiver() const{
        return receiver;
    }
    MessageType getMessageType() const{
        return type;
    }
    bool getIsAuditWorthy() const{
        return isAuditWorthy;
    }
    virtual bool toSendMessage() const = 0;
    virtual ~Message() = default;
};

class IRetryable{
public:
    virtual bool retry() const = 0;
    virtual ~IRetryable() = default;
};

class MarketingMessage : public Message{
    const bool isAvailable;
public:
    MarketingMessage(const std::string& content, const std::string& receiver, MessageType type) : Message(content, receiver, type, false), isAvailable(true){};

    MarketingMessage(const std::string& content, const std::string& receiver, const MessageType type, const bool isAvailable) : Message(content, receiver, type, false), isAvailable(isAvailable){};

    bool toSendMessage() const override{
        return isAvailable;
    }

};
class TransactionMessage : public Message, public IRetryable{
public:
    TransactionMessage(const std::string& content, const std::string& receiver, MessageType type) : Message(content, receiver, type, true){};

    bool retry() const override{
        return true;
    }

    bool toSendMessage() const override{
        return retry();
    }
};

class ILogger{
public:
    ILogger() = default;
    virtual void logMessage(const std::shared_ptr<Message> m) const = 0;
    virtual ~ILogger() = default;
};
class ConsoleLogger : public ILogger{
public:
    void logMessage(const std::shared_ptr<Message> m) const override{
        std::string receiver = m -> getreceiver();
        std::string content = m -> getContent();
        std::cout << "On console : " << content << " sent to " << receiver << std::endl;
    }
};
class FileLogger : public ILogger{
public:
    void logMessage(const std::shared_ptr<Message> m) const override{
        std::string receiver = m -> getreceiver();
        std::string content = m -> getContent();
        std::cout << "On file : " << content << " sent to " << receiver << std::endl;
    }
};
class AuditLogger : public ILogger{
public:
    void logMessage(const std::shared_ptr<Message> m) const override{
        if(m -> getIsAuditWorthy()){
            std::string receiver = m -> getreceiver();
            std::string content = m -> getContent();
            std::cout << "Audit logging : " << content << " sent to " << receiver << std::endl;
        }
    }
};

class INotificationChannel{
public:
    INotificationChannel() = default;
    virtual void sendNotification(const std::shared_ptr<Message> m) const = 0;
    virtual ~INotificationChannel() = default;
};
class EmailNotification : public INotificationChannel{
public:
    void sendNotification(const std::shared_ptr<Message> m) const override{
        std::string receiver = m -> getreceiver();
        std::string content = m -> getContent();
        std::cout << "Via email : " << content << " sent to " << receiver << std::endl;
    }
};
class SMSNotification : public INotificationChannel{
public:
    void sendNotification(const std::shared_ptr<Message> m) const override{
        std::string receiver = m -> getreceiver();
        std::string content = m -> getContent();
        std::cout << "Via sms : " << content << " sent to " << receiver << std::endl;
    }
};

class NotificationSerivce{
    std::vector<std::shared_ptr<INotificationChannel>> notificationChannelsList;
    std::vector<std::shared_ptr<ILogger>> loggersList;
public:
    NotificationSerivce() = default;

    void process(const std::shared_ptr<Message> message){
        if(!message -> toSendMessage()) return;
        for(const auto& channel : notificationChannelsList) channel -> sendNotification(message);
        for(const auto& logger : loggersList) logger -> logMessage(message);
    }

    void addNotificationChannel(std::shared_ptr<INotificationChannel> channel){
        notificationChannelsList.push_back(channel);
    }

    void addLogger(std::shared_ptr<ILogger> logger){
        loggersList.push_back(logger);
    }

    // To Do - add remover functions

    ~NotificationSerivce() = default;
};

int main() {

    std::unique_ptr<NotificationSerivce> service = std::make_unique<NotificationSerivce>();

    std::shared_ptr<INotificationChannel> emailNotification = std::make_shared<EmailNotification>();
    service -> addNotificationChannel(emailNotification);

    std::shared_ptr<ILogger> consoleLogger = std::make_shared<ConsoleLogger>();
    std::shared_ptr<ILogger> fileLogger = std::make_shared<FileLogger>();
    service -> addLogger(consoleLogger);
    service -> addLogger(fileLogger);

    const std::string maketingContent = "hey! this is marketing message";
    const std::string marketingReceiver = "Singla";
    const MessageType marketingType = MessageType::Marketing;

    std::shared_ptr<Message> marketingMessage1 = std::make_shared<MarketingMessage>(maketingContent, marketingReceiver, marketingType, false);
    service ->process(marketingMessage1);

    std::shared_ptr<Message> marketingMessage2 = std::make_shared<MarketingMessage>(maketingContent, marketingReceiver, marketingType);
    service ->process(marketingMessage2);

    std::shared_ptr<INotificationChannel> smsNotification = std::make_shared<SMSNotification>();
    service -> addNotificationChannel(smsNotification);

    const std::string transactionalContent = "hey! this is transactional message";
    const std::string transactionalReceiver = "Bhupesh";
    const MessageType transactionalType = MessageType::Transactional;

    std::shared_ptr<Message> transactionalMessage = std::make_shared<TransactionMessage>(transactionalContent, transactionalReceiver, transactionalType);
    service ->process(transactionalMessage);

    return 0;
}