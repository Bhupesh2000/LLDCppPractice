Step 1
Functional Requirements
Entry Flow
    Vehicle arrives at gate
    Assign slot based on vehicle type
    Generate ticket
    Mark slot as occupied
    Return entry response
Exit Flow
    Present ticket
    Calculate fee using pricing rules (flat vs hourly)
    Process payment
    Release slot
    Return exit response with receipt
Admin Flow
    Add/Edit/Delete floors and slots
    Define/update pricing rules
    View parking lot status

Interview Tip: Always ask the interviewer to confirm the requirements before jumping into code. It shows clarity and alignment.
Think of some cases like system failure, where some kind of human override might be required
Another cases - What if the ticket is lost, what if the person uses a duplicate/false ticket


NON-FUNCTIONAL REQUIREMENTS:
Scalability: Must support multiple parking lots and thousands of slots
Consistency: Strong consistency for slot allocation and release
Availability: High availability for Entry/Exit even during payment gateway failure
Latency: Low latency (<500ms) for ticket generation and exit processing
Extensibility: Easily add new vehicle types, pricing strategies, or gateways
Security: Role-based access for admin actions

EDGE CASES:
Payment failure during exit – retry and hold slot
Ticket lost – allow admin override
Clock skew – system time validation, // very critical, for eg. 1 hr 1 min -> 2 hrs or if your backend server is in US and your system is in india
Slot state mismatch – periodic reconciliation


Step 2
Entities

1. Vehicle
    id : UUID(PK)
    licencePlate : String
    vehicleType : Enum(BIKE, CAR, TRUCK, EV)

2. ParkingSlot
    id : UUID(PK)
    slotType : Enum(BIKE, CAR, TRUCK, EV)
    floorNumber : int
    isOccupied : Boolean

3. Floor
    id : UUID(PK)
    floorNumber : int
    slots : vector<ParkingSlot>

4. Ticket
    id : UUID(PK)
    vehicleId : UUID(FK -> Vehicle.id)
    slotId : UUID(FK -> ParkingSlot.id)
    entryTime : Timestamp
    isActive : Boolean

5. Receipt
    id : UUID(PK)
    ticketId : UUID(FK -> Ticket.id)
    exitTime : Timestamp
    totalFee : DOUBLE
    PaymentStatus : Enum(SUCCESS, PENDING, FAILED)
// PaymentStatus can be debatable. Can be skipped as well.
// One can argue that I'll generate Receipt only when once the payment is completed. And other thing can be once receipt is generated with PENDING status and can be moved to SUCCESS/FAILURE status based on the payment.

6. PricingRule
    id : UUID(PK)
    vehicleType : Enum
    ratePerHour : DOUBLE
    flatRate : DOUBLE
    ruleType : Enum(FLAT, HOURLY)

7. Payment
    id : UUID(PK)
    ticketId : UUID(FK -> Ticket.Id)
    amount : DOUBLE
    gateway : Enum(Rajorpay, Stripe)
    status : Enum(PENDING, SUCCESS, FAILED)
// Status for payment is required for lets say if you want to check how many payments failed in the day. You can query that using payment status

DTO's(Data Transfer Objects) : The objects which APIs are going to use
1. EntryResult
    success : Boolean
    ticket : Ticket(if successful)
    message : String
// this message is basically if you want to tell a specific reason if success is false. Lets say no slot available

2. ExitResult
    success : Boolean
    receipt : Receipt(if successful)
    message : String

    