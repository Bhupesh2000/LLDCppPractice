#ifndef PRICING_RULE_H
#define PRICING_RULE_H

#include <string>
#include <utils/IdGenerator.h>
#include <enums/VehicleType.h>

class PricingRule {
private:
    long long id;
    VehicleType vehicleType;
    double ratePerHour;
    double flatRate;

public:
    PricingRule(VehicleType vehicleType, double ratePerHour, double flatRate);

    void updateRates(double ratePerHour, double flatRate);
    void updateFlatRate(double flatRate);
    void updateHourlyRate(double ratePerHour);

    long long getId() const;
    VehicleType getVehicleType() const;
    double getRatePerHour() const;
    double getFlatRate() const;

    std::string toString() const;
};

#endif