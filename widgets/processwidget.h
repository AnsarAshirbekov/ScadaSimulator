#ifndef PROCESSWIDGET_H
#define PROCESSWIDGET_H

#include <QWidget>
#include <QTimer>
#include "equipment/plow.h"
#include "equipment/processstates.h"
#include "equipment/motor.h"
#include "equipment/damper.h"
#include "equipment/aspiration.h"
#include "events/eventlogger.h"

class EventLogger;

class ProcessWidget : public QWidget
{
    Q_OBJECT

    static const int CONVEYOR_COUNT = 2;
    static const int PLOWS_PER_CONVEYOR = 10;
    static const int CONVEYOR_SPACING = 350;
    static const int ASPIRATION_COUNT = 2;

public:
    explicit ProcessWidget(QWidget *parent = nullptr);
    void motorStart(int index);
    void motorStop(int index);
    void setTargetSpeed(int index, int speed);
    void motorFault(int index);
    void motorReset(int index);
    void openKSL(int index);

    void plowLower(int index);
    void plowRaise(int index);
    void plowStop(int index);

    void damperOpen(int index);
    void damperClose(int index);
    void damperSetMode(int index, DamperMode mode);

    void aspTurnOn(int index);
    void aspTurnOff(int index);
    void aspSetMode(int index, AspirationMode mode);
    void aspRequestCleaning(int index);

    void setLogger(EventLogger* logger);

signals:
    void motorClicked(int index);
    void stateChanged(int index, MotorState state);
    void speedChanged(int index, int speed);

    void plowPositionChanged(int index, int percent);
    void plowStateChanged(int index, PlowState state);
    void plowClicked(int index);

    void damperClicked(int index);
    void damperStateChanged(int index, DamperState state);
    void damperModeChanged(int index, DamperMode mode);

    void aspClicked(int index);
    void aspStateChanged(int index, AspirationState state);
    void aspModeChanged(int index, AspirationMode mode);    
    void cleaningFinished(int index);

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;

private:
    QVector<double> m_offsets;
    QVector<double> m_aspOffsets;

    QTimer m_animationTimer;

    QVector<Motor*> m_motors;
    QVector<Plow*> m_plows;
    QVector<QPoint> m_pipeEnds;
    QVector<Damper*> m_dampers;
    QVector<Aspiration*> m_aspirations;

    bool m_lampOn = false;
    bool m_plowBlink = false;
    bool m_arrowBlink = false;
    bool m_aspBlink = false;

    QRect getMotorRect(int centerY);
    QRect getLampRect(const QRect& motorRect);
    QRect getBeltRect(int centerY, int w);

    void drawMotor(QPainter& p, const QRect& rect, int index);
    void drawLamp(QPainter& p, const QRect& rect, int index);
    void drawBelt(QPainter& p, const QRect& rect, int index);
    void drawPlow(QPainter& p, int x, QRect beltRect, int plowPosition, PlowState state, bool mirror,
                    const QString& name, int motorIndex, int plowIndex);
    void drawPlows(QPainter& p, QRect beltRect, int conveyor);
    void drawConveyor(QPainter& p, int conveyor, int centerY, int w);
    void drawDamper(QPainter& p, const QRect& chuteRect, bool mirror, const QString& name, int index);
    void drawMainPipes(QPainter& p);
    void drawAspiration(QPainter& p, int x, int y, const QString& name, int index);

    EventLogger* m_logger = nullptr;
};

#endif // PROCESSWIDGET_H
