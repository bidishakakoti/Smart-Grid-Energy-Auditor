// src/InductiveLoad.cpp
#include "InductiveLoad.h"
#include <iostream>
#include <iomanip>

InductiveLoad::InductiveLoad(const std::string& name,
                             float ratedWattage,
                             float voltage,
                             float current,
                             float powerFactor,
                             const std::string& inductiveType,
                             float standbyDrawWatts)
    : Appliance(name, ratedWattage, voltage, current, powerFactor),
      inductiveType(inductiveType),
      standbyDrawWatts(standbyDrawWatts) {}

float InductiveLoad::calculateLoad() const {
    // Inductive loads: actual power = V x I x PF
    // We derive PF back via ratedWattage / (V x I) for the formula
    // but since powerFactor is private, we use ratedWattage as the
    // apparent power baseline — Auditor handles the PF inspection
    switch (state) {
        case ApplianceState::ON:      return ratedWattage; // Already accounts for PF at rating
        case ApplianceState::STANDBY: return standbyDrawWatts;
        case ApplianceState::OFF:     return 0.0f;
    }
    return 0.0f;
}

void InductiveLoad::displayInfo() const {
    std::string stateStr;
    switch (state) {
        case ApplianceState::ON:      stateStr = "ON";      break;
        case ApplianceState::OFF:     stateStr = "OFF";     break;
        case ApplianceState::STANDBY: stateStr = "STANDBY"; break;
    }

    // Visual PF warning — but exact PF value only shown by Auditor
    std::string pfStatus = "[OK]";
    // Note: We show a generic status here intentionally
    // The Auditor (friend class) is the only one that reads exact PF
    std::cout << std::left
              << std::setw(22) << name
              << " | " << std::setw(10) << inductiveType
              << " | State: " << std::setw(8) << stateStr
              << " | Load: " << std::setw(8) << calculateLoad() << "W"
              << " | PF: Run Audit to inspect"
              << "\n";
}

std::string InductiveLoad::getType() const { return inductiveType; }