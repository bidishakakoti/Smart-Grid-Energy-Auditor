// src/GridController.cpp
#include "GridController.h"
#include "ApplianceState.h"
#include <iostream>
#include <iomanip>
#include <fstream>
#include <chrono>
#include <ctime>
#include <algorithm>

// ─────────────────────────────────────────
//  Constructor
// ─────────────────────────────────────────
GridController::GridController(const std::string& gridName,
                               float maxCapacityWatts,
                               float tariffPerUnit)
    : gridName(gridName),
      maxCapacityWatts(maxCapacityWatts),
      currentTariffPerUnit(tariffPerUnit) {

    std::cout << "\n [GridController] Initializing \"" << gridName << "\"\n";
    std::cout << " Max Capacity : " << maxCapacityWatts << "W\n";
    std::cout << " Tariff Rate  : Rs. " << tariffPerUnit << " / kWh\n";
    std::cout << " ─────────────────────────────────────────\n";
}

// ─────────────────────────────────────────
//  Destructor — RAII Session Save
// ─────────────────────────────────────────
GridController::~GridController() {
    std::cout << "\n [GridController] Session ending. Saving summary...\n";
    saveSessionToFile();
    std::cout << " Summary saved to data/session.txt\n";
}

// ─────────────────────────────────────────
//  Private: sum all active loads
// ─────────────────────────────────────────
float GridController::getTotalLoad() const {
    float total = 0.0f;
    for (const auto& appliance : appliances) {
        total += appliance->calculateLoad(); // Polymorphic dispatch
    }
    return total;
}

// ─────────────────────────────────────────
//  Private: save session summary to file
// ─────────────────────────────────────────
void GridController::saveSessionToFile() const {
    std::ofstream file("data/session.txt", std::ios::app);
    if (!file.is_open()) {
        std::cerr << " [ERROR] Could not open data/session.txt\n";
        return;
    }

    // Timestamp
    auto now = std::chrono::system_clock::now();
    std::time_t t = std::chrono::system_clock::to_time_t(now);
    std::string timeStr = std::ctime(&t);
    // For Linux/Mac use: std::string timeStr = std::ctime(&t);

    file << "══════════════════════════════════════\n";
    file << " SESSION SUMMARY — " << gridName << "\n";
    file << " Saved at: " << timeStr;
    file << "──────────────────────────────────────\n";
    file << std::left << std::setw(22) << "Appliance"
         << std::setw(12) << "State"
         << std::setw(12) << "Load (W)" << "\n";
    file << "──────────────────────────────────────\n";

    for (const auto& appliance : appliances) {
        std::string stateStr;
        switch (appliance->getState()) {
            case ApplianceState::ON:      stateStr = "ON";      break;
            case ApplianceState::OFF:     stateStr = "OFF";     break;
            case ApplianceState::STANDBY: stateStr = "STANDBY"; break;
        }
        file << std::left
             << std::setw(22) << appliance->getName()
             << std::setw(12) << stateStr
             << std::setw(12) << appliance->calculateLoad() << "\n";
    }

    file << "──────────────────────────────────────\n";
    file << " Total Load : " << getTotalLoad() << "W\n";
    file << " Capacity   : " << maxCapacityWatts << "W\n";
    file << " Tariff     : Rs. " << currentTariffPerUnit << "/kWh\n";
    file << "══════════════════════════════════════\n\n";
    file.close();
}

// ─────────────────────────────────────────
//  Add appliance to grid
// ─────────────────────────────────────────
void GridController::addAppliance(std::unique_ptr<Appliance> appliance) {
    std::cout << " [+] Added: " << appliance->getName() << "\n";
    appliances.push_back(std::move(appliance));
}

// ─────────────────────────────────────────
//  Toggle appliance state by name
// ─────────────────────────────────────────
void GridController::toggleApplianceState(const std::string& name,
                                          ApplianceState newState) {
    for (auto& appliance : appliances) {
        if (appliance->getName() == name) {
            appliance->setState(newState);
            std::string stateStr;
            switch (newState) {
                case ApplianceState::ON:      stateStr = "ON";      break;
                case ApplianceState::OFF:     stateStr = "OFF";     break;
                case ApplianceState::STANDBY: stateStr = "STANDBY"; break;
            }
            std::cout << " [~] " << name << " set to " << stateStr << "\n";
            return;
        }
    }
    std::cout << " [!] Appliance \"" << name << "\" not found.\n";
}

// ─────────────────────────────────────────
//  Live Dashboard
// ─────────────────────────────────────────
void GridController::showDashboard() const {
    float totalLoad = getTotalLoad();
    float percent   = (totalLoad / maxCapacityWatts) * 100.0f;

    // Progress bar
    int barFilled = static_cast<int>(percent / 5); // 20 blocks = 100%
    barFilled = std::min(barFilled, 20);
    std::string bar = "";
    for (int i = 0; i < 20; i++)
        bar += (i < barFilled) ? "█" : "░";

    std::cout << "\n";
    std::cout << " ╔══════════════════════════════════════════════════╗\n";
    std::cout << " ║       ⚡  SMART GRID ENERGY AUDITOR  ⚡           ║\n";
    std::cout << " ╚══════════════════════════════════════════════════╝\n";
    std::cout << "\n GRID  : " << gridName << "\n";
    std::cout << " LOAD  : " << totalLoad << "W / "
              << maxCapacityWatts << "W"
              << "  [" << bar << "] "
              << std::fixed << std::setprecision(1) << percent << "%\n";
    std::cout << "\n";
    std::cout << " ─────────────────────────────────────────────────────\n";
    std::cout << std::left
              << " " << std::setw(22) << "Appliance"
              << std::setw(12) << "Type"
              << std::setw(10) << "State"
              << std::setw(10) << "Load"
              << "\n";
    std::cout << " ─────────────────────────────────────────────────────\n";

    for (const auto& appliance : appliances) {
        std::cout << " ";
        appliance->displayInfo(); // Polymorphic call
    }

    std::cout << " ─────────────────────────────────────────────────────\n";

    // Safety status line
    runSafetyCheck();
    std::cout << "\n";
}

// ─────────────────────────────────────────
//  Safety Check
// ─────────────────────────────────────────
void GridController::runSafetyCheck() const {
    float totalLoad = getTotalLoad();
    float percent   = (totalLoad / maxCapacityWatts) * 100.0f;

    if (totalLoad >= maxCapacityWatts) {
        std::cout << "\n [🔴 CRITICAL] Load EXCEEDED capacity! "
                  << "Risk of tripped breaker!\n";
    } else if (percent >= 85.0f) {
        std::cout << "\n [🟡 WARNING ] Load at "
                  << std::fixed << std::setprecision(1) << percent
                  << "% — approaching limit. Consider turning off an appliance.\n";
    } else {
        std::cout << "\n [🟢 SAFE    ] Load within safe limits ("
                  << std::fixed << std::setprecision(1) << percent << "%).\n";
    }
}

// ─────────────────────────────────────────
//  Getters
// ─────────────────────────────────────────
float GridController::getMaxCapacity() const { return maxCapacityWatts; }

const std::vector<std::unique_ptr<Appliance>>&
GridController::getAppliances() const { return appliances; }