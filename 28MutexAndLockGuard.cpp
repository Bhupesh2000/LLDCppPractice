// If using mtx.lock() and mtx.unlock() your code threw an exception or returned early between those two lines, the mutex would stay locked forever (a Deadlock).

// We use std::lock_guard.

#include <iostream>
#include <mutex>
#include <memory>
#include <vector>

class IOrderState{};

class Order {
private:
    std::unique_ptr<IOrderState> currentState;
    std::mutex stateMutex; // The "Key" for this specific Order

public:
    void setState(std::unique_ptr<IOrderState> newState) {
        // This 'guard' locks the mutex NOW. 
        // It will unlock automatically when the function ends.
        std::lock_guard<std::mutex> guard(stateMutex); 
        
        currentState = std::move(newState);
        std::cout << "State updated safely!\n";
    }
};


// Thread-Safe Singleton - The language specification guarantees that the initialization of a static local variable is thread-safe. The compiler internally puts a "guard" around it. You don't need to write your own mutex logic for Singletons anymore.

// In "High Read, Low Write" scenarios, NotificationService(done previously) - 
// Reads: Thousands of notifications are sent (reading the subscribersList).
// Writes: Occasionally, a new user joins (writing to the subscribersList).
// If we use a standard std::mutex, only one thread can read at a time. This is a massive waste of resources!

// Solution - std::shared_mutex
// It allows for two types of locking:
// Shared Lock (Reader Lock): Multiple threads can hold this lock simultaneously. If 10 threads want to read, they all can.
// Unique Lock (Writer Lock): Only one thread can hold this. If someone wants to write, they must wait for all readers to finish, and while they write, no one else can read or write.


#include <shared_mutex> // The new tool

class ISubscriber {
public:
    virtual void update(const std::string& msg) = 0;
    virtual ~ISubscriber() = default;
};


class NotificationService {
private:
    std::vector<std::shared_ptr<ISubscriber>> subscribers;
    // In C++, when a member function is marked const, it promises not to modify the object’s logical state.
    // mutable allows a class data member to be modified even inside a const member function.
    mutable std::shared_mutex rwMutex; // "rw" stands for Read-Write

public:
    void addSubscriber(std::shared_ptr<ISubscriber> sub) {
        // WRITER: Needs exclusive access
        std::unique_lock lock(rwMutex); 
        subscribers.push_back(sub);
    }

    void notifyAll(const std::string& msg) {
        // READER: Multiple threads can run this loop at once!
        std::shared_lock lock(rwMutex); 
        for (const auto& sub : subscribers) {
            sub->update(msg);
        }
    }
};

// Thread-Safety Dilemma
// let's combine Thread Safety with the Observer Pattern

// The Scenario:
// Thread A is inside the notifyAll() loop. It has a shared_ptr to a Subscriber.
// Thread B calls removeSubscriber() or the Subscriber is deleted elsewhere.

// The Danger:
// If we use std::weak_ptr, Thread A must call lock() to promote it to a shared_ptr.
// What if the object is being destroyed exactly while Thread A is calling lock()?
// What if the Subscriber's update() function calls removeSubscriber() on itself? (This is called Re-entrancy, and it causes a Deadlock!)

// Solution: "Lock, Copy, and Release"
// To keep the system responsive, we don't hold the lock while calling the update() function (which might be slow). Instead:
// -> Lock the vector.
// -> Copy the vector of pointers to a local variable.
// -> Unlock the vector immediately.
// Loop through the local copy to notify everyone.
// Why? This way, the NotificationService is only locked for a few microseconds (the time it takes to copy a vector), and the slow update() calls happen outside the lock.

// The "Snapshot" (Eventual Consistency)
// In the "Lock, Copy, and Release" strategy, if a new user joins after I've made my copy, they won't get the current notification.
// Is this okay? In LLD, Yes. It is called "Eventual Consistency." As long as they get the next notification, the system is considered correct. It’s better to have a slightly delayed notification for one person than to freeze the entire system for everyone.

// The Golden Rule of Locks
// To prevent Deadlocks, the rule is: Always acquire locks in a globally defined order.
// Example: If you need Lock A and Lock B, every thread in the entire program must agree to always take A first, then B. If Thread 1 takes A → B and Thread 2 takes B → A, they will eventually collide and die.


// Eg - Imagine a Database Connection Pool. You have 10 connections. 100 threads want to use them.

class Connection{
public:
    void query(const std::string& q){std::cout << "Executing" << q << "\n";}
};

class ConnectionPool{
    std::vector<std::unique_ptr<Connection>> pool;
    std::mutex mtx; // Protects the vector

public:
    ConnectionPool(int size){
        for(int i = 0; i < size; i++) pool.push_back(std::make_unique<Connection>());
    }

    std::unique_ptr<Connection> acquire(){
        std::lock_guard<std::mutex> lock(mtx); // Thread-safe check
        if (pool.empty()) return nullptr;

        // Take the last connection out of the pool
        auto conn = std::move(pool.back());
        pool.pop_back();
        return conn;
    }

    void release(std::unique_ptr<Connection> conn){
        std::lock_guard<std::mutex> lock(mtx); // Thread-safe return
        pool.push_back(std::move(conn));
    }
};