Requirements -
The vending machine should support multiple products with different prices and quantities.
The machine should accept coins and notes of different denominations.
The machine should dispense the selected product and return change if necessary.
The machine should keep track of the available products and their quantities.
The machine should handle multiple transactions concurrently and ensure data consistency.
The machine should provide an interface for restocking products and collecting money.
The machine should handle exceptional scenarios, such as insufficient funds or out-of-stock products.

-------------------------------------------------------------------------------------------------------------------------------

System at high level(Major components) - 
| Concern                 | Must Be Owned By                             |
| ----------------------- | -------------------------------------------- |
| Product quantities      | Inventory component                          |
| Machine cash reserve    | Cash/Coin manager                            |
| Per-user money inserted | Transaction (ephemeral)                      |
| Orchestration           | VendingMachine (stateless or near-stateless) |

-------------------------------------------------------------------------------------------------------------------------------

Locking strategy
| Component      | Lock? | Reason                       |
| -------------- | ----- | ---------------------------- |
| Inventory      | ✅     | Shared product quantities    |
| Cash Reserve   | ✅     | Shared coins/notes           |
| Transaction    | ❌     | Thread-confined, user-scoped |
| VendingMachine | ❌     | Orchestrator only            |

-------------------------------------------------------------------------------------------------------------------------------

Locking of Inventory-
Scenario:
A single transaction wants to buy:
Product A
Product B
Two threads do:
T1 locks A → then B
T2 locks B → then A
💥 Deadlock.

“Lock one product, work on it, release, then lock the next product” <- wrong
Steps you proposed:
Lock A → decrement A → unlock A
Lock B → decrement B → unlock B
Now imagine:
After step 1, another transaction consumes B
Step 2 fails (B out of stock)
Result:
A already decremented
B not dispensed
Transaction is partially applied
❌ No atomicity, no rollback

Correct Direction
When a transaction needs multiple shared resources, you must:
Either lock them together
Or enforce an ordering rule
Releasing intermediate locks is not allowed when invariants span multiple resources.

✅ Lock Ordering Strategy (Gold Standard Answer)
Rule:
Always acquire multiple product locks in a globally consistent order.
Example:
Each product has a unique, immutable productId
Sort required products by productId
Acquire locks strictly in ascending order
Release in reverse order

Why this works
Deadlock requires a cycle
Global ordering eliminates cycles
No retries
No livelock
Preserves atomicity

-------------------------------------------------------------------------------------------------------------------------------

Final Design Snapshot (Conceptual)
Transaction
Thread-confined
No locks
Owns inserted money and selected products

Inventory
Per-product mutexes
Lock ordering by productId

CashReserve
Single mutex (or per-denomination if extended)
Locked after inventory

VendingMachine
Stateless orchestrator
Enforces lock order
No shared mutable state

-------------------------------------------------------------------------------------------------------------------------------

