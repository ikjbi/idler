#include "MainWindow.h"
#include "clicker.h"

#include <QDoubleSpinBox>
#include <QSpinBox>
#include <QComboBox>
#include <QRadioButton>
#include <QPushButton>
#include <QLabel>
#include <QGroupBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QButtonGroup>

static const double UNIT_FACTORS[] = { 1.0, 60.0, 3600.0 }; // s, min, hr

// Spin box limits per unit: {min, max, decimals, step}
struct SpinConfig { double min, max; int decimals; double step; };

static const SpinConfig INTERVAL_CFG[] = {
    { 0.05,  3600.0, 2, 0.1  }, // seconds
    { 0.01,    60.0, 2, 0.5  }, // minutes
    { 0.001,   24.0, 3, 0.05 }, // hours
};
static const SpinConfig DURATION_CFG[] = {
    { 1.0,  86400.0, 1, 1.0  }, // seconds
    { 0.1,   1440.0, 1, 1.0  }, // minutes
    { 0.01,    24.0, 2, 0.25 }, // hours
};

static void applyConfig(QDoubleSpinBox* spin, const SpinConfig& cfg) {
    spin->blockSignals(true);
    spin->setDecimals(cfg.decimals);
    spin->setRange(cfg.min, cfg.max);
    spin->setSingleStep(cfg.step);
    spin->blockSignals(false);
}

static QComboBox* makeUnitCombo(QWidget* parent) {
    auto* c = new QComboBox(parent);
    c->addItems({ "seconds", "minutes", "hours" });
    return c;
}

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent) {
    setWindowTitle("Auto Clicker");
    setFixedWidth(360);

    auto* central = new QWidget(this);
    setCentralWidget(central);
    auto* root = new QVBoxLayout(central);
    root->setSpacing(10);
    root->setContentsMargins(14, 14, 14, 14);

    // --- Interval row ---
    auto* intervalRow = new QHBoxLayout;
    auto* intervalLabel = new QLabel("Click every:");
    m_intervalSpin = new QDoubleSpinBox;
    m_intervalSpin->setValue(1.0);
    m_intervalUnit = makeUnitCombo(central);
    applyConfig(m_intervalSpin, INTERVAL_CFG[0]);

    intervalRow->addWidget(intervalLabel);
    intervalRow->addWidget(m_intervalSpin);
    intervalRow->addWidget(m_intervalUnit);
    root->addLayout(intervalRow);

    // --- Mode group ---
    m_modeBox = new QGroupBox("Mode");
    auto* modeLayout = new QVBoxLayout(m_modeBox);

    m_radioInfinite = new QRadioButton("Infinite");
    m_radioTimer    = new QRadioButton("Stop after");
    m_radioCount    = new QRadioButton("Stop after");
    m_radioInfinite->setChecked(true);

    auto* btnGroup = new QButtonGroup(this);
    btnGroup->addButton(m_radioInfinite);
    btnGroup->addButton(m_radioTimer);
    btnGroup->addButton(m_radioCount);

    // Timer row
    auto* timerRow = new QHBoxLayout;
    m_durationSpin = new QDoubleSpinBox;
    m_durationSpin->setValue(10.0);
    m_durationSpin->setEnabled(false);
    m_durationUnit = makeUnitCombo(m_modeBox);
    m_durationUnit->setEnabled(false);
    applyConfig(m_durationSpin, DURATION_CFG[0]);

    timerRow->addWidget(m_radioTimer);
    timerRow->addWidget(m_durationSpin);
    timerRow->addWidget(m_durationUnit);

    // Count row
    auto* countRow = new QHBoxLayout;
    m_clickCountSpin = new QSpinBox;
    m_clickCountSpin->setRange(1, 10000000);
    m_clickCountSpin->setValue(10);
    m_clickCountSpin->setEnabled(false);
    auto* clicksLabel = new QLabel("clicks");

    countRow->addWidget(m_radioCount);
    countRow->addWidget(m_clickCountSpin);
    countRow->addWidget(clicksLabel);
    countRow->addStretch();

    modeLayout->addWidget(m_radioInfinite);
    modeLayout->addLayout(timerRow);
    modeLayout->addLayout(countRow);
    root->addWidget(m_modeBox);

    // --- Status + button ---
    m_statusLabel = new QLabel("Stopped");
    m_statusLabel->setAlignment(Qt::AlignCenter);
    root->addWidget(m_statusLabel);

    m_startStopBtn = new QPushButton("Start");
    m_startStopBtn->setFixedHeight(36);
    QFont f = m_startStopBtn->font();
    f.setBold(true);
    m_startStopBtn->setFont(f);
    root->addWidget(m_startStopBtn);

    // Connections
    connect(m_startStopBtn,  &QPushButton::clicked,       this, &MainWindow::onStartStop);
    connect(&m_clickTimer,   &QTimer::timeout,            this, &MainWindow::onTick);
    connect(m_radioInfinite, &QRadioButton::toggled,      this, &MainWindow::onModeChanged);
    connect(m_radioTimer,    &QRadioButton::toggled,      this, &MainWindow::onModeChanged);
    connect(m_radioCount,    &QRadioButton::toggled,      this, &MainWindow::onModeChanged);
    connect(m_intervalUnit,  &QComboBox::currentIndexChanged, this, &MainWindow::onIntervalUnitChanged);
    connect(m_durationUnit,  &QComboBox::currentIndexChanged, this, &MainWindow::onDurationUnitChanged);

    adjustSize();
}

