#pragma once
#include <QMainWindow>
#include <QLabel>
#include <QProgressBar>
#include <QTableWidget>
#include <QTextEdit>
#include <QTimer>
#include "GridController.h"
#include "BillingEngine.h"
#include "Auditor.h"

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

private:
    GridController* grid;
    BillingEngine*  billing;
    Auditor*        auditor;

    QLabel*       totalLoadLabel;
    QLabel*       safetyStatusLabel;
    QProgressBar* loadBar;
    QTableWidget* applianceTable;
    QLabel*       billResultLabel;
    QTextEdit*    auditLogDisplay;
    QLabel*       alertBannerLabel;
    QLabel*       ledIndicatorLabel;
    QTimer*       ledBlinkTimer;
    bool          ledVisible;

    float monthlyUnitsUsed;
    float monthlyLimit;

    void setupUI();
    void setupAppliances();
    void updateDashboard();
    void updateLED(float loadPercent);
    void checkMonthlyAlert();
    void refreshApplianceTable();
};