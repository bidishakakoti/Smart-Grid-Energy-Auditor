#include "mainwindow.h"
#include "InductiveLoad.h"
#include "ResistiveLoad.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QPushButton>
#include <QLineEdit>
#include <QComboBox>
#include <QTabWidget>
#include <QHeaderView>
#include <QMessageBox>
#include <QSpinBox>
#include <QCheckBox>
#include <QDoubleSpinBox>
#include <QDialog>
#include <QDialogButtonBox>
#include <QFormLayout>
#include <QTableWidget>
#include <QLabel>
#include <QProgressBar>
#include <QTextEdit>
#include <QTimer>
#include <QFont>
#include <QPalette>
#include <iostream>
#include <sstream>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent),
    monthlyUnitsUsed(184.0f),
    monthlyLimit(220.0f),
    ledVisible(true)
{
    grid    = new GridController("Home Grid — Block A", 5000.0f, 6.50f);
    billing = new BillingEngine("Resident: Flat 4B");
    auditor = new Auditor("GridAudit Pro v1.0");

    setupAppliances();
    setupUI();
    updateDashboard();

    // Show popup notification on startup if needed
    checkMonthlyAlert();
}

MainWindow::~MainWindow() {
    delete grid;
    delete billing;
    delete auditor;
}

void MainWindow::setupAppliances() {
    grid->addAppliance(std::make_unique<ResistiveLoad>(
        "Water Heater", 1500.0f, 230.0f, 6.52f, "Heater"));
    grid->addAppliance(std::make_unique<ResistiveLoad>(
        "LED Ceiling Light", 120.0f, 230.0f, 0.52f, "Bulb"));
    grid->addAppliance(std::make_unique<ResistiveLoad>(
        "Electric Iron", 1000.0f, 230.0f, 4.35f, "Iron"));
    grid->addAppliance(std::make_unique<InductiveLoad>(
        "Samsung 1.5T AC", 1800.0f, 230.0f, 9.5f, 0.78f, "AC", 8.0f));
    grid->addAppliance(std::make_unique<InductiveLoad>(
        "Refrigerator", 200.0f, 230.0f, 1.05f, 0.92f, "Motor", 15.0f));
    grid->addAppliance(std::make_unique<InductiveLoad>(
        "Ceiling Fan", 75.0f, 230.0f, 0.40f, 0.80f, "Fan", 2.0f));

    grid->toggleApplianceState("Samsung 1.5T AC",  ApplianceState::ON);
    grid->toggleApplianceState("Water Heater",      ApplianceState::ON);
    grid->toggleApplianceState("LED Ceiling Light", ApplianceState::ON);
    grid->toggleApplianceState("Refrigerator",      ApplianceState::STANDBY);
    grid->toggleApplianceState("Ceiling Fan",       ApplianceState::ON);
}

