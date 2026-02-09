#include <iostream>
#include <memory>
#include <vector>
#include <string>
#include <map>
#include <unordered_map>
#include <vector>
#include <algorithm>
#include <mutex>
#include <optional>

class Product{
    const int productId;
    const std::string productName;
    const int price;

public:
    Product(int productId, const std::string& name, int price) : productId(productId), productName(name), price(price){}

    int getProductId() const{
        return productId;
    }

    const std::string getProductName() const{
        return productName;
    }

    int getPrice() const{
        return price;
    }
};

struct Stock{
    Product product;  // immutable, value type(unique_ptr will introduce unncessary ownership), Product is immutable metadata and can be shared safely.
    int availableQty; // mutable, protected
    mutable std::mutex productMtx; // mutable, protected

    Stock(Product product, int qty) : product(std::move(product)), availableQty(qty){}
};

class InventoryManager{
    std::map<int, std::unique_ptr<Stock>> inventory;
    mutable std::mutex inventoryLock; // required while adding new product and removing product from inventory map

public:
    bool addProduct(Product product, int qty){
        int productId = product.getProductId();
        std::lock_guard<std::mutex> guard(inventoryLock);
        // if(inventory.count(productId) != 0) return false;
        // inventory[productId] = std::make_unique<Stock>(product, qty); // Missing std::move(product), This causes an extra copy of Product.
        // using count() + mutation:
        // This is correct under the lock, but:
        // It does two lookups
        // operator[] is semantically misleading (it suggests default insertion)
        // Preferred (interview-cleaner):
        // Use find() + emplace()

        if (inventory.find(productId) != inventory.end()) {
            return false;
        }

        inventory.emplace(
            productId,
            std::make_unique<Stock>(std::move(product), qty)
        );
        return true;
    }

    bool removeProduct(int productId){
        std::lock_guard<std::mutex> guard(inventoryLock);
        // if(inventory.count(productId) == 0) return false;
        // inventory.erase(productId);

        auto it = inventory.find(productId);
        if (it == inventory.end()) {
            return false;
        }

        inventory.erase(it);
        return true;
    }

    const Product* getProduct(int productId) const{
        std::lock_guard<std::mutex> guard(inventoryLock);

        auto it = inventory.find(productId);
        if(it == inventory.end()) return nullptr;

        return &it -> second ->product;
    }

    

    int getAvailableQty(int productId) const{
        std::lock_guard<std::mutex> guard(inventoryLock);
        auto it = inventory.find(productId);
        if(it == inventory.end()) return -1;

        Stock* s = it -> second.get();

        std::lock_guard<std::mutex> guard(s -> productMtx);
        return s -> availableQty;
    }

    bool tryConsume(int productId, int qty){
        if (qty <= 0) return false;
        std::lock_guard<std::mutex> guard(inventoryLock);

        auto it = inventory.find(productId);
        if(it == inventory.end()) return false;

        Stock* s = it -> second.get();
        std::lock_guard<std::mutex> guard(s -> productMtx);

        if(s -> availableQty < qty) return false;

        s -> availableQty -= qty;
        return true;
    }

    bool tryConsumeTransaction(const std::unordered_map<int, int>& productIdWithQty){
        std::lock_guard<std::mutex> guard(inventoryLock);

        // Phase 1: validation (no mutation)
        for (const auto& [productId, qty] : productIdWithQty) {
            auto it = inventory.find(productId);
            if (it == inventory.end()) {
                return false;
            }

            Stock* stock = it->second.get();
            if (stock->availableQty < qty) {
                return false;
            }
        }

        // Phase 2: commit (all-or-nothing)
        for (const auto& [productId, qty] : productIdWithQty) {
            inventory[productId]->availableQty -= qty;
        }

        return true;
    }

    void restock(int productId, int qty){
        if(qty <= 0) return;
        std::lock_guard<std::mutex> guard(inventoryLock);

        auto it = inventory.find(productId);
        if(it == inventory.end()) return;

        Stock* s = it -> second.get();
        std::lock_guard<std::mutex> guard(s -> productMtx);
        
        s -> availableQty += qty;
    }

};

enum class Denomination{
    FIVE = 5, TEN = 10, TWENTY = 20, FIFTY = 50, HUNDRED = 100
};

