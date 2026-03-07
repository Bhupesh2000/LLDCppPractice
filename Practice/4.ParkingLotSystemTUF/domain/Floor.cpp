#include "Floor.h"

Floor::Floor(int floorNumber) : id(IdGenerator::generate()), floorNumber(floorNumber) {}

void Floor::addSlot(std::shared_ptr<ParkingSlot> slot) {
    slots.push_back(slot);
}

std::vector<std::shared_ptr<ParkingSlot>> Floor::getSlots() const {
    return slots;   // returns copy -> defensive copy
}

std::vector<std::shared_ptr<ParkingSlot>> Floor::getAvailableSlots(VehicleType vehicleType) const {

    std::vector<std::shared_ptr<ParkingSlot>> availableSlots;

    for (const auto& slot : slots) {
        if (slot->getSlotType() == vehicleType && !slot->isOccupied()) {
            availableSlots.push_back(slot);
        }
    }

    return availableSlots;
}
// Why No Mutex in addSlot()?
// Floors and slots are created during system initialization.
// Structural data (floors + slots) is not modified at runtime.
// No concurrent writes → no need for locking.
// Only runtime state (slot occupancy, ticket generation) needs synchronization.

long long Floor::getId() const {
    return id;
}

int Floor::getFloorNumber() const {
    return floorNumber;
}

int Floor::getTotalSlots() const {
    return static_cast<int>(slots.size());
}

int Floor::getAvailableSlotsCount(VehicleType vehicleType) const {
    return static_cast<int>(getAvailableSlots(vehicleType).size());
}

std::string Floor::toString() const {
    return "Floor{id=" + std::to_string(id) +
           ", floorNumber=" + std::to_string(floorNumber) +
           ", totalSlots=" + std::to_string(slots.size()) +
           "}";
}


