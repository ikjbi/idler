#pragma once
#include <QMainWindow>
#include <QTimer>

class QDoubleSpinBox;
class QSpinBox;
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

private:
    void updateUI();
    void stop();

    QDoubleSpinBox* m_intervalSpin;   // seconds between clicks
    QRadioButton*   m_radioInfinite;
    QRadioButton*   m_radioTimer;
    QRadioButton*   m_radioCount;
    QDoubleSpinBox* m_timerDurationSpin; // seconds to run
    QSpinBox*       m_clickCountSpin;    // max clicks
    QPushButton*    m_startStopBtn;
    QLabel*         m_statusLabel;
    QGroupBox*      m_modeBox;

    QTimer  m_clickTimer;
    bool    m_running = false;
    int     m_clicksDone = 0;
    double  m_elapsedSecs = 0.0;
};
