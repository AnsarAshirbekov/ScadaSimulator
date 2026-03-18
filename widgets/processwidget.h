#ifndef PROCESSWIDGET_H
#define PROCESSWIDGET_H

#include <QWidget>
#include <QTimer>
#include "equipment/plow.h"
#include "equipment/processstates.h"
#include "equipment/motor.h"
#include "equipment/damper.h"

class ProcessWidget : public QWidget
{
    Q_OBJECT

    static const int CONVEYOR_COUNT = 2;
    static const int PLOWS_PER_CONVEYOR = 10;
    static const int CONVEYOR_SPACING = 350;

public:
    explicit ProcessWidget(QWidget *parent = nullptr);
    void setRunning(int index, bool running);
    void setTargetSpeed(int index, int speed);

    void plowLower();
    void plowRaise();
    void plowStop();

    void damperOpen();
    void damperClose();
    void damperSetMode(int index, DamperMode mode);

signals:
    void motorClicked(int index);
    void stateChanged(int index, ProcessState state);
    void speedChanged(int index, int speed);

    void plowPositionChanged(int index, int percent);
    void plowStateChanged(int index, PlowState state);
    void plowClicked(int index);

    void damperClicked(int index);
    void damperStateChanged(int index, DamperState state);
    void damperModeChanged(int index, DamperMode mode);

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;

private:
    QVector<double> m_offsets;
    bool m_lampOn = false;

    QTimer m_animationTimer;

    QVector<Motor*> m_motors;
    QVector<Plow*> m_plows;
    QVector<QPoint> m_pipeEnds;
    QVector<Damper*> m_dampers;

    int m_selectedPlow = -1;
    int m_selectedDamper = -1;

    bool m_plowBlink = false;
    bool m_arrowBlink = false;

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
    void drawAspiration(QPainter& p, int x, int y, const QString& name);
};

#endif // PROCESSWIDGET_H
