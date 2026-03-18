#include "equipment/damper.h"

Damper::Damper(QObject *parent)
    : QObject(parent)
{
}

DamperState Damper::state() const
{
    return m_state;
}

DamperMode Damper::mode() const
{
    return m_mode;
}

void Damper::setMode(DamperMode mode)
{
    m_mode = mode;
    emit modeChanged(mode);
}

void Damper::open()
{
    if(m_mode == DamperMode::Manual)
    {
        m_state = DamperState::Open;
        emit stateChanged(m_state);
    }
}

void Damper::close()
{
    if(m_mode == DamperMode::Manual)
    {
        m_state = DamperState::Closed;
        emit stateChanged(m_state);
    }
}

void Damper::updateFromPlow(int plowPosition)
{
    if(m_mode != DamperMode::Auto)
        return;

    DamperState newState = plowPosition > 40 ? DamperState::Open : DamperState::Closed;

    if(newState != m_state)
    {
        m_state = newState;
        emit stateChanged(m_state);
    }
}
