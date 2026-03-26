#ifndef ASPIRATION_H
#define ASPIRATION_H

#include <QObject>
#include "equipment/processstates.h"
#include <QTimer>

class Motor;

class Aspiration : public QObject
{
    Q_OBJECT

public:
    explicit Aspiration(QObject *parent = nullptr);

    AspirationState state() const;
    AspirationMode mode() const;

public:
    void setMode(AspirationMode mode);
    void turnOn();
    void turnOff();

    void updateFromMotors(const QVector<Motor*>& motors);
    void requestCleaning();
    void startCleaning();

signals:
    void stateChanged(AspirationState state);
    void modeChanged(AspirationMode mode);
    void cleaningFinished();

private:
    AspirationState m_state = AspirationState::Off;
    AspirationMode m_mode = AspirationMode::Auto;
    QTimer m_stopDelayTimer;
    bool m_cleanRequested = false;
    QTimer m_cleanTimer;
};

#endif // ASPIRATION_H
