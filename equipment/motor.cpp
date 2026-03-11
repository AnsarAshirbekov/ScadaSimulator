#include "motor.h"

Motor::Motor(QObject *parent)
    : QObject(parent)
{
    m_startDelayTimer.setSingleShot(true);

    connect(&m_startDelayTimer, &QTimer::timeout, this, [this]() {

        m_state = ProcessState::Running;
        m_currentSpeed = 10;
        m_targetSpeed = 10;

        emit stateChanged(m_state);
        emit speedChanged(m_currentSpeed);
    });

    connect(&m_speedTimer, &QTimer::timeout, this, [this]() {

        if (m_state != ProcessState::Running)
            return;

        if (m_currentSpeed < m_targetSpeed)
            m_currentSpeed++;

        else if (m_currentSpeed > m_targetSpeed)
            m_currentSpeed--;

        emit speedChanged(m_currentSpeed);
    });

    m_speedTimer.start(80);
}

void Motor::start()
{
    if (m_state == ProcessState::Stopped)
    {
        m_state = ProcessState::PreStart;
        emit stateChanged(m_state);

        m_startDelayTimer.start(10000);
    }
}

void Motor::stop()
{
    m_state = ProcessState::Stopped;

    m_currentSpeed = 10;
    m_targetSpeed = 10;

    emit stateChanged(m_state);
    emit speedChanged(m_currentSpeed);
}

void Motor::setTargetSpeed(int speed)
{
    m_targetSpeed = speed;
}

int Motor::currentSpeed() const
{
    return m_currentSpeed;
}

ProcessState Motor::state() const
{
    return m_state;
}
