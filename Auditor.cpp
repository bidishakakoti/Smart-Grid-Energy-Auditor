// src/Auditor.cpp
#include "Auditor.h"
#include "InductiveLoad.h"
#include "ApplianceState.h"
#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <chrono>
#include <ctime>

// ─────────────────────────────────────────
//  Constructor / Destructor
// ─────────────────────────────────────────
Auditor::Auditor(const std::string& auditorName)
    : auditorName(auditorName),
      totalAuditsRun(0),
      appliancesFlagged(0) {

    std::cout << "\n [Auditor] \"" << auditorName
              << "\" initialized. Ready to inspect.\n";
}

Auditor::~Auditor() {
    std::cout << "\n [Auditor] Session closed. Total audits run: "
              << totalAuditsRun << "\n";
}

// ─────────────────────────────────────────
//  Core Audit — Friend access to private PF
// ─────────────────────────────────────────
void Auditor::runAudit(const GridController& grid) {
    totalAuditsRun++;
    appliancesFlagged = 0;

    std::cout << "\n";
    std::cout << " ╔══════════════════════════════════════════════════╗\n";
    std::cout << " ║         🔍  EFFICIENCY AUDIT REPORT              ║\n";
    std::cout << " ╚══════════════════════════════════════════════════╝\n";
    std::cout << " Auditor  : " << auditorName << "\n";
    std::cout << " PF Limit : " << PF_THRESHOLD
              << " (below this = inefficient)\n";
    std::cout << "\n";
    std::cout << " ─────────────────────────────────────────────────────\n";
    std::cout << std::left
              << " " << std::setw(22) << "Appliance"
              << std::setw(10) << "PF"
              << std::setw(12) << "Status"
              << "Recommendation\n";
    std::cout << " ─────────────────────────────────────────────────────\n";

    // ── KEY MOMENT: Friend access to private powerFactor ──
    for (const auto& appPtr : grid.appliances) {
        Appliance* app = appPtr.get();

        // Directly accessing private member — only possible because
        // Auditor is declared friend in Appliance
        float pf = app->powerFactor;

        std::string status;
        std::string recommendation;

        if (pf < PF_THRESHOLD) {
            status = "⚠ POOR";
            recommendation = "Apply Capacitor Bank Correction";
            appliancesFlagged++;
        } else if (pf < 0.95f) {
            status = "~ FAIR";
            recommendation = "Monitor — marginal efficiency";
        } else {
            status = "✓ GOOD";
            recommendation = "No action needed";
        }

        std::cout << " " << std::left
                  << std::setw(22) << app->name         // private access
                  << std::setw(10) << std::fixed
                  << std::setprecision(2) << pf
                  << std::setw(12) << status
                  << recommendation << "\n";
    }

    std::cout << " ─────────────────────────────────────────────────────\n";
    std::cout << " Appliances flagged : " << appliancesFlagged << "\n";

    if (appliancesFlagged > 0) {
        std::cout << "\n [!] " << appliancesFlagged
                  << " appliance(s) have poor power factor.\n";
        std::cout << "     Capacitor correction can reduce energy waste"
                  << " by up to 20%%.\n";
    } else {
        std::cout << "\n [✓] All appliances within efficient PF range.\n";
    }

    std::cout << "\n";

    // Save to file
    std::string report = buildReport(grid);
    writeReportToFile(report);
    std::cout << " [Auditor] Report saved to data/audit_logs.txt\n\n";
}

// ─────────────────────────────────────────
//  Build report string for file output
// ─────────────────────────────────────────
std::string Auditor::buildReport(const GridController& grid) const {
    std::ostringstream oss;

    // Timestamp
    auto now = std::chrono::system_clock::now();
    std::time_t t = std::chrono::system_clock::to_time_t(now);
    std::string timeStr = std::ctime(&t);

    oss << "══════════════════════════════════════\n";
    oss << " AUDIT REPORT — " << auditorName << "\n";
    oss << " Date : " << timeStr;
    oss << " PF Threshold : " << PF_THRESHOLD << "\n";
    oss << "──────────────────────────────────────\n";
    oss << std::left
        << std::setw(22) << "Appliance"
        << std::setw(8)  << "PF"
        << std::setw(12) << "Status"
        << "Action\n";
    oss << "──────────────────────────────────────\n";

    for (const auto& appPtr : grid.appliances) {
        Appliance* app = appPtr.get();
        float pf = app->powerFactor; // Friend access

        std::string status      = (pf < PF_THRESHOLD) ? "POOR"
                                : (pf < 0.95f)        ? "FAIR"
                                :                       "GOOD";
        std::string action      = (pf < PF_THRESHOLD)
                                ? "Capacitor Correction Required"
                                : (pf < 0.95f)
                                ? "Monitor"
                                : "None";

        oss << std::left
            << std::setw(22) << app->name
            << std::setw(8)  << std::fixed
            << std::setprecision(2) << pf
            << std::setw(12) << status
            << action << "\n";
    }

    oss << "──────────────────────────────────────\n";
    oss << " Total Flagged : " << appliancesFlagged << "\n";
    oss << "══════════════════════════════════════\n\n";

    return oss.str();
}

// ─────────────────────────────────────────
//  Write report string to file
// ─────────────────────────────────────────
void Auditor::writeReportToFile(const std::string& report) const {
    std::ofstream file("data/audit_logs.txt", std::ios::app);
    if (!file.is_open()) {
        std::cerr << " [ERROR] Could not open data/audit_logs.txt\n";
        return;
    }
    file << report;
    file.close();
}

// ─────────────────────────────────────────
//  Session summary
// ─────────────────────────────────────────
void Auditor::showAuditSummary() const {
    std::cout << "\n [Auditor Summary]\n";
    std::cout << " ─────────────────────────────\n";
    std::cout << " Auditor       : " << auditorName    << "\n";
    std::cout << " Total Audits  : " << totalAuditsRun << "\n";
    std::cout << " Last Flagged  : " << appliancesFlagged << " appliance(s)\n";
    std::cout << " ─────────────────────────────\n";
}