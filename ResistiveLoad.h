// include/ResistiveLoad.h
#pragma once
#include "Appliance.h"
#include <string>

class ResistiveLoad : public Appliance {
private:
    std::string resistiveType; // e.g. "Heater", "Bulb", "Iron"

public:
    ResistiveLoad(const std::string& name,
                  float ratedWattage,
                  float voltage,
                  float current,
                  const std::string& resistiveType);

    // Override pure virtual — resistive loads have PF = 1.0
    float calculateLoad() const override;

    // Override display to show type
    void displayInfo() const override;

    std::string getType() const;
};