#ifndef PARKING_SLOT_H
#define PARKING_SLOT_H

#include <string>
#include <mutex>
#include <enums/VehicleType.h>
#include <utils/IdGenerator.h>

class ParkingSlot {
private:
    long long id;
    VehicleType slotType;
    bool occupied;
    int floorNumber;
    mutable std::mutex mtx;

public:
    ParkingSlot(VehicleType slotType, int floorNumber);

    long long getId() const;
    VehicleType getSlotType() const;

    bool isOccupied() const;
    void setOccupied(bool value);

    int getFloorNumber() const;

    std::string toString() const;
};

#endif