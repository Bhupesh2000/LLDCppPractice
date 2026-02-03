#include <iostream>
#include <memory>
#include <vector>
#include <string>
#include <map>
#include <mutex>

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

int main() {

    

    return 0;
}