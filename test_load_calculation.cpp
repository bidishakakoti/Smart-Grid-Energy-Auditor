// Tests: Does calculateLoad() return correct values?
#include "../include/ResistiveLoad.h"
#include "../include/InductiveLoad.h"
#include "../include/ApplianceState.h"
#include <iostream>
#include <cassert>

int main() {
    // ResistiveLoad — ON should return full wattage
    ResistiveLoad heater("Test Heater", 1500.0f, 230.0f, 6.5f, "Heater");
    heater.setState(ApplianceState::ON);
    assert(heater.calculateLoad() == 1500.0f);
    std::cout << "[PASS] ResistiveLoad ON = 1500W\n";

    // ResistiveLoad — OFF should return 0
    heater.setState(ApplianceState::OFF);
    assert(heater.calculateLoad() == 0.0f);
    std::cout << "[PASS] ResistiveLoad OFF = 0W\n";

    // ResistiveLoad — STANDBY should return 10%
    heater.setState(ApplianceState::STANDBY);
    assert(heater.calculateLoad() == 150.0f);
    std::cout << "[PASS] ResistiveLoad STANDBY = 150W\n";

    // InductiveLoad — OFF should return 0
    InductiveLoad ac("Test AC", 1800.0f, 230.0f, 9.5f, 0.78f, "AC", 8.0f);
    ac.setState(ApplianceState::OFF);
    assert(ac.calculateLoad() == 0.0f);
    std::cout << "[PASS] InductiveLoad OFF = 0W\n";

    std::cout << "\n All load calculation tests passed!\n";
    return 0;
}