#pragma once
#include <QMainWindow>
#include <QTimer>

class QDoubleSpinBox;
class QSpinBox;
class QComboBox;
class QRadioButton;
class QPushButton;
class QLabel;
class QGroupBox;

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);

private slots:
    void onStartStop();
    void onTick();
    void onModeChanged();
    void onIntervalUnitChanged();
    void onDurationUnitChanged();

private:
    void updateUI();
    void stop();

    double intervalSeconds() const;
    double durationSeconds() const;

    QDoubleSpinBox* m_intervalSpin;
    QComboBox*      m_intervalUnit;

    QRadioButton*   m_radioInfinite;
    QRadioButton*   m_radioTimer;
    QRadioButton*   m_radioCount;

    QDoubleSpinBox* m_durationSpin;
    QComboBox*      m_durationUnit;
    QSpinBox*       m_clickCountSpin;

    QPushButton*    m_startStopBtn;
    QLabel*         m_statusLabel;
    QGroupBox*      m_modeBox;

    QTimer  m_clickTimer;
    bool    m_running    = false;
    int     m_clicksDone = 0;
    double  m_elapsedSecs = 0.0;
};
