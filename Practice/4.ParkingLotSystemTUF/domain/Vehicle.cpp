#include "Vehicle.h"

Vehicle::Vehicle(const std::string& licensePlate, VehicleType vehicleType)
    : id(IdGenerator::generate()),
      licensePlate(licensePlate),
      vehicleType(vehicleType) {}

long long Vehicle::getId() const {
    return id;
}

std::string Vehicle::getLicensePlate() const {
    return licensePlate;
}

VehicleType Vehicle::getVehicleType() const {
    return vehicleType;
}

std::string Vehicle::toString() const {
    return "Vehicle{id=" + std::to_string(id) +
           ", licensePlate=" + licensePlate +
           ", vehicleType=" + std::to_string(static_cast<int>(vehicleType)) +
           "}";
}