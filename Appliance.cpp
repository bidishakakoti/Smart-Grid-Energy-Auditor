// src/Appliance.cpp
#include "Appliance.h"
#include <iostream>
#include <iomanip>

Appliance::Appliance(const std::string& name, float ratedWattage,
                     float voltage, float current, float powerFactor)
    : name(name), ratedWattage(ratedWattage),
      voltage(voltage), current(current),
      powerFactor(powerFactor), state(ApplianceState::OFF) {}

void Appliance::displayInfo() const {
    std::string stateStr;
    switch(state) {
        case ApplianceState::ON:      stateStr = "ON";      break;
        case ApplianceState::OFF:     stateStr = "OFF";     break;
        case ApplianceState::STANDBY: stateStr = "STANDBY"; break;
    }
    std::cout << std::left << std::setw(20) << name
              << " | State: " << std::setw(8) << stateStr
              << " | Load: " << std::setw(8) << calculateLoad() << "W"
              << "\n";
}

float Appliance::operator+(const Appliance& other) const {
    return this->calculateLoad() + other.calculateLoad();
}

void Appliance::setState(ApplianceState newState) { state = newState; }
ApplianceState Appliance::getState() const { return state; }
std::string Appliance::getName() const { return name; }
float Appliance::getRatedWattage() const { return ratedWattage; }