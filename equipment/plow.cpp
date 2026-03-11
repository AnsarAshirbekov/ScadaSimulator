#include "plow.h"

Plow::Plow(QObject *parent)
    : QObject(parent)
{
    connect(&m_timer, &QTimer::timeout, this, [this]() {

        if (m_state == PlowState::Lowering) {

            m_position++;

            if (m_position >= 100) {
                m_position = 100;
                m_state = PlowState::Lowered;
                m_timer.stop();
                emit stateChanged(m_state);
            }
        }

        if (m_state == PlowState::Raising) {

            m_position--;

            if (m_position <= 0) {
                m_position = 0;
                m_state = PlowState::Raised;
                m_timer.stop();
                emit stateChanged(m_state);
            }
        }

        emit positionChanged(m_position);
    });
}

void Plow::lower()
{
    m_state = PlowState::Lowering;
    m_timer.start(100);

    emit stateChanged(m_state);
}

void Plow::raise()
{
    m_state = PlowState::Raising;
    m_timer.start(100);

    emit stateChanged(m_state);
}

void Plow::stop()
{
    m_state = PlowState::Stopped;
    m_timer.stop();

    emit stateChanged(m_state);
}

int Plow::position() const
{
    return m_position;
}

PlowState Plow::state() const
{
    return m_state;
}
