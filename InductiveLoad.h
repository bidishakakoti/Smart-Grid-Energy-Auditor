// include/InductiveLoad.h
#pragma once
#include "Appliance.h"
#include <string>

class InductiveLoad : public Appliance {
private:
    std::string inductiveType;  // e.g. "AC", "Motor", "Fan"
    float standbyDrawWatts;     // Fixed standby consumption

public:
    InductiveLoad(const std::string& name,
                  float ratedWattage,
                  float voltage,
                  float current,
                  float powerFactor,
                  const std::string& inductiveType,
                  float standbyDrawWatts = 5.0f);

    // Override pure virtual — inductive loads use V x I x PF
    float calculateLoad() const override;

    // Override display to show power factor warning if low
    void displayInfo() const override;

    std::string getType() const;
};