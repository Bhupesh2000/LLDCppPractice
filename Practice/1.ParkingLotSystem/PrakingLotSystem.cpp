#include <iostream>
#include <memory>
#include <vector>
#include <string>
#include <unordered_map>
#include <chrono>
#include <optional>
#include <algorithm>
#include <mutex>
#include <atomic>

using TimePoint = std::chrono::system_clock::time_point;


enum class VehicleType{Car, Bike, Truck};
class Vehicle{
    const std::string vehicleNumber;
    const VehicleType vehicleType;
    const std::string model;

public:
    Vehicle(const std::string& number,
        VehicleType vehicleType, // Pass enums by value
        const std::string& model) : 
        vehicleNumber(number), 
        vehicleType(vehicleType), 
        model(model){}

    // Getters
    const std::string getVehicleNumber() const{
        return vehicleNumber;
    }

    VehicleType getVehicleType() const{
        return vehicleType;
    }

    const std::string getVehicleModel() const{
        return model;
    }
};


enum class SlotType{Small, Medium, Large};
class ParkingSlot{
    const int slotId;
    const SlotType slotType;
    bool isOccupied;

public:
    ParkingSlot(int slotId, SlotType slotType) : slotId(slotId), slotType(slotType), isOccupied(false){}

    // Getters
    int getSlotId() const{
        return slotId;
    }

    SlotType getSlotType() const{
        return slotType;
    }

    bool isSlotOccupied() const{
        return isOccupied;
    }

    // Setters
    void occupySlot(){
        isOccupied = true;
    }

    void vacateSlot(){
        isOccupied = false;
    }

};

class Ticket{
    // inline static int nextTicketId = 1; // property of class rather than object, but not doing this because not thread-safe
    const int ticketId;
    const std::string vehicleNumber;
    const int slotId;
    const SlotType slotType;
    const TimePoint entryTime;
    // const TimePoint exitTime{}; -> sentinal
    std::optional<TimePoint> exitTime; // Better option than sentinal(preffered in Modern cpp)

public:
    Ticket(int ticketId, const std::string& vehicleNumber, int slotId, SlotType slotType) :
    // nextTicketId(nextTicketId ++) -> wrong because static cannot be initialised in constructor
    ticketId(ticketId),
    vehicleNumber(vehicleNumber),
    slotId(slotId),
    slotType(slotType),
    entryTime(std::chrono::system_clock::now()){}

    // Getters
    int getTicketId() const {
        return ticketId;
    }

    const std::string& getVehicleNumber() const {
        return vehicleNumber;
    }

    int getSlotId() const {
        return slotId;
    }

    SlotType getSlotType() const {
        return slotType;
    }

    TimePoint getEntryTime() const {
        return entryTime;
    }

    std::optional<TimePoint> getExitTime() const{
        return exitTime;
    }

    bool isClosed() const{
        // return exitTime != TimePoint{}; -> if used sentinal
        return exitTime.has_value(); // works with std::optinal<T>
    }

    bool closeTicket(){
        if(!isClosed()){
            exitTime = std::chrono::system_clock::now();
            return true;
        }
        std::cout << "Ticket already closed\n";
        return false;
    }
};

class ISlotSelectionStrategy{
protected:
    bool isSlotCompatible(const SlotType slotype, const VehicleType vehicleType) const{ // const is required because isSlotCompatible will be used inside selectSlot which is a const function and a const function can only call const functions
        if(vehicleType == VehicleType::Bike) return true;
        else if(vehicleType == VehicleType::Car) return slotype != SlotType::Small;
        else return slotype == SlotType::Large;
    }
public:
    ISlotSelectionStrategy() = default;
    virtual ~ISlotSelectionStrategy() = default;
    virtual int selectSlot(const std::unordered_map<int, ParkingSlot*>& slotsMap, VehicleType vehicleType) const = 0;
};


class FirstAvailableStrategy : public ISlotSelectionStrategy{
public:
    int selectSlot(const std::unordered_map<int, ParkingSlot*>& slotsMap, VehicleType vehicleType) const override{
        for(const auto &[slotId, slotDetails] : slotsMap){
            if(!slotDetails->isSlotOccupied() 
                && isSlotCompatible(slotDetails -> getSlotType(), vehicleType)){
                    return slotId;
            }
        }
        return -1;
    }
};

class SmallestFitStrategy : public ISlotSelectionStrategy{
public:
    int selectSlot(const std::unordered_map<int, ParkingSlot*>& slotsMap, VehicleType vehicleType) const override{
        int slot1 = -1, slot2 = -1, slot3 = -1;
        for(const auto &[slotId, slotDetails] : slotsMap){
            if(!slotDetails->isSlotOccupied() 
                && isSlotCompatible(slotDetails -> getSlotType(), vehicleType)){
                    if(slotDetails -> getSlotType() == SlotType::Small) slot1 = slotId;
                    else if(slotDetails -> getSlotType() == SlotType::Medium) slot2 = slotId;
                    else slot3 = slotId;
            }
        }
        if(slot1 != -1) return slot1;
        else if(slot2 != -1) return slot2;
        else return slot3;
    }
};