void MainWindow::setupUI() {
    setWindowTitle("Smart Grid Energy Auditor v1.0");
    setMinimumSize(1000, 700);

    // Dark theme for whole app
    setStyleSheet(
        "QMainWindow { background-color: #1e1e2e; }"
        "QTabWidget::pane { border: 1px solid #44475a; background: #1e1e2e; }"
        "QTabBar::tab { background: #282a36; color: #cdd6f4; padding: 8px 20px; "
        "               border: 1px solid #44475a; margin-right: 2px; }"
        "QTabBar::tab:selected { background: #89b4fa; color: #1e1e2e; font-weight: bold; }"
        "QGroupBox { color: #cdd6f4; border: 1px solid #44475a; border-radius: 6px; "
        "            margin-top: 10px; padding-top: 10px; font-weight: bold; }"
        "QGroupBox::title { subcontrol-origin: margin; left: 10px; color: #89b4fa; }"
        "QLabel { color: #cdd6f4; }"
        "QTableWidget { background: #282a36; color: #cdd6f4; "
        "               gridline-color: #44475a; border: 1px solid #44475a; }"
        "QTableWidget::item { color: #cdd6f4; padding: 4px; }"
        "QTableWidget::item:selected { background: #44475a; color: #ffffff; }"
        "QHeaderView::section { background: #313244; color: #89b4fa; "
        "                       border: 1px solid #44475a; padding: 6px; font-weight: bold; }"
        "QPushButton { background: #313244; color: #cdd6f4; border: 1px solid #44475a; "
        "              border-radius: 4px; padding: 6px 14px; }"
        "QPushButton:hover { background: #44475a; }"
        "QPushButton:pressed { background: #89b4fa; color: #1e1e2e; }"
        "QComboBox { background: #313244; color: #cdd6f4; border: 1px solid #44475a; "
        "            border-radius: 4px; padding: 4px; }"
        "QComboBox QAbstractItemView { background: #313244; color: #cdd6f4; }"
        "QSpinBox, QDoubleSpinBox, QLineEdit { background: #313244; color: #cdd6f4; "
        "                                      border: 1px solid #44475a; border-radius: 4px; padding: 4px; }"
        "QTextEdit { background: #282a36; color: #cdd6f4; border: 1px solid #44475a; }"
        "QCheckBox { color: #cdd6f4; }"
        "QProgressBar { border: 1px solid #44475a; border-radius: 4px; "
        "               background: #313244; color: #ffffff; text-align: center; }"
        );

    QWidget* central = new QWidget(this);
    setCentralWidget(central);
    QVBoxLayout* rootLayout = new QVBoxLayout(central);
    rootLayout->setSpacing(8);
    rootLayout->setContentsMargins(10, 10, 10, 10);

    // ── Alert Banner ──────────────────────────────────
    alertBannerLabel = new QLabel();
    alertBannerLabel->setAlignment(Qt::AlignCenter);
    alertBannerLabel->setStyleSheet(
        "background:#f59e0b; color:#1e1e2e; padding:10px;"
        "border-radius:6px; font-weight:bold; font-size:13px;");
    alertBannerLabel->hide();
    rootLayout->addWidget(alertBannerLabel);

    // ── Tab Widget ────────────────────────────────────
    QTabWidget* tabs = new QTabWidget();
    rootLayout->addWidget(tabs);

    // ════════════════════════════════
    //  TAB 1 — DASHBOARD
    // ════════════════════════════════
    QWidget* dashTab = new QWidget();
    dashTab->setStyleSheet("background: #1e1e2e;");
    QVBoxLayout* dashLayout = new QVBoxLayout(dashTab);
    dashLayout->setSpacing(12);

    // Top stats row
    QGroupBox* statsBox = new QGroupBox("Live Grid Status — Home Grid, Block A");
    QGridLayout* statsGrid = new QGridLayout(statsBox);
    statsGrid->setSpacing(12);

    totalLoadLabel = new QLabel("-- W");
    totalLoadLabel->setStyleSheet(
        "font-size:32px; font-weight:bold; color:#a6e3a1;");

    safetyStatusLabel = new QLabel("Safe");
    safetyStatusLabel->setStyleSheet(
        "font-size:16px; color:#a6e3a1; font-weight:bold;");

    ledIndicatorLabel = new QLabel("●");
    ledIndicatorLabel->setStyleSheet("font-size:28px; color:#a6e3a1;");

    ledBlinkTimer = new QTimer(this);
    connect(ledBlinkTimer, &QTimer::timeout, this, [this](){
        ledVisible = !ledVisible;
        ledIndicatorLabel->setVisible(ledVisible);
    });

    loadBar = new QProgressBar();
    loadBar->setRange(0, 5000);
    loadBar->setTextVisible(true);
    loadBar->setFormat("%v W / 5000 W");
    loadBar->setMinimumHeight(26);
    loadBar->setStyleSheet(
        "QProgressBar { border:1px solid #44475a; border-radius:4px; "
        "background:#313244; color:#ffffff; text-align:center; }"
        "QProgressBar::chunk { background:#a6e3a1; border-radius:4px; }");

    QLabel* loadTitleLabel    = new QLabel("Total Load:");
    QLabel* statusTitleLabel  = new QLabel("Status:");
    QLabel* ledTitleLabel     = new QLabel("LED Indicator:");
    QLabel* loadBarTitleLabel = new QLabel("Load Bar:");
    loadTitleLabel->setStyleSheet("color:#89b4fa; font-weight:bold;");
    statusTitleLabel->setStyleSheet("color:#89b4fa; font-weight:bold;");
    ledTitleLabel->setStyleSheet("color:#89b4fa; font-weight:bold;");
    loadBarTitleLabel->setStyleSheet("color:#89b4fa; font-weight:bold;");

    statsGrid->addWidget(loadTitleLabel,    0, 0);
    statsGrid->addWidget(totalLoadLabel,    0, 1);
    statsGrid->addWidget(statusTitleLabel,  1, 0);
    statsGrid->addWidget(safetyStatusLabel, 1, 1);
    statsGrid->addWidget(ledTitleLabel,     2, 0);
    statsGrid->addWidget(ledIndicatorLabel, 2, 1);
    statsGrid->addWidget(loadBarTitleLabel, 3, 0);
    statsGrid->addWidget(loadBar,           3, 1);

    dashLayout->addWidget(statsBox);

    // Appliance table
    QGroupBox* appBox = new QGroupBox("Appliances — Real Time");
    QVBoxLayout* appLayout = new QVBoxLayout(appBox);

    applianceTable = new QTableWidget(0, 5);
    applianceTable->setHorizontalHeaderLabels(
        {"Name", "Type", "Load (W)", "Power Factor", "State"});
    applianceTable->horizontalHeader()->setSectionResizeMode(
        QHeaderView::Stretch);
    applianceTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    applianceTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    applianceTable->setAlternatingRowColors(false);
    applianceTable->verticalHeader()->setStyleSheet(
        "color: #cdd6f4;");
    appLayout->addWidget(applianceTable);

    // Toggle state controls
    QHBoxLayout* toggleRow = new QHBoxLayout();
    QLabel* setStateLabel = new QLabel("Selected appliance state:");
    setStateLabel->setStyleSheet("color:#89b4fa; font-weight:bold;");
    QComboBox* stateCombo = new QComboBox();
    stateCombo->addItems({"ON", "OFF", "STANDBY"});
    stateCombo->setFixedWidth(120);
    QPushButton* toggleBtn = new QPushButton("Set State");
    toggleBtn->setStyleSheet(
        "background:#89b4fa; color:#1e1e2e; font-weight:bold; padding:6px 16px;");
    toggleRow->addWidget(setStateLabel);
    toggleRow->addWidget(stateCombo);
    toggleRow->addWidget(toggleBtn);
    toggleRow->addStretch();
    appLayout->addLayout(toggleRow);

    connect(toggleBtn, &QPushButton::clicked, this, [=](){
        int row = applianceTable->currentRow();
        if(row < 0){
            QMessageBox::warning(this, "No selection",
                                 "Please click on an appliance row first.");
            return;
        }
        QString name = applianceTable->item(row, 0)->text();
        ApplianceState s = ApplianceState::OFF;
        if(stateCombo->currentText() == "ON")      s = ApplianceState::ON;
        if(stateCombo->currentText() == "STANDBY") s = ApplianceState::STANDBY;
        grid->toggleApplianceState(name.toStdString(), s);
        updateDashboard();
    });

    dashLayout->addWidget(appBox);

    // ── Add New Appliance Form ────────────────────────
    QGroupBox* addBox = new QGroupBox("Add New Appliance");
    QHBoxLayout* addLayout = new QHBoxLayout(addBox);

    QLineEdit* nameInput = new QLineEdit();
    nameInput->setPlaceholderText("Name (e.g. Washing Machine)");
    nameInput->setFixedWidth(200);

    QDoubleSpinBox* wattInput = new QDoubleSpinBox();
    wattInput->setRange(1, 10000);
    wattInput->setValue(100);
    wattInput->setSuffix(" W");
    wattInput->setFixedWidth(100);

    QComboBox* kindCombo = new QComboBox();
    kindCombo->addItems({"Resistive", "Inductive"});
    kindCombo->setFixedWidth(110);

    QDoubleSpinBox* pfInput = new QDoubleSpinBox();
    pfInput->setRange(0.1, 1.0);
    pfInput->setSingleStep(0.01);
    pfInput->setValue(0.85);
    pfInput->setPrefix("PF: ");
    pfInput->setFixedWidth(100);

    // Show/hide PF input based on type
    pfInput->setVisible(false);
    connect(kindCombo, &QComboBox::currentTextChanged, this,
            [=](const QString& text){
                pfInput->setVisible(text == "Inductive");
            });

    QPushButton* addBtn = new QPushButton("+ Add to Grid");
    addBtn->setStyleSheet(
        "background:#a6e3a1; color:#1e1e2e; font-weight:bold; padding:6px 16px;");

    addLayout->addWidget(new QLabel("Name:"));
    addLayout->addWidget(nameInput);
    addLayout->addWidget(new QLabel("Wattage:"));
    addLayout->addWidget(wattInput);
    addLayout->addWidget(new QLabel("Type:"));
    addLayout->addWidget(kindCombo);
    addLayout->addWidget(pfInput);
    addLayout->addWidget(addBtn);
    addLayout->addStretch();

    connect(addBtn, &QPushButton::clicked, this, [=](){
        QString name = nameInput->text().trimmed();
        if(name.isEmpty()){
            QMessageBox::warning(this, "Missing name",
                                 "Please enter an appliance name.");
            return;
        }
        float watts = (float)wattInput->value();
        if(kindCombo->currentText() == "Resistive"){
            grid->addAppliance(std::make_unique<ResistiveLoad>(
                name.toStdString(), watts, 230.0f,
                watts/230.0f, "Custom"));
        } else {
            float pf = (float)pfInput->value();
            grid->addAppliance(std::make_unique<InductiveLoad>(
                name.toStdString(), watts, 230.0f,
                watts/230.0f, pf, "Custom", 5.0f));
        }
        grid->toggleApplianceState(name.toStdString(), ApplianceState::ON);
        nameInput->clear();
        updateDashboard();
        QMessageBox::information(this, "Added",
                                 name + " added to grid and set to ON.");
    });

    dashLayout->addWidget(addBox);
    tabs->addTab(dashTab, "Dashboard");

    // ════════════════════════════════
    //  TAB 2 — BILLING
    // ════════════════════════════════
    QWidget* billTab = new QWidget();
    billTab->setStyleSheet("background: #1e1e2e;");
    QVBoxLayout* billLayout = new QVBoxLayout(billTab);
    billLayout->setSpacing(12);

    // Monthly tracker
    QGroupBox* monthBox = new QGroupBox("Monthly Limit Tracker");
    QVBoxLayout* monthLayout = new QVBoxLayout(monthBox);

    int pct = (int)((monthlyUnitsUsed / monthlyLimit) * 100);
    QProgressBar* monthBar = new QProgressBar();
    monthBar->setRange(0, 100);
    monthBar->setValue(pct);
    monthBar->setMinimumHeight(26);
    monthBar->setFormat(
        QString("%1 / %2 kWh used (%3%)")
            .arg(monthlyUnitsUsed).arg(monthlyLimit).arg(pct));

    QString monthBarColor = (pct >= 95) ? "#f38ba8"
                            : (pct >= 80) ? "#fab387"
                                          : "#a6e3a1";
    monthBar->setStyleSheet(
        "QProgressBar { border:1px solid #44475a; border-radius:4px; "
        "background:#313244; color:#ffffff; text-align:center; }"
        "QProgressBar::chunk { background:" + monthBarColor + "; border-radius:4px; }");

    float rate = monthlyUnitsUsed / 26.0f;
    float daysLeft = (monthlyLimit - monthlyUnitsUsed) / rate;
    QLabel* projLabel = new QLabel(
        QString("At current rate: limit reached in approximately %1 days")
            .arg(daysLeft, 0, 'f', 1));
    projLabel->setStyleSheet("color:#fab387; font-style:italic;");

    monthLayout->addWidget(monthBar);
    monthLayout->addWidget(projLabel);
    billLayout->addWidget(monthBox);

    // Bill calculator
    QGroupBox* billBox = new QGroupBox("Calculate Bill");
    QGridLayout* billGrid = new QGridLayout(billBox);
    billGrid->setSpacing(10);

    QSpinBox* unitsInput = new QSpinBox();
    unitsInput->setRange(1, 1000);
    unitsInput->setValue(150);
    unitsInput->setSuffix(" kWh");
    unitsInput->setFixedWidth(120);

    QCheckBox* peakCheck = new QCheckBox("Peak hour surcharge (25%)");

    QPushButton* calcBillBtn = new QPushButton("Calculate from Units");
    calcBillBtn->setStyleSheet(
        "background:#89b4fa; color:#1e1e2e; font-weight:bold; padding:6px 16px;");

    QPushButton* gridBillBtn = new QPushButton("Calculate from Live Grid (1 hr)");
    gridBillBtn->setStyleSheet(
        "background:#cba6f7; color:#1e1e2e; font-weight:bold; padding:6px 16px;");

    billResultLabel = new QLabel("Result will appear here");
    billResultLabel->setStyleSheet(
        "font-size:18px; font-weight:bold; padding:14px;"
        "background:#313244; border-radius:6px; color:#a6e3a1;");
    billResultLabel->setAlignment(Qt::AlignCenter);

    billGrid->addWidget(new QLabel("Units consumed:"), 0, 0);
    billGrid->addWidget(unitsInput,    0, 1);
    billGrid->addWidget(peakCheck,     1, 0, 1, 2);
    billGrid->addWidget(calcBillBtn,   2, 0);
    billGrid->addWidget(gridBillBtn,   2, 1);
    billGrid->addWidget(billResultLabel, 3, 0, 1, 2);
    billLayout->addWidget(billBox);
    billLayout->addStretch();

    connect(calcBillBtn, &QPushButton::clicked, this, [=](){
        float units = unitsInput->value();
        float bill  = billing->calculateBill(units, peakCheck->isChecked());
        billResultLabel->setText(
            QString("Bill: Rs. %1  |  Units: %2 kWh%3")
                .arg(bill, 0, 'f', 2)
                .arg(units, 0, 'f', 1)
                .arg(peakCheck->isChecked() ? "  [Peak hour +25%]" : ""));
    });

    connect(gridBillBtn, &QPushButton::clicked, this, [=](){
        float bill = billing->calculateBill(*grid, 1.0f);
        float total = 0;
        for(const auto& app : grid->getAppliances())
            total += app->calculateLoad();
        float units = total / 1000.0f;
        billResultLabel->setText(
            QString("Live grid bill (1 hr): Rs. %1  |  %2 kWh")
                .arg(bill, 0, 'f', 2)
                .arg(units, 0, 'f', 3));
    });

    tabs->addTab(billTab, "Billing");

    // ════════════════════════════════
    //  TAB 3 — AUDIT
    // ════════════════════════════════
    QWidget* auditTab = new QWidget();
    auditTab->setStyleSheet("background: #1e1e2e;");
    QVBoxLayout* auditLayout = new QVBoxLayout(auditTab);

    QLabel* auditInfo = new QLabel(
        "The audit inspects power factor of every appliance. "
        "Inductive loads (AC, Fan, Motor) with PF below 0.85 are flagged.");
    auditInfo->setStyleSheet("color:#a6adc8; font-style:italic; padding:4px;");
    auditInfo->setWordWrap(true);

    QPushButton* runAuditBtn = new QPushButton("Run Efficiency Audit");
    runAuditBtn->setStyleSheet(
        "background:#f38ba8; color:#1e1e2e; font-weight:bold; "
        "padding:10px 24px; font-size:14px;");

    auditLogDisplay = new QTextEdit();
    auditLogDisplay->setReadOnly(true);
    auditLogDisplay->setFont(QFont("Courier New", 10));
    auditLogDisplay->setStyleSheet(
        "background:#181825; color:#cdd6f4; border:1px solid #44475a;");
    auditLogDisplay->setPlaceholderText(
        "Click 'Run Efficiency Audit' to inspect all appliances...");

    auditLayout->addWidget(auditInfo);
    auditLayout->addWidget(runAuditBtn);
    auditLayout->addWidget(auditLogDisplay);

    connect(runAuditBtn, &QPushButton::clicked, this, [=](){
        std::ostringstream oss;
        std::streambuf* old = std::cout.rdbuf(oss.rdbuf());
        auditor->runAudit(*grid);
        std::cout.rdbuf(old);
        auditLogDisplay->setText(QString::fromStdString(oss.str()));
    });

    tabs->addTab(auditTab, "Audit");
}

