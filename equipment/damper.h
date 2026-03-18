#ifndef DAMPER_H
#define DAMPER_H
#include <QObject>
#include "equipment/processstates.h"

class Damper : public QObject
{
    Q_OBJECT

public:
    explicit Damper(QObject *parent = nullptr);

    DamperState state() const;
    DamperMode mode() const;

    void setMode(DamperMode mode);

public:
    void open();
    void close();

    void updateFromPlow(int plowPosition);

signals:
    void stateChanged(DamperState state);
    void modeChanged(DamperMode mode);

private:
    DamperState m_state = DamperState::Closed;
    DamperMode m_mode = DamperMode::Auto;
};

#endif // DAMPER_H
