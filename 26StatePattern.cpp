// it’s about "shifting" responsibilities

// Imagine a high-end flashlight with one single button.
// If the Flashlight is OFF: You press the button → It turns on (Low Light).
// If the Flashlight is on LOW: You press the same button → It turns on (High Light).
// If the Flashlight is on HIGH: You press the same button → It turns off.

// The "Bad" Design (The if-else Mess):
// You have one class Flashlight with a variable int state.
// void pressButton() {
//     if (state == OFF) { 
//         state = LOW; 
//         std::cout << "Low Light\n";
//     } else if (state == LOW) { 
//         state = HIGH; 
//         std::cout << "High Light\n";
//     } else if (state == HIGH) { 
//         state = OFF; 
//         std::cout << "Turned Off\n";
//     }
// }


// Instead of the Flashlight class trying to remember all the rules, we create State Objects. We tell the Flashlight: "Don't worry about the rules. Just ask your 'Current State' what to do."
// The Flashlight (Context): Only knows it has a pointer to a State.
// The States (Locked, Low, High): Each state is a separate class. They know exactly what happens when the button is pressed in their specific state.


#include <iostream>
#include <memory>
#include <vector>
#include <string>

class FlashLight{}; // Forward declaration so States can "see" the FlashLight

// 1. The Interface
class IFlashLightState{
public:
    virtual void pressButton(FlashLight* f) = 0;
    virtual std::string getName() const = 0;
    virtual ~IFlashLightState() = default;
};

// 2. The Context (The FlashLight)
class FlashLight{
    std::unique_ptr<IFlashLightState> currentState;

public:
    FlashLight(std::unique_ptr<IFlashLightState> initialState) : currentState(std::move(initialState)){}

    void setState(std::unique_ptr<IFlashLightState> newState){
        currentState = std::move(newState);
    }

    void pressButton(){
        currentState -> pressButton(this);
    }

};

// 3. Concrete States (The "Rules")

// We define the classes first
class HighState;
class MediumState;
class LowState;
class OffState;

class HighState : public IFlashLightState{
public:
    std::string getName() const override{ return "HIGH"; }
    void pressButton(FlashLight* f) override; // Transition logic defined below
};

class MediumState : public IFlashLightState{
public:
    std::string getName() const override{ return "MEDIUM"; }
    void pressButton(FlashLight* f) override; // Transition logic defined below
};

class LowState : public IFlashLightState{
public:
    std::string getName() const override{ return "LOW"; }
    void pressButton(FlashLight* f) override; // Transition logic defined below
};

class OffState : public IFlashLightState{
public:
    std::string getName() const override{ return "OFF"; }
    void pressButton(FlashLight* f) override; // Transition logic defined below
};

// --- Transition Definitions (The "Relay Race" logic) ---

// Off -> Low
void OffState::pressButton(FlashLight* f) {
    std::cout << "Switching to LOW beam.\n";
    f->setState(std::make_unique<LowState>()); // In high performing system, rather than creating new object each time, create all the objects at the starting and pass them around
}

// Low -> Medium
void LowState::pressButton(FlashLight* f) {
    std::cout << "Switching to MEDIUM beam.\n";
    f->setState(std::make_unique<MediumState>());
}

// Medium -> High
void MediumState::pressButton(FlashLight* f) {
    std::cout << "Switching to HIGH beam.\n";
    f->setState(std::make_unique<HighState>());
}

// High -> Off
void HighState::pressButton(FlashLight* f) {
    std::cout << "Turning OFF.\n";
    f->setState(std::make_unique<OffState>());
}

// 4. Execution
int main() {
    // Start the FlashLight in the OFF state
    FlashLight myFlashLight(std::make_unique<OffState>());

    // Simulate pressing the button multiple times
    myFlashLight.pressButton(); // OFF -> LOW
    myFlashLight.pressButton(); // LOW -> MEDIUM
    myFlashLight.pressButton(); // MEDIUM -> HIGH
    myFlashLight.pressButton(); // HIGH -> OFF
    myFlashLight.pressButton(); // OFF -> LOW

    return 0;
}