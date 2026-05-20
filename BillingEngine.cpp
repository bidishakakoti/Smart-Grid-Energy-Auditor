// src/BillingEngine.cpp
#include "BillingEngine.h"
#include "ApplianceState.h"
#include <iostream>
#include <iomanip>

// ─────────────────────────────────────────
//  Constructor / Destructor
// ─────────────────────────────────────────
BillingEngine::BillingEngine(const std::string& consumerName)
    : consumerName(consumerName),
      totalUnitsConsumed(0.0f),
      totalBillAmount(0.0f) {

    std::cout << "\n [BillingEngine] Initialized for consumer: "
              << consumerName << "\n";
    std::cout << " Slab Rates: "
              << "0-100u @ Rs." << RATE_SLAB_1
              << " | 101-200u @ Rs." << RATE_SLAB_2
              << " | 201-300u @ Rs." << RATE_SLAB_3
              << " | 300u+ @ Rs."   << RATE_SLAB_4 << "\n";
}

BillingEngine::~BillingEngine() {
    std::cout << "\n [BillingEngine] Closing. "
              << "Total billed this session: Rs."
              << std::fixed << std::setprecision(2)
              << totalBillAmount << "\n";
}

// ─────────────────────────────────────────
//  Private: tiered slab calculation
// ─────────────────────────────────────────
float BillingEngine::applySlabRates(float units) const {
    float bill = 0.0f;

    if (units <= 100.0f) {
        bill = units * RATE_SLAB_1;

    } else if (units <= 200.0f) {
        bill  = 100.0f * RATE_SLAB_1;
        bill += (units - 100.0f) * RATE_SLAB_2;

    } else if (units <= 300.0f) {
        bill  = 100.0f * RATE_SLAB_1;
        bill += 100.0f * RATE_SLAB_2;
        bill += (units - 200.0f) * RATE_SLAB_3;

    } else {
        bill  = 100.0f * RATE_SLAB_1;
        bill += 100.0f * RATE_SLAB_2;
        bill += 100.0f * RATE_SLAB_3;
        bill += (units - 300.0f) * RATE_SLAB_4;
    }

    return bill;
}

// ─────────────────────────────────────────
//  Overload 1: Standard bill from units
// ─────────────────────────────────────────
float BillingEngine::calculateBill(float units) {
    float bill = applySlabRates(units);
    totalUnitsConsumed += units;
    totalBillAmount    += bill;
    return bill;
}

// ─────────────────────────────────────────
//  Overload 2: Peak hour bill
// ─────────────────────────────────────────
float BillingEngine::calculateBill(float units, bool isPeakHour) {
    float bill = applySlabRates(units);
    if (isPeakHour) {
        bill *= PEAK_SURCHARGE; // 25% extra during peak hours
    }
    totalUnitsConsumed += units;
    totalBillAmount    += bill;
    return bill;
}

// ─────────────────────────────────────────
//  Overload 3: Calculate from live grid state
// ─────────────────────────────────────────
float BillingEngine::calculateBill(const GridController& grid,
                                   float hoursUsed) {
    float totalWatts = 0.0f;

    for (const auto& app : grid.getAppliances()) {
        totalWatts += app->calculateLoad(); // Polymorphic
    }

    // Convert W to kWh
    float units = (totalWatts * hoursUsed) / 1000.0f;
    return calculateBill(units); // Calls overload 1
}

// ─────────────────────────────────────────
//  Bill breakdown display
// ─────────────────────────────────────────
void BillingEngine::showBillBreakdown(float units,
                                      bool isPeakHour) const {
    float baseBill = applySlabRates(units);
    float finalBill = isPeakHour ? baseBill * PEAK_SURCHARGE : baseBill;

    std::cout << "\n";
    std::cout << " ╔══════════════════════════════════════════════════╗\n";
    std::cout << " ║           💰  ELECTRICITY BILL BREAKDOWN         ║\n";
    std::cout << " ╚══════════════════════════════════════════════════╝\n";
    std::cout << " Consumer    : " << consumerName    << "\n";
    std::cout << " Units Used  : " << std::fixed
              << std::setprecision(2) << units << " kWh\n";
    std::cout << "\n";
    std::cout << " ─── Slab Breakdown ───────────────────────────────\n";

    float remaining = units;
    float slabBill  = 0.0f;

    // Slab 1
    float slab1 = std::min(remaining, 100.0f);
    slabBill    = slab1 * RATE_SLAB_1;
    std::cout << " 0   - 100 units : " << std::setw(6) << slab1
              << " kWh x Rs." << RATE_SLAB_1
              << " = Rs." << std::setw(8) << slabBill << "\n";
    remaining -= slab1;

    if (remaining > 0) {
        float slab2 = std::min(remaining, 100.0f);
        slabBill    = slab2 * RATE_SLAB_2;
        std::cout << " 101 - 200 units : " << std::setw(6) << slab2
                  << " kWh x Rs." << RATE_SLAB_2
                  << " = Rs." << std::setw(8) << slabBill << "\n";
        remaining -= slab2;
    }

    if (remaining > 0) {
        float slab3 = std::min(remaining, 100.0f);
        slabBill    = slab3 * RATE_SLAB_3;
        std::cout << " 201 - 300 units : " << std::setw(6) << slab3
                  << " kWh x Rs." << RATE_SLAB_3
                  << " = Rs." << std::setw(8) << slabBill << "\n";
        remaining -= slab3;
    }

    if (remaining > 0) {
        slabBill = remaining * RATE_SLAB_4;
        std::cout << " 300+     units  : " << std::setw(6) << remaining
                  << " kWh x Rs." << RATE_SLAB_4
                  << " = Rs." << std::setw(8) << slabBill << "\n";
    }

    std::cout << " ───────────────────────────────────────────────────\n";
    std::cout << " Base Bill   : Rs." << std::setw(10)
              << std::fixed << std::setprecision(2) << baseBill << "\n";

    if (isPeakHour) {
        float surcharge = finalBill - baseBill;
        std::cout << " Peak Surcharge (25%) : Rs." << std::setw(8)
                  << surcharge << "\n";
    }

    std::cout << " ───────────────────────────────────────────────────\n";
    std::cout << " TOTAL DUE   : Rs." << std::setw(10)
              << finalBill << "\n";
    std::cout << " ═══════════════════════════════════════════════════\n\n";
}

// ─────────────────────────────────────────
//  Session total
// ─────────────────────────────────────────
void BillingEngine::showSessionTotal() const {
    std::cout << "\n [Session Billing Summary]\n";
    std::cout << " ─────────────────────────────\n";
    std::cout << " Consumer         : " << consumerName       << "\n";
    std::cout << " Total Units      : " << std::fixed
              << std::setprecision(2)
              << totalUnitsConsumed << " kWh\n";
    std::cout << " Total Billed     : Rs."
              << totalBillAmount    << "\n";
    std::cout << " ─────────────────────────────\n";
}