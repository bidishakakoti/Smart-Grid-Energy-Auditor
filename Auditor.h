// include/Auditor.h
#pragma once
#include "GridController.h"
#include <string>

// Threshold below which a power factor is considered inefficient
constexpr float PF_THRESHOLD = 0.85f;

class Auditor {
private:
    std::string auditorName;
    int totalAuditsRun;
    int appliancesFlagged;      // Count of poor PF appliances found

    // Private helpers
    void writeReportToFile(const std::string& report) const;
    std::string buildReport(const GridController& grid) const;

public:
    Auditor(const std::string& auditorName);
    ~Auditor();

    // Core audit — inspects private PF via friend access
    void runAudit(const GridController& grid);

    // Summary of all audits this session
    void showAuditSummary() const;
};