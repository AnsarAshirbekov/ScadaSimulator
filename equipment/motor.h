#ifndef MOTOR_H
#define MOTOR_H

#include <QObject>
#include <QTimer>
#include "equipment/processstates.h"

class Motor : public QObject
{
    Q_OBJECT

public:
    explicit Motor(QObject *parent = nullptr);

    void start();
    void stop();
    void setTargetSpeed(int speed);

    int currentSpeed() const;
    MotorState state() const;

    void triggerFault();
    void resetFault();
    bool isFault() const;
    void closeKSL();
    void openKSL();

signals:
    void stateChanged(MotorState state);
    void speedChanged(int speed);

private:
    MotorState m_state = MotorState::Stopped;

    int m_currentSpeed = 0;
    int m_targetSpeed = 0;
    QTimer m_startDelayTimer;
    QTimer m_speedTimer;
    bool m_fault = false;
    bool m_KSLClosed = false;
};

#endif // MOTOR_H
