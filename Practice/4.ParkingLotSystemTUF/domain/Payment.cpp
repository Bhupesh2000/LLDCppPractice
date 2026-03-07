#include "Payment.h"

Payment::Payment(long long ticketId, double amount, PaymentGateway gateway)
    : id(IdGenerator::generate()),
      ticketId(ticketId),
      amount(amount),
      gateway(gateway),
      status(PaymentStatus::PENDING) {}

// No mutex used because Payment state is typically modified by one request thread.
void Payment::markAsSuccess() {
    status = PaymentStatus::SUCCESS;
}

void Payment::markAsFailed() {
    status = PaymentStatus::FAILED;
}

long long Payment::getId() const {
    return id;
}

long long Payment::getTicketId() const {
    return ticketId;
}

double Payment::getAmount() const {
    return amount;
}

PaymentGateway Payment::getGateway() const {
    return gateway;
}

PaymentStatus Payment::getStatus() const {
    return status;
}

std::string Payment::toString() const {
    return "Payment{id=" + std::to_string(id) +
           ", ticketId=" + std::to_string(ticketId) +
           ", amount=" + std::to_string(amount) +
           ", gateway=" + std::to_string(static_cast<int>(gateway)) +
           ", status=" + std::to_string(static_cast<int>(status)) +
           "}";
}