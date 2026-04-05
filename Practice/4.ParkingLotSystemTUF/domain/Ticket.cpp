#include "Ticket.h"

Ticket::Ticket(long long vehicleId, long long slotId)
    : id(IdGenerator::generate()),
      vehicleId(vehicleId),
      slotId(slotId),
      entryTime(std::chrono::system_clock::now()),
      active(true) {}

void Ticket::deactivate() {
    // Ticket object won't be shared across multiple threads, hence no mutex/lock required
    active = false;
}

long long Ticket::getId() const {
    return id;
}

long long Ticket::getVehicleId() const {
    return vehicleId;
}

long long Ticket::getSlotId() const {
    return slotId;
}

std::chrono::system_clock::time_point Ticket::getEntryTime() const {
    return entryTime;
}

bool Ticket::isActive() const {
    return active;
}

std::string Ticket::toString() const {
    auto epoch = entryTime.time_since_epoch().count();

    return "Ticket{id=" + std::to_string(id) +
           ", vehicleId=" + std::to_string(vehicleId) +
           ", slotId=" + std::to_string(slotId) +
           ", entryTime=" + std::to_string(epoch) +
           ", active=" + std::to_string(active) +
           "}";
}