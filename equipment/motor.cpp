#include "motor.h"

Motor::Motor(QObject *parent)
    : QObject(parent)
{
    m_startDelayTimer.setSingleShot(true);

    connect(&m_startDelayTimer, &QTimer::timeout, this, [this]() {

        if(m_state != MotorState::PreStart)
            return;

        m_state = MotorState::Running;
        m_currentSpeed = 10;
        m_targetSpeed = 10;

        emit stateChanged(m_state);
        emit speedChanged(m_currentSpeed);
    });

    connect(&m_speedTimer, &QTimer::timeout, this, [this]() {

        if (m_state != MotorState::Running)
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
    if (m_fault)
        return;

    if (m_state == MotorState::Stopped)
    {
        m_state = MotorState::PreStart;
        emit stateChanged(m_state);

        m_startDelayTimer.start(10000);
    }
}

void Motor::stop()
{
    if (m_fault)
        return;

    m_startDelayTimer.stop();

    m_state = MotorState::Stopped;

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

MotorState Motor::state() const
{
    return m_state;
}

void Motor::triggerFault()
{
    m_startDelayTimer.stop();

    m_fault = true;
    m_state = MotorState::Fault;

    m_currentSpeed = 10;
    m_targetSpeed = 10;

    emit stateChanged(m_state);
    emit speedChanged(m_currentSpeed);
}

void Motor::resetFault()
{
    if(m_KSLClosed)
        return;

    m_fault = false;
    m_state = MotorState::Stopped;

    emit stateChanged(m_state);
}

bool Motor::isFault() const
{
    return m_fault;
}

void Motor::closeKSL()
{
    m_KSLClosed = true;
}

void Motor::openKSL()
{
    m_KSLClosed = false;
}
