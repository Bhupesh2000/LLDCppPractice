#ifndef TICKET_H
#define TICKET_H

#include <string>
#include <chrono>
#include <utils/IdGenerator.h>

class Ticket {
private:
    long long id;
    long long vehicleId;
    long long slotId;
    std::chrono::system_clock::time_point entryTime;
    bool active;

public:
    Ticket(long long vehicleId, long long slotId);

    void deactivate();

    long long getId() const;
    long long getVehicleId() const;
    long long getSlotId() const;
    std::chrono::system_clock::time_point getEntryTime() const;
    bool isActive() const;

    std::string toString() const;
};

#endif