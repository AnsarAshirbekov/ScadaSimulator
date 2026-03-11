#ifndef CONVEYOR_H
#define CONVEYOR_H

#include <QObject>
#include <QVector>
#include "equipment/motor.h"
#include "equipment/plow.h"

class Conveyor : public QObject
{
    Q_OBJECT

public:
    Conveyor(int plowCount);

    Motor* motor();
    QVector<Plow*>& plows();

private:
    Motor m_motor;
    QVector<Plow*> m_plows;
};

#endif // CONVEYOR_H