void MainWindow::updateDashboard() {
    float total = 0.0f;
    for(const auto& app : grid->getAppliances())
        total += app->calculateLoad();

    float pct = (total / grid->getMaxCapacity()) * 100.0f;

    totalLoadLabel->setText(QString("%1 W").arg((int)total));
    loadBar->setValue((int)total);

    updateLED(pct);

    if(pct >= 100.0f){
        safetyStatusLabel->setText("CRITICAL — OVERLOAD!");
        safetyStatusLabel->setStyleSheet(
            "font-size:16px; color:#f38ba8; font-weight:bold;");
        loadBar->setStyleSheet(
            "QProgressBar { border:1px solid #44475a; border-radius:4px; "
            "background:#313244; color:#ffffff; text-align:center; }"
            "QProgressBar::chunk { background:#f38ba8; border-radius:4px; }");
        alertBannerLabel->setText(
            "CRITICAL — Grid overloaded! Turn off appliances immediately.");
        alertBannerLabel->setStyleSheet(
            "background:#f38ba8; color:#1e1e2e; padding:10px;"
            "border-radius:6px; font-weight:bold; font-size:13px;");
        alertBannerLabel->show();
    } else if(pct >= 85.0f){
        safetyStatusLabel->setText(
            QString("Warning — %1%").arg(pct, 0, 'f', 1));
        safetyStatusLabel->setStyleSheet(
            "font-size:16px; color:#fab387; font-weight:bold;");
        loadBar->setStyleSheet(
            "QProgressBar { border:1px solid #44475a; border-radius:4px; "
            "background:#313244; color:#ffffff; text-align:center; }"
            "QProgressBar::chunk { background:#fab387; border-radius:4px; }");
    } else {
        safetyStatusLabel->setText(
            QString("Safe — %1%").arg(pct, 0, 'f', 1));
        safetyStatusLabel->setStyleSheet(
            "font-size:16px; color:#a6e3a1; font-weight:bold;");
        loadBar->setStyleSheet(
            "QProgressBar { border:1px solid #44475a; border-radius:4px; "
            "background:#313244; color:#ffffff; text-align:center; }"
            "QProgressBar::chunk { background:#a6e3a1; border-radius:4px; }");
    }

    refreshApplianceTable();
}

