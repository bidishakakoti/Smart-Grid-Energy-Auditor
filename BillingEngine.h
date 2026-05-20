// include/BillingEngine.h
#pragma once
#include "GridController.h"
#include <string>

// Tiered slab rates (Indian electricity billing model)
constexpr float RATE_SLAB_1 = 3.50f;   // Rs/kWh → 0   to 100 units
constexpr float RATE_SLAB_2 = 5.00f;   // Rs/kWh → 101 to 200 units
constexpr float RATE_SLAB_3 = 6.50f;   // Rs/kWh → 201 to 300 units
constexpr float RATE_SLAB_4 = 8.00f;   // Rs/kWh → 300+ units
constexpr float PEAK_SURCHARGE = 1.25f; // 25% surcharge during peak hours

class BillingEngine {
private:
    std::string consumerName;
    float totalUnitsConsumed;   // kWh accumulated this session
    float totalBillAmount;      // Rs. accumulated this session

    // Private helper — slab calculation logic
    float applySlabRates(float units) const;

public:
    BillingEngine(const std::string& consumerName);
    ~BillingEngine();

    // ── FUNCTION OVERLOADING ──────────────────────────
    // Version 1: Standard bill — just units consumed
    float calculateBill(float units);

    // Version 2: Peak hour bill — applies surcharge
    float calculateBill(float units, bool isPeakHour);

    // Version 3: Duration based — calculates units from grid state
    float calculateBill(const GridController& grid, float hoursUsed);
    // ─────────────────────────────────────────────────

    void showBillBreakdown(float units, bool isPeakHour = false) const;
    void showSessionTotal() const;
};