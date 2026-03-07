#include "ParkingSlot.h"

ParkingSlot::ParkingSlot(VehicleType slotType, int floorNumber)
    : id(IdGenerator::generate()),
      slotType(slotType),
      occupied(false),
      floorNumber(floorNumber) {}

long long ParkingSlot::getId() const {
    return id;
}

VehicleType ParkingSlot::getSlotType() const {
    return slotType;
}

bool ParkingSlot::isOccupied() const {
    std::lock_guard<std::mutex> lock(mtx);
    return occupied;
}

void ParkingSlot::setOccupied(bool value) {
    std::lock_guard<std::mutex> lock(mtx);
    occupied = value;
}

int ParkingSlot::getFloorNumber() const {
    return floorNumber;
}

std::string ParkingSlot::toString() const {
    std::lock_guard<std::mutex> lock(mtx);
    return "ParkingSlot{id=" + std::to_string(id) +
           ", slotType=" + std::to_string(static_cast<int>(slotType)) +
           ", occupied=" + std::to_string(occupied) +
           ", floorNumber=" + std::to_string(floorNumber) +
           "}";
}