// ── Helpers ────────────────────────────────────────────────────────────────

double MainWindow::intervalSeconds() const {
    return m_intervalSpin->value() * UNIT_FACTORS[m_intervalUnit->currentIndex()];
}

double MainWindow::durationSeconds() const {
    return m_durationSpin->value() * UNIT_FACTORS[m_durationUnit->currentIndex()];
}

// ── Slots ──────────────────────────────────────────────────────────────────

void MainWindow::onModeChanged() {
    bool timerMode = m_radioTimer->isChecked();
    bool countMode = m_radioCount->isChecked();
    m_durationSpin->setEnabled(timerMode);
    m_durationUnit->setEnabled(timerMode);
    m_clickCountSpin->setEnabled(countMode);
}

void MainWindow::onIntervalUnitChanged() {
    applyConfig(m_intervalSpin, INTERVAL_CFG[m_intervalUnit->currentIndex()]);
}

void MainWindow::onDurationUnitChanged() {
    applyConfig(m_durationSpin, DURATION_CFG[m_durationUnit->currentIndex()]);
}

void MainWindow::onStartStop() {
    if (m_running) {
        stop();
        return;
    }

    m_clicksDone  = 0;
    m_elapsedSecs = 0.0;
    m_running     = true;

    int intervalMs = qMax(50, static_cast<int>(intervalSeconds() * 1000.0));
    m_clickTimer.start(intervalMs);

    m_startStopBtn->setText("Stop");
    m_intervalSpin->setEnabled(false);
    m_intervalUnit->setEnabled(false);
    m_modeBox->setEnabled(false);
    updateUI();
}

void MainWindow::onTick() {
    performLeftClick();
    ++m_clicksDone;
    m_elapsedSecs += intervalSeconds();
    updateUI();

    if (m_radioCount->isChecked() && m_clicksDone >= m_clickCountSpin->value()) {
        stop();
        return;
    }
    if (m_radioTimer->isChecked() && m_elapsedSecs >= durationSeconds()) {
        stop();
    }
}

void MainWindow::stop() {
    m_clickTimer.stop();
    m_running = false;
    m_startStopBtn->setText("Start");
    m_intervalSpin->setEnabled(true);
    m_intervalUnit->setEnabled(true);
    m_modeBox->setEnabled(true);
    m_statusLabel->setText(QString("Stopped — %1 click(s) sent").arg(m_clicksDone));
}

void MainWindow::updateUI() {
    if (!m_running) return;

    QString status = QString("Running — %1 click(s)").arg(m_clicksDone);

    if (m_radioTimer->isChecked()) {
        double remaining = durationSeconds() - m_elapsedSecs;
        remaining = qMax(0.0, remaining);

        QString timeStr;
        if (remaining >= 3600.0)
            timeStr = QString::number(remaining / 3600.0, 'f', 2) + " hr";
        else if (remaining >= 60.0)
            timeStr = QString::number(remaining / 60.0, 'f', 1) + " min";
        else
            timeStr = QString::number(remaining, 'f', 1) + " s";

        status += " | " + timeStr + " left";

    } else if (m_radioCount->isChecked()) {
        int remaining = m_clickCountSpin->value() - m_clicksDone;
        status += QString(" | %1 left").arg(qMax(0, remaining));
    }

    m_statusLabel->setText(status);
}
