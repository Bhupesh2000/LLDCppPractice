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

Class List -
VendingMachine
Product
Transaction
InventoryManager
CashManager
Payment (interface / stateless logic)
enum class Coin, enum class Note

-------------------------------------------------------------------------------------------------------------------------------

Notes - 
1. When some other component (e.g. VendingMachine, Transaction) needs product information, should InventoryManager:
    A) Return a Product (or reference to it)
    B) Or keep Product completely internal and only expose derived data (price, availability, etc.)?
If InventoryManager exposes Product directly:
Other components start depending on inventory’s internal structure
Harder to refactor later
Inventory leaks domain knowledge
If it does not expose Product:
Inventory becomes a strict boundary
Cleaner responsibilities
Slightly more boilerplate, but safer design

InventoryManager should expose read-only access to Product metadata, either by returning a const Product& or a copy, but never allow mutation. Inventory remains the owner of stock state, not product identity.

2. If your map stores owning pointers (raw pointers or smart pointers), the correct approach to clear memory depends on the pointer type.
Case 1: std::map<int, Struct*> (raw pointer — manual memory management)
auto it = mp.find(key);
if (it != mp.end()) {
    delete it->second;   // free heap memory
    mp.erase(it);        // remove map entry
}
Case 2: std::map<int, std::unique_ptr<Struct>>
mp.erase(key);
erase() destroys the unique_ptr
unique_ptr destructor automatically deletes the object
Case 3: std::map<int, std::shared_ptr<Struct>>
mp.erase(key);
Memory is freed automatically when reference count becomes zero.

3. State owned by CashManager
    You should be thinking in terms of machine reserve only:
    Counts of coins per denomination
    Counts of notes per denomination
    No transaction-scoped money
    No totals without denomination breakdown
This is long-lived, shared, mutable state.

Operations CashManager must support
At minimum, it must support both directions of money flow:
1. Accept money into reserve
    After a successful transaction
2. Check if exact change is possible
    Before committing a transaction
3. Dispense change
    Atomically reduce denomination counts
4. Admin operations
    Collect money
    Refill denominations

4. I initially thought Payment should only check feasibility (inventory availability and change possibility) and return a boolean, with VendingMachine later reducing inventory and dispensing change based on that result. However, this design is unsafe in a concurrent system. Between the “check” and the “commit”, shared state (inventory or cash reserves) may change due to other transactions, leading to race conditions, partial failures, and broken invariants. Therefore, the correct design is that the same component that validates shared mutable state must also commit it atomically. Payment must both validate and commit inventory consumption and change dispensing (via InventoryManager and CashManager), while not mutating the Transaction state. VendingMachine remains the orchestrator: it computes derived values (like total price), calls Payment::pay(), and updates the transaction status based on success or failure. This cleanly separates orchestration from policy and ensures correctness under concurrency.

