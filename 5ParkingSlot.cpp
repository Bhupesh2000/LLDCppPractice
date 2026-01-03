#include <iostream>
#include <memory>
#include <string> // Added explicit header for std::string
#include <vector>
using namespace std;

class parkingSlot;

class Vehicle{
    const std::string licensePlate; // Renamed for clarity
    bool isParked;
    weak_ptr<parkingSlot> vehicleSlot;
public :
    // Vehicle(string number) : number(number){
    //     isParked = false;
    // };
    // CHANGE: Added 'explicit' to prevent accidental conversions from string to Vehicle
    // CHANGE: Moved isParked to Initializer List (More efficient than body assignment)
    explicit Vehicle(std::string plate) : licensePlate(std::move(plate)), isParked(false) {}

    // CHANGE: Using const for methods that don't modify the object
    std::string getPlate() const { return licensePlate; }

    void parkVehicle(shared_ptr<parkingSlot> Slot){
        isParked = true;
        this -> vehicleSlot = Slot;
    }

    void unParkVehicle(){
        isParked = false;
        vehicleSlot.reset(); // Slot = nullptr will give compilation error
    }

    // ~Vehicle() = default;
    // SDE-2 Standard: Virtual destructor for base classes
    virtual ~Vehicle() { 
        std::cout << "Vehicle " << licensePlate << " destroyed.\n"; 
    }
};

class parkingSlot{
    const int slotId;
    bool isOccupied; // CHANGE: Renamed 'isEmpty' to 'isOccupied' to fix logic flip
    weak_ptr<Vehicle> vehicle;
public:
    // parkingSlot(int slotId) : slotId(slotId){
    //     isEmpty = false;
    // };
    // CHANGE: Initializer list is the preferred C++ way
    explicit parkingSlot(int id) : slotId(id), isOccupied(false) {}

    // CHANGE: Const-correctness. This method doesn't change the slot's state.
    bool getStatus() const { return isOccupied; }

    void parkVehicle(shared_ptr<Vehicle> vehicle){
        isOccupied = true;
        this -> vehicle = vehicle;
    }

    void unParkVehicle(){
        isOccupied = false;
        vehicle.reset(); // vehicle = nullptr will give compilation error
    }

    ~parkingSlot() { 
        std::cout << "Slot " << slotId << " destroyed.\n"; 
    }
};

int main() {
    // Both objects are managed by shared_ptrs (The "Owners")
    auto myCar = std::make_shared<Vehicle>("ABC-123");
    auto mySlot = std::make_shared<parkingSlot>(101);

    // Linking them (The "Weak" relationship)
    mySlot->parkVehicle(myCar);
    myCar->parkVehicle(mySlot);

    if (mySlot->getStatus()) {
        std::cout << "Slot 101 is now occupied by car " << myCar->getPlate() << "\n";
    }

    return 0;
} // Both myCar and mySlot go out of scope and are deleted safely here.


// I marked Vehicle as virtual because it is a Base Class (intended to be inherited by Car, Bike, Truck). 
// I left ParkingSlot non-virtual because, in that specific snippet, it was treated as a Final/Leaf Class (no intended children)
// In C++, if you delete a derived object (like Car) through a base pointer (like Vehicle*), 
// and the destructor is not virtual, Undefined Behavior occurs. 
// Usually, only the base part of the object is destroyed, causing a memory leak of the derived part.
// The Result WITHOUT virtual:
// Only "Base Destructor" is printed. The specializedParts in Car are never deleted. This is a classic memory leak.
// The Result WITH virtual:
// C++ looks at the "Virtual Table" (vtable), sees it's actually a Car, and calls ~Car() first, then ~Vehicle(). Everything is cleaned up.

// SDE-2 Rule of Thumb:
// If your class has even one virtual function, its destructor MUST be virtual. 
// In LLD interviews, we often make all entity destructors virtual just to be safe for future requirements.