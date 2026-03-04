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


Step 2
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

