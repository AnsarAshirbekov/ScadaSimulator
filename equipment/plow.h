#ifndef PLOW_H
#define PLOW_H

#include <QObject>
#include <QTimer>
#include "equipment/processstates.h"

class Plow : public QObject
{
    Q_OBJECT

public:
    explicit Plow(QObject *parent = nullptr);

    void lower();
    void raise();
    void stop();

    int position() const;
    PlowState state() const;

signals:
    void positionChanged(int percent);
    void stateChanged(PlowState state);

private:
    PlowState m_state = PlowState::Raised;
    int m_position = 0;
    QTimer m_timer;
    bool m_blink = false;
};

#endif // PLOW_H
