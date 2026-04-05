#include "Receipt.h"

Receipt::Receipt(long long ticketId, double totalFee)
    : id(IdGenerator::generate()),
      ticketId(ticketId),
      exitTime(std::chrono::system_clock::now()),
      totalFee(totalFee),
      paymentStatus(PaymentStatus::PENDING) {}

void Receipt::markAsPaid() {
    paymentStatus = PaymentStatus::SUCCESS;
}

void Receipt::markAsFailed() {
    paymentStatus = PaymentStatus::FAILED;
}

long long Receipt::getId() const {
    return id;
}

long long Receipt::getTicketId() const {
    return ticketId;
}

std::chrono::system_clock::time_point Receipt::getExitTime() const {
    return exitTime;
}

double Receipt::getTotalFee() const {
    return totalFee;
}

PaymentStatus Receipt::getPaymentStatus() const {
    return paymentStatus;
}

std::string Receipt::toString() const {
    auto epoch = exitTime.time_since_epoch().count();

    return "Receipt{id=" + std::to_string(id) +
           ", ticketId=" + std::to_string(ticketId) +
           ", exitTime=" + std::to_string(epoch) +
           ", totalFee=" + std::to_string(totalFee) +
           ", paymentStatus=" + std::to_string(static_cast<int>(paymentStatus)) +
           "}";
}