void MainWindow::updateLED(float pct) {
    ledBlinkTimer->stop();
    ledIndicatorLabel->show();
    ledVisible = true;

    if(pct >= 100.0f){
        ledIndicatorLabel->setStyleSheet("font-size:28px; color:#f38ba8;");
        ledBlinkTimer->start(400);
    } else if(pct >= 85.0f){
        ledIndicatorLabel->setStyleSheet("font-size:28px; color:#fab387;");
        ledBlinkTimer->start(900);
    } else {
        ledIndicatorLabel->setStyleSheet("font-size:28px; color:#a6e3a1;");
    }
}

void MainWindow::checkMonthlyAlert() {
    float pct          = monthlyUnitsUsed / monthlyLimit;
    float rate         = monthlyUnitsUsed / 26.0f;
    float daysToLimit  = (monthlyLimit - monthlyUnitsUsed) / rate;

    // Show banner
    if(daysToLimit <= 5.0f || pct >= 0.80f){
        alertBannerLabel->setText(
            QString("⚠  Monthly alert: projected to exhaust limit in %1 days"
                    "  (%2 / %3 kWh used)")
                .arg(daysToLimit, 0, 'f', 1)
                .arg(monthlyUnitsUsed).arg(monthlyLimit));
        alertBannerLabel->show();
    }

    // Popup notification
    if(daysToLimit <= 5.0f){
        QMessageBox alert(this);
        alert.setWindowTitle("Monthly Limit Alert");
        alert.setIcon(QMessageBox::Warning);
        alert.setText(
            QString("<b>You are close to your monthly electricity limit!</b>"));
        alert.setInformativeText(
            QString("At your current usage rate, you will exhaust your "
                    "monthly limit of <b>%1 kWh</b> in approximately "
                    "<b>%2 days</b>.<br><br>"
                    "Units used so far: <b>%3 kWh</b><br>"
                    "Units remaining: <b>%4 kWh</b><br><br>"
                    "Consider turning off non-essential appliances.")
                .arg(monthlyLimit)
                .arg(daysToLimit, 0, 'f', 1)
                .arg(monthlyUnitsUsed)
                .arg(monthlyLimit - monthlyUnitsUsed));
        alert.setStandardButtons(QMessageBox::Ok);
        alert.exec();
    }

    if(pct >= 0.95f){
        QMessageBox critical(this);
        critical.setWindowTitle("Critical — 95% Limit Reached");
        critical.setIcon(QMessageBox::Critical);
        critical.setText("<b>You have used 95% of your monthly limit!</b>");
        critical.setInformativeText(
            QString("Only <b>%1 kWh</b> remaining this month. "
                    "Reduce usage immediately to avoid excess charges.")
                .arg(monthlyLimit - monthlyUnitsUsed));
        critical.setStandardButtons(QMessageBox::Ok);
        critical.exec();
    }
}

