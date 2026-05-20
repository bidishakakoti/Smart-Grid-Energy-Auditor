// Tests: Does the + operator correctly sum two appliance loads?
#include "../include/ResistiveLoad.h"
#include "../include/ApplianceState.h"
#include <iostream>
#include <cassert>

int main() {
    ResistiveLoad heater("Heater", 1500.0f, 230.0f, 6.5f, "Heater");
    ResistiveLoad bulb("Bulb",   120.0f,  230.0f, 0.5f, "Bulb");

    heater.setState(ApplianceState::ON);
    bulb.setState(ApplianceState::ON);

    float combined = heater + bulb;
    assert(combined == 1620.0f);
    std::cout << "[PASS] 1500W + 120W = " << combined << "W\n";

    // OFF appliance should contribute 0
    bulb.setState(ApplianceState::OFF);
    combined = heater + bulb;
    assert(combined == 1500.0f);
    std::cout << "[PASS] 1500W + 0W   = " << combined << "W\n";

    std::cout << "\n All operator overload tests passed!\n";
    return 0;
}