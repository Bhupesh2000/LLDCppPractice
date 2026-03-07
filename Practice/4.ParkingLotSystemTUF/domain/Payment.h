#ifndef PAYMENT_H
#define PAYMENT_H

#include <string>
#include <utils/IdGenerator.h>
#include <enums/PaymentGateway.h>
#include <enums/PaymentStatus.h>

class Payment {
private:
    long long id;
    long long ticketId;
    double amount;
    PaymentGateway gateway;
    PaymentStatus status;

public:
    Payment(long long ticketId, double amount, PaymentGateway gateway);

    void markAsSuccess();
    void markAsFailed();

    long long getId() const;
    long long getTicketId() const;
    double getAmount() const;
    PaymentGateway getGateway() const;
    PaymentStatus getStatus() const;

    std::string toString() const;
};

#endif