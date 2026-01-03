// The Abstract Factory ensures you get a "Family" of related objects that are designed to work together.
// The Problem Scenario: Cross-Platform UI
// Imagine you are building a UI library that works on Windows and Mac.
// Windows has WindowsButton and WindowsCheckbox.
// Mac has MacButton and MacCheckbox.
// If your code manually creates a WindowsButton but then accidentally creates a MacCheckbox, the app will look broken or crash.
// The Solution: The Abstract Factory
// Instead of asking for a "Button," the code asks the PlatformFactory for a button.

#include <iostream>
#include <memory>
#include <vector>
#include <string>

// 1. Abstract Products
class IButton{public: virtual void paint() = 0; virtual ~IButton() = default;};
class ICheckBox{public: virtual void render() = 0; virtual ~ICheckBox() = default;};

class WindowsButton : public IButton{
public:
    void paint(){}
};
class MacButton : public IButton{
    void paint(){}
};
class WindowsCheckBox : public ICheckBox{
    void render(){}
};
class MacCheckBox : public ICheckBox{
    void render(){}
};

// 2. The Abstract Factory (The Interface)
class IUIFactory{
public:
    virtual std::unique_ptr<IButton> createButton() = 0;
    virtual std::unique_ptr<ICheckBox> createCheckBox() = 0;
    virtual ~IUIFactory() = default;
};

// 3. Concrete Factories (One for each "Family")
class WindowsFactory : public IUIFactory{
public:
    std::unique_ptr<IButton> createButton() override{
        return std::make_unique<WindowsButton>();
    }
    std::unique_ptr<ICheckBox> createCheckBox() override{
        return std::make_unique<WindowsCheckBox>();
    }
};

class MacFactory : public IUIFactory{
public:
    std::unique_ptr<IButton> createButton() override{
        return std::make_unique<MacButton>();
    }
    std::unique_ptr<ICheckBox> createCheckBox() override{
        return std::make_unique<MacCheckBox>();
    }
};

// How the Client (The logic) uses it:
void initialize(IUIFactory& factory){
    auto btn = factory.createButton();
    auto checkbx = factory.createCheckBox();

    btn -> paint();
    checkbx -> render();
}

int main() {
    return 0;
}