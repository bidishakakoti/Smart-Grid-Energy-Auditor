// include/GridController.h
#pragma once
#include "Appliance.h"
#include <vector>
#include <memory>
#include <string>

class GridController {
private:
    std::string gridName;
    float maxCapacityWatts;         // Safety limit (default 5000W)
    float currentTariffPerUnit;     // Rs. per kWh — set at construction
    std::vector<std::unique_ptr<Appliance>> appliances; // Polymorphic storage

    // Private helpers
    float getTotalLoad() const;
    void saveSessionToFile() const; // Called only by destructor

public:
    // Constructor loads tariff rate at startup
    GridController(const std::string& gridName,
                   float maxCapacityWatts = 5000.0f,
                   float tariffPerUnit = 6.50f);

    // Destructor — RAII: auto-saves session summary on exit
    ~GridController();

    // Appliance management
    void addAppliance(std::unique_ptr<Appliance> appliance);
    void toggleApplianceState(const std::string& name, ApplianceState newState);

    // Core features
    void showDashboard() const;
    void runSafetyCheck() const;

    // Getters
    float getMaxCapacity() const;
    const std::vector<std::unique_ptr<Appliance>>& getAppliances() const;

    // Friend — Auditor needs access to appliances list
    friend class Auditor;
};