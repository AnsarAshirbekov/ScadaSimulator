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
    ProcessState state() const;

signals:
    void stateChanged(ProcessState state);
    void speedChanged(int speed);

private:
    ProcessState m_state = ProcessState::Stopped;

    int m_currentSpeed = 0;
    int m_targetSpeed = 0;
    QTimer m_startDelayTimer;
    QTimer m_speedTimer;
};

#endif // MOTOR_H
