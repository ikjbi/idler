#include "MainWindow.h"
#include "clicker.h"

#include <QDoubleSpinBox>
#include <QSpinBox>
#include <QRadioButton>
#include <QPushButton>
#include <QLabel>
#include <QGroupBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QButtonGroup>

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent) {
    setWindowTitle("Auto Clicker");
    setFixedSize(320, 300);

    auto* central = new QWidget(this);
    setCentralWidget(central);
    auto* root = new QVBoxLayout(central);
    root->setSpacing(10);
    root->setContentsMargins(14, 14, 14, 14);

    // --- Interval ---
    auto* intervalForm = new QFormLayout;
    m_intervalSpin = new QDoubleSpinBox;
    m_intervalSpin->setRange(0.05, 3600.0);
    m_intervalSpin->setDecimals(2);
    m_intervalSpin->setSuffix(" s");
    m_intervalSpin->setValue(1.0);
    intervalForm->addRow("Click every:", m_intervalSpin);
    root->addLayout(intervalForm);

    // --- Mode ---
    m_modeBox = new QGroupBox("Mode");
    auto* modeLayout = new QVBoxLayout(m_modeBox);

    m_radioInfinite = new QRadioButton("Infinite");
    m_radioTimer    = new QRadioButton("Stop after duration");
    m_radioCount    = new QRadioButton("Stop after N clicks");
    m_radioInfinite->setChecked(true);

    auto* btnGroup = new QButtonGroup(this);
    btnGroup->addButton(m_radioInfinite);
    btnGroup->addButton(m_radioTimer);
    btnGroup->addButton(m_radioCount);

    auto* timerRow = new QHBoxLayout;
    m_timerDurationSpin = new QDoubleSpinBox;
    m_timerDurationSpin->setRange(1.0, 86400.0);
    m_timerDurationSpin->setDecimals(1);
    m_timerDurationSpin->setSuffix(" s");
    m_timerDurationSpin->setValue(10.0);
    m_timerDurationSpin->setEnabled(false);
    timerRow->addWidget(m_radioTimer);
    timerRow->addWidget(m_timerDurationSpin);
    timerRow->addStretch();

    auto* countRow = new QHBoxLayout;
    m_clickCountSpin = new QSpinBox;
    m_clickCountSpin->setRange(1, 1000000);
    m_clickCountSpin->setValue(10);
    m_clickCountSpin->setEnabled(false);
    countRow->addWidget(m_radioCount);
    countRow->addWidget(m_clickCountSpin);
    countRow->addStretch();

    modeLayout->addWidget(m_radioInfinite);
    modeLayout->addLayout(timerRow);
    modeLayout->addLayout(countRow);
    root->addWidget(m_modeBox);

    // --- Status ---
    m_statusLabel = new QLabel("Stopped");
    m_statusLabel->setAlignment(Qt::AlignCenter);
    root->addWidget(m_statusLabel);

    // --- Start/Stop ---
    m_startStopBtn = new QPushButton("Start");
    m_startStopBtn->setFixedHeight(36);
    QFont f = m_startStopBtn->font();
    f.setBold(true);
    m_startStopBtn->setFont(f);
    root->addWidget(m_startStopBtn);

    connect(m_startStopBtn, &QPushButton::clicked, this, &MainWindow::onStartStop);
    connect(&m_clickTimer,  &QTimer::timeout,      this, &MainWindow::onTick);
    connect(m_radioInfinite, &QRadioButton::toggled, this, &MainWindow::onModeChanged);
    connect(m_radioTimer,    &QRadioButton::toggled, this, &MainWindow::onModeChanged);
    connect(m_radioCount,    &QRadioButton::toggled, this, &MainWindow::onModeChanged);
}

void MainWindow::onModeChanged() {
    m_timerDurationSpin->setEnabled(m_radioTimer->isChecked());
    m_clickCountSpin->setEnabled(m_radioCount->isChecked());
}

void MainWindow::onStartStop() {
    if (m_running) {
        stop();
    } else {
        m_clicksDone = 0;
        m_elapsedSecs = 0.0;
        m_running = true;
        int intervalMs = static_cast<int>(m_intervalSpin->value() * 1000.0);
        m_clickTimer.start(intervalMs);
        m_startStopBtn->setText("Stop");
        m_intervalSpin->setEnabled(false);
        m_modeBox->setEnabled(false);
        updateUI();
    }
}

void MainWindow::onTick() {
    performLeftClick();
    ++m_clicksDone;
    m_elapsedSecs += m_intervalSpin->value();
    updateUI();

    if (m_radioCount->isChecked() && m_clicksDone >= m_clickCountSpin->value()) {
        stop();
        return;
    }
    if (m_radioTimer->isChecked() && m_elapsedSecs >= m_timerDurationSpin->value()) {
        stop();
    }
}

void MainWindow::stop() {
    m_clickTimer.stop();
    m_running = false;
    m_startStopBtn->setText("Start");
    m_intervalSpin->setEnabled(true);
    m_modeBox->setEnabled(true);
    m_statusLabel->setText(QString("Stopped — %1 click(s) sent").arg(m_clicksDone));
}

void MainWindow::updateUI() {
    if (!m_running) return;

    QString status = QString("Running — %1 click(s)").arg(m_clicksDone);
    if (m_radioTimer->isChecked()) {
        double remaining = m_timerDurationSpin->value() - m_elapsedSecs;
        status += QString(" | %.1f s left").arg(qMax(0.0, remaining));
    } else if (m_radioCount->isChecked()) {
        int remaining = m_clickCountSpin->value() - m_clicksDone;
        status += QString(" | %1 left").arg(qMax(0, remaining));
    }
    m_statusLabel->setText(status);
}