class CashManager{
    std::map<Denomination, int> reserves;
    mutable std::mutex cashMtx;

public:
    bool canMakeChange(int amount){
        if (amount <= 0) return false;

        std::lock_guard<std::mutex> guard(cashMtx);

        auto it = reserves.rbegin(); // rbegin and rend, because map will be sorted in ascending order. So begin will go 5 -> 10 -> 20 -> 50 -> 100. but we want from 100 -> .. -> 5.

        while(it != reserves.rend() && amount > 0){
            int val = static_cast<int>(it ->first); //100 - Denomination::FIVE will not compile directly because enum class is strongly typed and does not implicitly convert to integers. You must explicitly cast the enum to an integer (e.g., 100 - static_cast<int>(Denomination::FIVE)), resulting in a valid integer subtraction (\(100-5=95\)). 
            int denomsRequired = amount / val;
            amount -= (val * std::min(denomsRequired, it -> second));
            it ++;
        }

        return amount == 0;
    }

    std::optional<std::unordered_map<Denomination, int>> dispenseChange(int amount){
        if (amount <= 0) return std::nullopt;

        std::lock_guard<std::mutex> guard(cashMtx);
        std::unordered_map<Denomination, int> changeMap;
        auto it = reserves.rbegin();

        while(it != reserves.rend() && amount > 0){
            int val = static_cast<int>(it ->first);
            int denomsRequired = amount / val;
            int denomsCount = std::min(denomsRequired, it -> second);
            amount -= (val * denomsCount);

            if(denomsCount != 0){
                changeMap.emplace(it -> first, denomsCount);
            }
            it ++;
        }

        if(amount != 0) return std::nullopt; // not calling canMakeChange again because that will lead to taking locks two times, one in dispenseChange and other in canMakeChange. So handled the logic of canMakeChange here in the dispenseChange itself. Also this will save some time. O(2N) -> O(N)
        
        // commit to reserves
        for(auto& [denom, cnt] : changeMap){
            reserves[denom] -= cnt;
        }

        return changeMap;
    }

    void addCash(Denomination denom, int count){
        if(count <= 0) return;

        std::lock_guard<std::mutex> guard(cashMtx);
        auto it = reserves.find(denom);

        if(it != reserves.end()) reserves[it -> first] += count;
        else reserves.emplace(denom, count);
    }

    void collectAll(){  // admin operation
        std::lock_guard<std::mutex> guard(cashMtx);

        for(auto& [denom, cnt] : reserves){
            reserves[denom] = 0;
        }
    }
};

enum class TransactionStatus{
    CREATED, CONFIRMED, FAILED, COMPLETED
};

class Transaction{
    TransactionStatus status = TransactionStatus::CREATED;
    // std::unordered_map<Product, int> productList; <- Product will be a very heavy has key. Better to keep productId
    std::unordered_map<int, int> productIdToQty;
    std::unordered_map<Denomination, int> denomList;
    
public:
    Transaction() = default;

    TransactionStatus getCurrentStatus() const{
        return status;
    }

    const std::unordered_map<int, int>& getAllProductsWithQty() const{
        return productIdToQty;
    }

    const std::unordered_map<Denomination, int>& getAllDenomsWithQty() const{
        return denomList;
    }

    void addProduct(int productId, int qty){ // along with qty check also check the status(only allowed in created state)
        if(this -> status == TransactionStatus::CREATED && qty > 0){
            if(productIdToQty.find(productId) != productIdToQty.end()) productIdToQty[productId] += qty;
            else productIdToQty.emplace(productId, qty);
        }
    }

    void insertCash(Denomination denom, int qty){ // along with qty check also check the status(only allowed in created state)
        if(this -> status == TransactionStatus::CREATED && qty > 0){
            if(denomList.find(denom) != denomList.end()) denomList[denom] += qty;
            else denomList.emplace(denom, qty);
        }
    }

    bool markConfirmed(){ // CREATED -> CONFIRMED
        if(this -> status == TransactionStatus::CREATED){
            this -> status = TransactionStatus::CONFIRMED;
            return true;
        }
        return false;
    }

    bool markFailed(){ // CREATED / CONFIRMED -> FAILED (failure can happen at any time)
        if(this -> status == TransactionStatus::CREATED || this -> status == TransactionStatus::CONFIRMED){
            this -> status = TransactionStatus::FAILED;
            return true;
        }
        return false;
    }

