#ifndef RECEIPT_H
#define RECEIPT_H

#include <string>
#include <chrono>
#include <../utils/IdGenerator.h>
#include <../enums/PaymentStatus.h>

class Receipt {
private:
    long long id;
    long long ticketId;
    std::chrono::system_clock::time_point exitTime;
    double totalFee;
    PaymentStatus paymentStatus;

public:
    Receipt(long long ticketId, double totalFee);

    void markAsPaid();
    void markAsFailed();

    long long getId() const;
    long long getTicketId() const;
    std::chrono::system_clock::time_point getExitTime() const;
    double getTotalFee() const;
    PaymentStatus getPaymentStatus() const;

    std::string toString() const;
};

#endif