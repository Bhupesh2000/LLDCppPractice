#ifndef VEHICLE_H
#define VEHICLE_H

#include <string>
#include <enums/VehicleType.h>
#include <utils/IdGenerator.h>

class Vehicle {
private:
    long long id;
    std::string licensePlate;
    VehicleType vehicleType;

public:
    Vehicle(const std::string& licensePlate, VehicleType vehicleType);

    long long getId() const;
    std::string getLicensePlate() const;
    VehicleType getVehicleType() const;

    std::string toString() const;
};

#endif