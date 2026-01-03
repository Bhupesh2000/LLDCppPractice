// "Software entities (classes, modules, functions, etc.) should be open for extension, but closed for modification."

#include<iostream>
#include<vector>

//Bad Way
enum class ShapeType { Circle, Rectangle };

struct Shape { ShapeType type; };

class AreaCalculator {
public:
    double calculate(const std::vector<Shape*>& shapes) {
        double area = 0;
        for (auto s : shapes) {
            if (s->type == ShapeType::Circle) { 
                // area += Math for circle 
            }
            else if (s->type == ShapeType::Rectangle) { 
                // area += Math for rectangle 
            }
        }
        return area;
    }
};

//Correct Way
class IShape {
public:
    virtual double getArea() const = 0;
    virtual ~IShape() = default;
};

// Now AreaCalculator is CLOSED for modification.
class AreaCalculator {
public:
    double totalArea(const std::vector<std::shared_ptr<IShape>>& shapes) {
        double total = 0;
        for (const auto& s : shapes) {
            total += s->getArea(); // Polymorphism handles the "type"
        }
        return total;
    }
};