void MainWindow::refreshApplianceTable() {
    const auto& apps = grid->getAppliances();
    applianceTable->setRowCount((int)apps.size());

    int row = 0;
    for(const auto& app : apps){
        float load = app->calculateLoad();

        QString stateStr;
        switch(app->getState()){
        case ApplianceState::ON:      stateStr = "ON";      break;
        case ApplianceState::OFF:     stateStr = "OFF";     break;
        case ApplianceState::STANDBY: stateStr = "STANDBY"; break;
        }

        QString typeStr = (app->getRatedWattage() > 0) ? "Appliance" : "-";

        auto* nameItem  = new QTableWidgetItem(
            QString::fromStdString(app->getName()));
        auto* typeItem  = new QTableWidgetItem(typeStr);
        auto* loadItem  = new QTableWidgetItem(
            QString::number(load, 'f', 1));
        auto* pfItem    = new QTableWidgetItem("See Audit tab");
        auto* stateItem = new QTableWidgetItem(stateStr);

        // Force white text on all items
        QColor textColor("#cdd6f4");
        nameItem->setForeground(textColor);
        typeItem->setForeground(textColor);
        loadItem->setForeground(textColor);
        pfItem->setForeground(textColor);
        stateItem->setForeground(textColor);

        // Row background by state
        QColor rowBg;
        if(stateStr == "ON")      rowBg = QColor("#1e3a2f");
        else if(stateStr == "STANDBY") rowBg = QColor("#2e2a1e");
        else                      rowBg = QColor("#282a36");

        nameItem->setBackground(rowBg);
        typeItem->setBackground(rowBg);
        loadItem->setBackground(rowBg);
        pfItem->setBackground(rowBg);
        stateItem->setBackground(rowBg);

        applianceTable->setItem(row, 0, nameItem);
        applianceTable->setItem(row, 1, typeItem);
        applianceTable->setItem(row, 2, loadItem);
        applianceTable->setItem(row, 3, pfItem);
        applianceTable->setItem(row, 4, stateItem);

        row++;
    }
}