// Don't make slotmanager singleton as it will over complicate the design and is not ideal. The main orchasterator is ParkingLotSystem. It can be made as a singleton
class SlotManager{
    std::unordered_map<int, std::unique_ptr<ParkingSlot>> slotsMap;
    std::unique_ptr<ISlotSelectionStrategy> slotSelectionStrategy;
    mutable std::mutex mtx;

public:
    SlotManager() : slotSelectionStrategy(std::make_unique<FirstAvailableStrategy>()){}
    explicit SlotManager(std::unique_ptr<ISlotSelectionStrategy> strategy) : slotSelectionStrategy(std::move(strategy)){}

    void addParkingSlot(SlotType slotType){
        std::lock_guard<std::mutex> guard(mtx);
        int slotId = slotsMap.size() + 1;
        slotsMap[slotId] = std::make_unique<ParkingSlot>(slotId, slotType);
    }
    
    // void removeParkingSlot(int slotId){} -> remove this as it will overcomplicate

    void updateStartegy(std::unique_ptr<ISlotSelectionStrategy> strategy){
        slotSelectionStrategy = std::move(strategy);
    }

    // {SlotId and SlotType} required for parkingLotSystem class. Also could have returned struct SlotView{int slotId, SlotType slot}
    std::optional<std::pair<int, SlotType>> allocateSlot(VehicleType vehicleType){
        std::lock_guard<std::mutex> guard(mtx);
        // Very important as selectSlot take Parking* as argument and not std::unique_ptr<ParkingSlot>
        std::unordered_map<int, ParkingSlot*> slotViewMap;
        for(auto &[slotId, slotDetails] : slotsMap){
            slotViewMap[slotId] = slotDetails.get();
            // slotDetails is: std::unique_ptr<ParkingSlot>
            // get() returns: ParkingSlot*
            // “Give me the raw pointer to the object you own, without transferring ownership.”
        }
        int selectedSlotId = slotSelectionStrategy -> selectSlot(slotViewMap, vehicleType);
        if(selectedSlotId != -1){
            slotsMap[selectedSlotId] -> occupySlot();
            SlotType slotType = slotsMap[selectedSlotId] -> getSlotType();
            return std::make_pair(selectedSlotId, slotType);
        }
        return std::nullopt; // required as it will lead to undefined value if not returned
    }

    bool releaseSlot(int slotId){
        std::lock_guard<std::mutex> guard(mtx);
        if(slotsMap.count(slotId) != 0){
            slotsMap[slotId] -> vacateSlot(); // a common trap of not checking if slotId exist or not
            return true;
        }
        return false;
    }
};

// a stub for now
double getParkingPrice(int minutes){
    return 100.0;
}

class ParkingLotSystem{
    std::unique_ptr<SlotManager> slotManager;
    std::unordered_map<int, std::unique_ptr<Ticket>> activeTicketsMap;
    // inline static int nextTicketId = 1 -> this won't be thread-safe
    std::atomic<int> nextTicketId{1}; //std::atomic<int> provides a thread-safe integer where reads and writes are performed atomically, without using explicit locks.
    mutable std::mutex mtx;

    ParkingLotSystem() : slotManager(std::make_unique<SlotManager>()){}

public:
    ParkingLotSystem(const ParkingLotSystem&) = delete;
    ParkingLotSystem& operator=(const ParkingLotSystem&) = delete;

    static ParkingLotSystem& getInstance(){
        static ParkingLotSystem instance;
        return instance;
    }

    void addParkingSlot(SlotType slotType){
        std::lock_guard<std::mutex> guard(mtx);
        slotManager -> addParkingSlot(slotType);
    }

    void setSlotSelectionStrategy(std::unique_ptr<ISlotSelectionStrategy> strategy){
        slotManager -> updateStartegy(std::move(strategy));
    }

    int parkVehicle(const std::string& vehicleNumber, VehicleType vehicleType){
         // Step 1: allocate slot (SlotManager handles its own locking)
        auto allocated = slotManager->allocateSlot(vehicleType);
        if (!allocated.has_value()) {
            return -1;
        }

        // Step 2: create ticket under ParkingLotSystem lock
        std::lock_guard<std::mutex> guard(mtx);

        int ticketId = nextTicketId++;

        auto ticket = std::make_unique<Ticket>(
            ticketId,
            vehicleNumber,
            allocated->first,   // slotId
            allocated->second   // slotType
        );

        activeTicketsMap[ticketId] = std::move(ticket);
        return ticketId;
    }

    double unparkVehicle(int ticketId){

        std::lock_guard<std::mutex> guard(mtx); // required because If a data structure is protected by a mutex in one method, it must be protected in all methods that read/write it.
        // also Two threads could call unparkVehicle(ticketId) at the same time; Both pass count(ticketId) != 0; One moves the ticket, the other accesses a moved-from entry

        if(activeTicketsMap.count(ticketId) != 0){
            std::unique_ptr <Ticket> closingTicket = std::move(activeTicketsMap[ticketId]);
            activeTicketsMap.erase(ticketId);

            // Slot can be release immediately and biling is independent and hence can be done afterwards
            int slotID = closingTicket -> getSlotId();
            slotManager -> releaseSlot(slotID);

            closingTicket -> closeTicket();

            TimePoint entryTime = closingTicket -> getEntryTime();
            std::optional<TimePoint> exitTime = closingTicket -> getExitTime();

            if(exitTime.has_value()){
                auto mins = std::chrono::duration_cast<std::chrono::minutes>(exitTime.value() - entryTime).count();
                double price = getParkingPrice(mins);
                return price;
            }
            else return 0.0;
        }
        else return 0.0;
    }
};

int main() {

    

    return 0;
}