    bool markCompleted(){ // CONFIRMED -> COMPLETED
        if(this -> status == TransactionStatus::CONFIRMED){
            this -> status = TransactionStatus::COMPLETED;
            return true;
        }
        return false;
    }
};

struct PaymentResult{
    bool success;
    std::unordered_map<Denomination, int> change;
};
class Payment{
protected:
    InventoryManager& invMgr;
    CashManager& cashMgr;

public:
    Payment(InventoryManager& inv, CashManager& cash) : invMgr(inv), cashMgr(cash){}

    virtual PaymentResult pay(Transaction& transct) = 0;

    virtual ~Payment() = default;
};

class CashPayment : public Payment{
public:
    CashPayment(InventoryManager& inv, CashManager& cash) : Payment(inv, cash){}

    PaymentResult CashPayment::pay(Transaction& transct) {
        PaymentResult result{false, {}};

        const auto& productIdWithQty = transct.getAllProductsWithQty();
        const auto& denomList = transct.getAllDenomsWithQty();

        if (productIdWithQty.empty()) return result;

        // 1. Compute total price
        int totalPrice = 0;
        for (const auto& [productId, qty] : productIdWithQty) {
            const Product* product = invMgr.getProduct(productId);
            if (!product) return result;
            totalPrice += product->getPrice() * qty;
        }

        // 2. Compute total inserted cash
        int totalInserted = 0;
        for (const auto& [denom, qty] : denomList) {
            totalInserted += static_cast<int>(denom) * qty;
        }

        if (totalInserted < totalPrice) return result;
        int changeAmount = totalInserted - totalPrice;

        // 3. Consume inventory FIRST (rollback possible)
        if (!invMgr.tryConsumeTransaction(productIdWithQty)) {
            return result;
        }

        // 4. Add inserted cash to machine
        for (const auto& [denom, qty] : denomList) {
            cashMgr.addCash(denom, qty);
        }

        // 5. Dispense change
        std::optional<std::unordered_map<Denomination, int>> denomChange;
        if (changeAmount > 0) {
            denomChange = cashMgr.dispenseChange(changeAmount);
            if (!denomChange) {
                // rollback inventory (cash rollback not needed)
                for (const auto& [productId, qty] : productIdWithQty) {
                    invMgr.restock(productId, qty);
                }
                return result;
            }
        }

        // success
        result.success = true;
        if (denomChange) result.change = *denomChange;
        return result;
    }
};

class VendingMachine {
private:
    InventoryManager inventoryMgr;
    CashManager cashMgr;
    std::unique_ptr<Payment> paymentStrategy;

public:
    VendingMachine() {
        paymentStrategy = std::make_unique<CashPayment>(inventoryMgr, cashMgr);
    }

    // -------- User Flow --------

    Transaction createTransaction() {
        return Transaction{};
    }

    PaymentResult processPayment(Transaction& txn) {
        if (txn.getCurrentStatus() != TransactionStatus::CREATED) {
            return {false, {}};
        }

        PaymentResult result = paymentStrategy->pay(txn);

        if (result.success) {
            txn.markConfirmed();
            txn.markCompleted();
        } else {
            txn.markFailed();
        }

        return result;
    }

    // -------- Inventory APIs --------

    bool addProduct(const Product& product, int qty) {
        return inventoryMgr.addProduct(product, qty);
    }

    bool removeProduct(int productId) {
        return inventoryMgr.removeProduct(productId);
    }

    int getAvailableQty(int productId) const {
        return inventoryMgr.getAvailableQty(productId);
    }

    // -------- Cash/Admin APIs --------

    void addInitialCash(Denomination denom, int qty) {
        cashMgr.addCash(denom, qty);
    }

    void collectAllCash() {
        cashMgr.collectAll();
    }
};


int main() {

    VendingMachine vm;

    // admin setup
    vm.addProduct(Product(1, "Coke", 30), 10);
    vm.addInitialCash(Denomination::FIVE, 10);

    // user flow
    Transaction txn = vm.createTransaction();
    txn.addProduct(1, 1);
    txn.insertCash(Denomination::TWENTY, 1);
    txn.insertCash(Denomination::TEN, 1);

    PaymentResult result = vm.processPayment(txn);

    if (result.success) {
        // dispense product
        // dispense result.change
    }


    return 0;
}