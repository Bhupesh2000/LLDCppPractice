// It defines a one-to-many dependency between objects so that when one object (the Subject) changes state, all its dependents (Observers) are notified and updated automatically.
// You have a central node (The Subject).
// It keeps a list of "neighbor" nodes (The Observers).
// When something happens to the Subject, it iterates through its list and calls a function on every neighbor.

#include <iostream>
#include <memory>
#include <vector>
#include <string>
#include <algorithm>

// 1. The Observer Interface
class ISubscriber{
public:
    virtual void update(const std::string& news) const = 0;
    virtual ~ISubscriber() = default;
};

// 2. The Subject (The News Agency)
class NewsAgency{
private:
    std::vector<std::weak_ptr<ISubscriber>> subscribersList; // weak_ptr because News Agency doesn't own the subscriber. So the subscriber can get deleted somewhere but new agency will still try to notify it

public:
    void addSubscriber(std::weak_ptr<ISubscriber> subscriber){
        subscribersList.push_back(subscriber);
    }

    void removeSubscriber(std::weak_ptr<ISubscriber> subscriber){
        subscribersList.erase(std::remove(subscribersList.begin(), subscribersList.end(), subscriber), subscribersList.end());
    }

    void notify(const std::string& news){
        for(auto& sub : subscribersList){
            // The "Push" model
            // sub -> update(news); // will be used if ISubscriber is shared_ptr rather than weak_ptr
            if(auto subShared = sub.lock()) subShared -> update(news);
            // else removeSubscriber(sub); <---- DANGER(don't do this). 
            // If you remove an element from a std::vector while iterating over it, you invalidate the iterators. This will likely cause a crash or undefined behavior.
            // FIX - Usually, we use the "Erase-Remove" idiom after the loop, or we collect "dead" indices and clean them up later.
        }
    }
};

// 3. Concrete Observers
class PhoneUser : public ISubscriber {
public:
    void update(const std::string& news) const override {
        std::cout << "Phone notification: " << news << "\n";
    }
};

class LaptopUser : public ISubscriber {
public:
    void update(const std::string& news) const override {
        std::cout << "Email update on laptop: " << news << "\n";
    }
};

int main() {

    

    return 0;
}