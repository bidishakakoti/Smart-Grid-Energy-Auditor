// src/ResistiveLoad.cpp
#include "ResistiveLoad.h"
#include <iostream>
#include <iomanip>

ResistiveLoad::ResistiveLoad(const std::string& name,
                             float ratedWattage,
                             float voltage,
                             float current,
                             const std::string& resistiveType)
    : Appliance(name, ratedWattage, voltage, current, 1.0f),
      resistiveType(resistiveType) {}

float ResistiveLoad::calculateLoad() const {
    // Resistive loads consume full rated wattage when ON
    // Reduced consumption in STANDBY, zero when OFF
    switch (state) {
        case ApplianceState::ON:      return ratedWattage;
        case ApplianceState::STANDBY: return ratedWattage * 0.1f; // 10% standby draw
        case ApplianceState::OFF:     return 0.0f;
    }
    return 0.0f;
}

void ResistiveLoad::displayInfo() const {
    std::string stateStr;
    switch (state) {
        case ApplianceState::ON:      stateStr = "ON";      break;
        case ApplianceState::OFF:     stateStr = "OFF";     break;
        case ApplianceState::STANDBY: stateStr = "STANDBY"; break;
    }

    std::cout << std::left
              << std::setw(22) << name
              << " | " << std::setw(10) << resistiveType
              << " | State: " << std::setw(8) << stateStr
              << " | Load: " << std::setw(8) << calculateLoad() << "W"
              << " | PF: 1.00 (Resistive)"
              << "\n";
}

std::string ResistiveLoad::getType() const { return resistiveType; }