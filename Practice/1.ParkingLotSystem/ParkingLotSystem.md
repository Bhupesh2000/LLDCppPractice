Goal (summary):
Design a parking lot system that supports multiple vehicle types, slot allocation, ticketing, and payment. We will incrementally refine the design and later code it in C++.

Requirements (initial)
Vehicle types: Bike, Car, Truck
Slot types: Small, Medium, Large
A vehicle can be parked only in a compatible slot
On entry:
    Assign a slot
    Generate a parking ticket
On exit:
    Calculate fee
    Free the slot

Constraints
Single parking lot
Single entry and exit (for now)
Ignore UI, persistence, and DB


Notes - 
1. Keep check of spelling mistaked
2. int getSlotId(){} // no need of const int getSlotId() in case of return by value
3. Constructor(SlotType slotType) -> no need of const SlotType slotType, pass enum by value
4. define an alias -> using TimePoint = std::chrono::system_clock::time_point;
5. read about chrono
6. time now = std::chrono::system_clock::now()
7. std::optional<T> represents:
    either a value of type T
    or no value at all
8. std::find(v.begin(), v.end(), type) != v.end() // #include <algorithm>
9. const_cast read about it(might be required in some cases, try to avoid it)
10. unique_ptr<T>::get() -> return *T
    Does not:
        Delete the object
        Change ownership
        Increase reference count
    Ownership stays with the unique_ptr
11. std::optional



SlotManager tasks - 
assign a slot
    needs to know available and non-available slots(3 vectors)
    needs to know the slot selection startegies(call its interface)
park and unpark
