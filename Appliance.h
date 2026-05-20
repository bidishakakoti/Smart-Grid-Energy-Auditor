// include/Appliance.h
#pragma once
#include <string>

#include "ApplianceState.h"

class Auditor; // Forward declaration — needed for friend

class Appliance {
private:
    float voltage;        // Private — encapsulation demo
    float current;        // Private — encapsulation demo
    float powerFactor;    // Private — only Auditor can inspect this

protected:
    std::string name;
    float ratedWattage;
    ApplianceState state;

public:
    // Constructors
    Appliance(const std::string& name, float ratedWattage,
              float voltage, float current, float powerFactor);
    virtual ~Appliance() = default;

    // Pure virtual — forces derived classes to implement their own logic
    virtual float calculateLoad() const = 0;

    // Virtual — can be overridden for custom display
    virtual void displayInfo() const;

    // Operator overloading — combines wattage of two appliances
    float operator+(const Appliance& other) const;

    // State management
    void setState(ApplianceState newState);
    ApplianceState getState() const;
    std::string getName() const;
    float getRatedWattage() const;

    // Friend declaration — Auditor gets special access to private members
    friend class Auditor;
};