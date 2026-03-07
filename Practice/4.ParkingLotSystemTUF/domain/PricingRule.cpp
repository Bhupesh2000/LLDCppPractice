#include "PricingRule.h"

PricingRule::PricingRule(VehicleType vehicleType, double ratePerHour, double flatRate)
    : id(IdGenerator::generate()),
      vehicleType(vehicleType),
      ratePerHour(ratePerHour),
      flatRate(flatRate) {}

void PricingRule::updateRates(double ratePerHour, double flatRate) {
    this->ratePerHour = ratePerHour;
    this->flatRate = flatRate;
}

void PricingRule::updateFlatRate(double flatRate) {
    this->flatRate = flatRate;
}

void PricingRule::updateHourlyRate(double ratePerHour) {
    this->ratePerHour = ratePerHour;
}

long long PricingRule::getId() const {
    return id;
}

VehicleType PricingRule::getVehicleType() const {
    return vehicleType;
}

double PricingRule::getRatePerHour() const {
    return ratePerHour;
}

double PricingRule::getFlatRate() const {
    return flatRate;
}

std::string PricingRule::toString() const {
    return "PricingRule{id=" + std::to_string(id) +
           ", vehicleType=" + std::to_string(static_cast<int>(vehicleType)) +
           ", ratePerHour=" + std::to_string(ratePerHour) +
           ", flatRate=" + std::to_string(flatRate) +
           "}";
}

// ## Why PricingRule Does Not Use Mutex

// PricingRule is usually **configuration data**.

// * Created during system initialization.
// * Rarely updated (only by admin operations).
// * Mostly read by multiple threads.

// Concurrency control is handled at the **service level**, not inside the entity.

// Example:

// ```
// PricingService
//    └─ mutex protects collection of PricingRule objects
// ```

// ### Design Principle

// ```
// Entities → simple data + behavior
// Services → handle concurrency and locking
// ```
