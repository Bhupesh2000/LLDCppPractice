// A design pattern that ensures a class has only one instance and provides a global point of access to it.
// A Logger (everyone should write to the same file).
// A DatabaseConnectionPool.
// A ConfigurationManager (you don't want 10 different versions of your app's settings).

#include <iostream>
#include <mutex>

class ConfigurationManager{
    // 1. Private Constructor: No one else can say 'new ConfigurationManager()'
    ConfigurationManager(){
        std::cout << "Loading configuration manager\n";
    }

public:
    // 2. Delete Copy Constructor and Assignment Operator
    // We don't want anyone cloning the singleton!
    ConfigurationManager(const ConfigurationManager&) = delete;
    void operator=(const ConfigurationManager&) = delete;

    // 3. Static Accessor
    static ConfigurationManager& getInstance(){
        // This is thread-safe in C++11/14/17/20+
        static ConfigurationManager instance;
        return instance;
    }

    void display() { std::cout << "App Mode: Production\n"; }
};

int main() {

    // ConfigurationManager cfg; // ERROR: Private constructor
    ConfigurationManager& cfg = ConfigurationManager::getInstance();
    cfg.display();

    return 0;
}


// 1. What does static mean here? (The "One-Time Setup")
// In normal programming, when you call a function, everything inside it is "temporary." It’s like writing on a whiteboard; when the function ends, you erase the board.
// The keyword static changes this. It tells the computer:
// "Create this item once, and keep it forever. Even when the function ends, do not erase this item. If someone calls this function again, just give them the same item you created the first time."
// 2. The code: static ConfigurationManager instance;
// When the code reaches this line, the computer does a quick check:
// Is this the first time we are here?
// Yes: Build the ConfigurationManager object (the "Vault").
// No: Do nothing. The "Vault" is already built.
// 3. The code: return instance; (The & symbol)
// The & means Reference. In simple terms, it means you are giving out the actual address of the object, not a "copy" of it.
// If I give you a copy of a key, you have your own key. If you break it, mine is fine.
// If I give you a reference to the front door, we are both looking at the exact same door. If you paint it red, it’s red for me too.
// The Singleton Goal: By returning a reference (&), we ensure that every part of your program is talking to the exact same piece of memory.
// 4. Why is it "Safe" in Modern C++? (The "Guard" Analogy)
// Imagine 100 people (Threads) all run into the library at the exact same millisecond, all shouting "I want the Dictionary!"
// In Old C++ (Pre-2011): There was a risk that two people would start building the Dictionary at the same time. You’d end up with two half-finished books, or a total mess.
// In Modern C++: The computer acts like a Strict Guard at the door of that function. Even if 100 people arrive at once, the Guard only lets one person in to build the object. Everyone else has to wait outside. Once the first person finishes building it, the Guard lets everyone else in to just use it.
// The computer handles all this "waiting and guarding" for you automatically because of how the C++ language is designed.
