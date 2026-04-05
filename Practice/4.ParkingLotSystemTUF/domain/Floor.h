#ifndef FLOOR_H
#define FLOOR_H

#include <utils/IdGenerator.h>
#include <ParkingSlot.h>
#include <enums/VehicleType.h>

#include <vector>
#include <memory>
#include <string>

class Floor{
    long long id;
    int floorNumber;
    std::vector<std::shared_ptr<ParkingSlot>> slots;
    // Floor owns slots
    // But slots may be referenced elsewhere (e.g., ParkingLotService)
    // Shared ownership is safer
    // If Floor is sole owner → use unique_ptr instead.

public:
    explicit Floor(int floorNumber);
    
    void addSlot(std::shared_ptr<ParkingSlot> slot);
    
    std::vector<std::shared_ptr<ParkingSlot>> getSlots() const;
    
    std::vector<std::shared_ptr<ParkingSlot>>  getAvailableSlots(VehicleType vehicleType) const;

    long long getId() const;
    int getFloorNumber() const;
    int getTotalSlots() const;
    int getAvailableSlotsCount(VehicleType vehicleType) const;

    std::string toString() const;
};

#endif