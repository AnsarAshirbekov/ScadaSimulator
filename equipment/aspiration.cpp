#include "equipment/aspiration.h"
#include "equipment/motor.h"

Aspiration::Aspiration(QObject *parent)
    : QObject(parent)
{
    m_stopDelayTimer.setSingleShot(true);

    connect(&m_stopDelayTimer, &QTimer::timeout, this, [this]()
    {
        if(m_mode == AspirationMode::Auto)
        {
            m_state = AspirationState::Off;
            emit stateChanged(m_state);

            if(m_cleanRequested)
            {
                startCleaning();
            }
        }
    });

    m_cleanTimer.setSingleShot(true);

    connect(&m_cleanTimer, &QTimer::timeout, this, [this]()
    {
        m_state = AspirationState::Off;
        m_cleanRequested = false;
        emit stateChanged(m_state);
        emit cleaningFinished();
    });
}

AspirationState Aspiration::state() const
{
    return m_state;
}

AspirationMode Aspiration::mode() const
{
    return m_mode;
}

void Aspiration::setMode(AspirationMode mode)
{
    m_mode = mode;
    emit modeChanged(mode);
}

void Aspiration::turnOn()
{
    if (m_mode == AspirationMode::Manual)
    {
        m_state = AspirationState::On;
        emit stateChanged(m_state);
    }
}

void Aspiration::turnOff()
{
    if (m_mode == AspirationMode::Manual)
    {
        m_state = AspirationState::Off;
        emit stateChanged(m_state);
    }
}

void Aspiration::updateFromMotors(const QVector<Motor*>& motors)
{
    if(m_state == AspirationState::Cleaning)
        return;

    if(m_mode != AspirationMode::Auto)
        return;

    bool anyRunning = false;

    for(const Motor* m : motors)
    {
        if(m->state() == MotorState::Running)
        {
            anyRunning = true;
            break;
        }
    }

    if(anyRunning)
    {
        m_stopDelayTimer.stop();

        if(m_state != AspirationState::On)
        {
            m_state = AspirationState::On;
            emit stateChanged(m_state);
        }
    }
    else
    {
        if(m_state == AspirationState::Off && m_cleanRequested)
        {
            startCleaning();
            return;
        }

        if(!m_stopDelayTimer.isActive())
        {
            m_stopDelayTimer.start(10000);
        }
    }
}

void Aspiration::requestCleaning()
{
    if(m_state == AspirationState::Cleaning)
        return;

    m_cleanRequested = true;
}

void Aspiration::startCleaning()
{
    m_stopDelayTimer.stop();

    m_cleanRequested = false;

    m_state = AspirationState::Cleaning;
    emit stateChanged(m_state);

    m_cleanTimer.start(30000);
}
