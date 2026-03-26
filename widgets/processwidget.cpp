#include "processwidget.h"
#include <QPainter>
#include <QMouseEvent>

ProcessWidget::ProcessWidget(QWidget *parent)
    : QWidget(parent)
{
    showMaximized();

    // Анимация
    connect(&m_animationTimer, &QTimer::timeout, this, [this]() {
        for(int i = 0; i < m_motors.size(); i++)
        {
            if(m_motors[i]->state() == MotorState::Running)
            {
                double speedFactor = m_motors[i]->currentSpeed() / 20.0;

                m_offsets[i] += speedFactor;

                if(m_offsets[i] > 40)
                    m_offsets[i] = 0;
            }
        }

        bool prestart = false;
        for(int i = 0; i < m_motors.size(); i++)
        {
            if(m_motors[i]->state() == MotorState::PreStart)
            {
                prestart = true;
                break;
            }
        }
        if(prestart)
            m_lampOn = !m_lampOn;

        m_plowBlink = !m_plowBlink;
        m_arrowBlink = !m_arrowBlink;
        m_aspBlink = !m_aspBlink;

        for(int i = 0; i < m_aspirations.size(); i++)
        {
            m_aspirations[i]->updateFromMotors(m_motors);

            if(m_aspirations[i]->state() == AspirationState::On)
            {
                m_aspOffsets[i] += 1.5;

                if(m_aspOffsets[i] > 20)
                    m_aspOffsets[i] = 0;
            }
        }

        update();
    });

    m_animationTimer.start(80);

    for(int i = 0; i < CONVEYOR_COUNT; i++)
    {
        Motor* motor = new Motor(this);

        connect(motor, &Motor::stateChanged,
                this,
                [this, i](MotorState state)
                {
                    emit stateChanged(i, state);
                });

        connect(motor, &Motor::speedChanged,
                this,
                [this, i](int speed)
                {
                    emit speedChanged(i, speed);
                });

        m_motors.append(motor);
        m_offsets.append(0.0);
    }
    
    for(int i = 0; i < CONVEYOR_COUNT * PLOWS_PER_CONVEYOR; i++)
    {
        Plow *plow = new Plow(this);

        connect(plow, &Plow::positionChanged,
                this,
                [this, i](int percent)
                {
                    emit plowPositionChanged(i, percent);
                });

        connect(plow, &Plow::stateChanged,
                this,
                [this, i](PlowState state)
                {
                    emit plowStateChanged(i, state);
                });

        m_plows.append(plow);
    }

    for(int i = 0; i < CONVEYOR_COUNT * PLOWS_PER_CONVEYOR; i++)
    {
        Damper *damper = new Damper(this);

        connect(m_plows[i], &Plow::positionChanged,
                damper, &Damper::updateFromPlow);

        connect(damper, &Damper::stateChanged,
                this, [this, i](DamperState state)
                {
                    emit damperStateChanged(i, state);
                });

        connect(damper, &Damper::modeChanged,
                this, [this, i](DamperMode mode)
                {
                    emit damperModeChanged(i, mode);
                });

        m_dampers.append(damper);
    }

    for(int i = 0; i < ASPIRATION_COUNT; i++)
    {
        Aspiration* asp = new Aspiration(this);

        connect(asp, &Aspiration::stateChanged,
                this, [this, i](AspirationState state)
                {
                    emit aspStateChanged(i, state);
                });

        connect(asp, &Aspiration::modeChanged,
                this, [this, i](AspirationMode mode)
                {
                    emit aspModeChanged(i, mode);
                });

        connect(asp, &Aspiration::cleaningFinished,
                this, [this, i]()
                {
                    emit cleaningFinished(i);
                });

        m_aspOffsets.append(0.0);
        m_aspirations.append(asp);
    }
}

QRect ProcessWidget::getMotorRect(int centerY)
{
    return QRect(50, centerY - 50, 100, 100);
}

QRect ProcessWidget::getLampRect(const QRect& motorRect)
{
    return QRect(
        motorRect.left() - 45,
        motorRect.center().y() - 20,
        40,
        40
        );
}

QRect ProcessWidget::getBeltRect(int centerY, int w)
{
    return QRect(150, centerY - 20, w - 180, 40);
}

void ProcessWidget::drawMotor(QPainter& p, const QRect& rect, int index)
{
    QColor motorColor =
        m_motors[index]->state() == MotorState::Running ? Qt::red : Qt::green;

    p.setBrush(motorColor);
    p.setPen(Qt::black);
    p.drawEllipse(rect);

    p.setPen(Qt::white);

    QFont font = p.font();
    font.setPointSize(30);
    font.setBold(true);
    p.setFont(font);

    p.drawText(rect, Qt::AlignCenter, "M");
}

void ProcessWidget::drawLamp(QPainter& p, const QRect& rect, int index)
{
    QColor lampColor;

    if (m_motors[index]->state() == MotorState::Fault)
    {
        lampColor = Qt::red;
    }
    else if (m_motors[index]->state() == MotorState::Running)
    {
        lampColor = Qt::yellow;
    }
    else if (m_motors[index]->state() == MotorState::PreStart)
    {
        lampColor = m_lampOn ? Qt::red : QColor(100,0,0);
    }
    else
    {
        lampColor = QColor(60,60,60);
    }

    p.setBrush(lampColor);
    p.setPen(Qt::black);
    p.drawEllipse(rect);
}

void ProcessWidget::drawBelt(QPainter& p, const QRect& rect, int index)
{
    p.setBrush(Qt::darkGray);
    p.setPen(Qt::black);
    p.drawRect(rect);

    QPen pen(Qt::white, 3);
    pen.setStyle(Qt::DashLine);

    if (m_motors[index]->state() == MotorState::Running)
    {
        pen.setDashPattern({10,10});
        pen.setDashOffset(-m_offsets[index]);
    }

    p.setPen(pen);

    p.drawLine(
        rect.right(),
        rect.center().y(),
        rect.left(),
        rect.center().y()
        );
}

void ProcessWidget::drawPlow(QPainter& p, int x, QRect beltRect, int plowPosition, PlowState state, bool mirror,
                                const QString& name, int motorIndex, int plowIndex)
{
    QRect plowFrame(
        x - 30,
        beltRect.top() - 10,
        60,
        beltRect.height() + 20
    );

    p.setBrush(Qt::NoBrush);
    p.setPen(QPen(Qt::black, 3));

    p.drawRect(plowFrame);

    QColor plowColor = QColor(200,200,200); // серый

    switch(state)
    {
    case PlowState::Lowered:
        plowColor = Qt::red;
        break;

    case PlowState::Raised:
        plowColor = Qt::green;
        break;

    case PlowState::Lowering:
        plowColor = m_plowBlink ? Qt::red : QColor(200,200,200);
        break;

    case PlowState::Raising:
        plowColor = m_plowBlink ? Qt::green : QColor(200,200,200);
        break;

    case PlowState::Stopped:
        plowColor = QColor(200,200,200);
        break;
    }

    QPen plowPen(plowColor, 6);
    p.setPen(plowPen);

    p.setRenderHint(QPainter::Antialiasing);

    if (!mirror)
    {
        p.drawLine(
            plowFrame.left() + 5,
            plowFrame.bottom() - 5,
            plowFrame.right() - 5,
            plowFrame.top() + 5
            );
    } else {
        p.drawLine(
            plowFrame.left() + 5,
            plowFrame.top() + 5,
            plowFrame.right() - 5,
            plowFrame.bottom() - 5
        );
    }

    QRect chute;

    if(!mirror)
    {
        chute = QRect(
            plowFrame.left() - 15,
            beltRect.bottom() + 10,
            plowFrame.width(),
            40
            );
    } else {
        chute = QRect(
            plowFrame.left() - 15,
            beltRect.top() - 50,
            plowFrame.width(),
            40
        );
    }

    p.setBrush(QColor(120,120,120));
    p.setPen(Qt::black);

    p.drawRect(chute);

    QPolygon arrow;

    arrow << QPoint(chute.center().x(), chute.center().y() + 14)
          << QPoint(chute.center().x() - 10, chute.center().y() - 6)
          << QPoint(chute.center().x() + 10, chute.center().y() - 6);

    QColor arrowColor = Qt::black;

    if (m_motors[motorIndex]->state() == MotorState::Running &&
        plowPosition > 40)
    {
        arrowColor = m_arrowBlink ? Qt::red : QColor(120,0,0);
    }

    p.setBrush(arrowColor);
    p.setPen(Qt::NoPen);
    p.drawPolygon(arrow);

    QFont font = p.font();
    font.setPointSize(10);
    font.setBold(true);
    p.setFont(font);
    p.setPen(Qt::black);

    if(!mirror)
    {
        p.drawText(
            plowFrame.center().x() - 20,
            plowFrame.top() - 5,
            name
        );
    } else {
        p.drawText(
            plowFrame.center().x() - 20,
            plowFrame.bottom() + 15,
            name
        );
    }

    QString damperName = QString("З-%1%2")
                             .arg(motorIndex == 0 ? "А" : "Б")
                             .arg(plowIndex + 1);

    int damperIndex = motorIndex * PLOWS_PER_CONVEYOR + plowIndex;

    drawDamper(p, chute, mirror, damperName, damperIndex);
}

void ProcessWidget::drawPlows(QPainter& p, QRect beltRect, int conveyor)
{
    for(int i = 0; i < PLOWS_PER_CONVEYOR; i++)
    {
        int index = conveyor * PLOWS_PER_CONVEYOR + i;

        PlowState plowState = m_plows[index]->state();
        int plowPosition = m_plows[index]->position();

        QString name = QString("ПС-%1%2")
                           .arg(conveyor == 0 ? "А" : "Б")
                           .arg(i + 1);

        double t = (double)i / PLOWS_PER_CONVEYOR;

        int plowX =
            beltRect.left() +
            beltRect.width() * (0.1 + t * 0.8);

        drawPlow(
            p,
            plowX,
            beltRect,
            plowPosition,
            plowState,
            conveyor == 1,
            name,
            conveyor,
            i
            );
    }
}

void ProcessWidget::drawConveyor(QPainter& p, int conveyor, int centerY, int w)
{
    int yOffset = conveyor * CONVEYOR_SPACING;

    QRect motorRect = getMotorRect(centerY).translated(0, yOffset);
    drawMotor(p, motorRect, conveyor);

    QRect lampRect = getLampRect(motorRect);
    drawLamp(p, lampRect, conveyor);

    QRect beltRect = getBeltRect(centerY, w).translated(0, yOffset);
    drawBelt(p, beltRect, conveyor);

    QString conveyorName = conveyor == 0 ? "ЛК-А" : "ЛК-Б";

    p.setPen(Qt::black);
    p.drawText(
        beltRect.left() - 120,
        beltRect.center().y() - 70,
        conveyorName
        );

    drawPlows(p, beltRect, conveyor);
}

void ProcessWidget::drawDamper(QPainter& p, const QRect& chuteRect, bool mirror, const QString& name, int index)
{
    QRect pipe;

    if(!mirror)
    {
        pipe = QRect(
            chuteRect.left() + chuteRect.width()/2 - 8,
            chuteRect.bottom(),
            16,
            80
            );
    }
    else
    {
        pipe = QRect(
            chuteRect.left() + chuteRect.width()/2 - 8,
            chuteRect.top() - 80,
            16,
            80
            );
    }

    p.setBrush(QColor(0,100,0));
    p.setPen(Qt::black);
    p.drawRect(pipe);

    QColor damperColor = m_dampers[index]->state() == DamperState::Open ? Qt::green : Qt::red;
    QPen damperPen(damperColor, 5);
    p.setPen(damperPen);

    int y = pipe.center().y();
    p.drawLine(
        pipe.left()+2,
        y,
        pipe.right()-2,
        y
    );

    QPoint pipeEnd;

    if(!mirror)
        pipeEnd = pipe.bottomLeft() + QPoint(pipe.width()/2,0);
    else
        pipeEnd = pipe.topLeft() + QPoint(pipe.width()/2,0);

    m_pipeEnds.append(pipeEnd);

    QFont font = p.font();
    font.setPointSize(10);
    font.setBold(true);
    p.setFont(font);

    p.setPen(Qt::black);

    p.drawText(
        pipe.left() - 35,
        pipe.center().y() + 5,
        name
        );

    int plowIndex = index % PLOWS_PER_CONVEYOR;
    int aspIndex = plowIndex < 5 ? 0 : 1;
    bool aspOn = m_aspirations[aspIndex]->state() == AspirationState::On;
    bool damperOpen = m_dampers[index]->state() == DamperState::Open;

    if(aspOn && damperOpen)
    {
        p.save();

        QPen pen(Qt::white, 2);
        pen.setStyle(Qt::DashLine);
        pen.setDashPattern({6,6});

        if(!mirror)
        {
            pen.setDashOffset(-m_aspOffsets[aspIndex]);

            p.setPen(pen);

            p.drawLine(
                pipe.center().x(),
                pipe.top(),
                pipe.center().x(),
                pipe.bottom()
                );
        }
        else
        {
            pen.setDashOffset(-m_aspOffsets[aspIndex]);

            p.setPen(pen);

            p.drawLine(
                pipe.center().x(),
                pipe.bottom(),
                pipe.center().x(),
                pipe.top()
                );
        }

        p.restore();
    }
}

void ProcessWidget::drawMainPipes(QPainter& p)
{
    if(m_pipeEnds.size() < 20)
        return;

    int thickness = 55;

    int y = (m_pipeEnds[0].y() + m_pipeEnds[10].y()) / 2;

    p.setBrush(QColor(0,100,0));
    p.setPen(Qt::black);

    // первая магистраль
    int startX = m_pipeEnds[0].x() - 70;
    int endX   = m_pipeEnds[4].x() + 20;

    QRect pipe1(
        startX,
        y - thickness/2,
        endX - startX,
        thickness
        );

    p.drawRect(pipe1);

    if(m_aspirations[0]->state() == AspirationState::On)
    {
        p.save();
        QPen pen(Qt::white, 3);
        pen.setStyle(Qt::DashLine);
        pen.setDashPattern({10,10});

        pen.setDashOffset(-m_aspOffsets[0]);

        p.setPen(pen);

        p.drawLine(
            pipe1.right(),
            pipe1.center().y(),
            pipe1.left(),
            pipe1.center().y()
            );
        p.restore();
    }

    // вторая магистраль
    startX = m_pipeEnds[5].x() - 20;
    endX   = m_pipeEnds[9].x() + 70;

    QRect pipe2(
        startX,
        y - thickness/2,
        endX - startX,
        thickness
        );

    p.drawRect(pipe2);

    if(m_aspirations[1]->state() == AspirationState::On)
    {
        p.save();
        QPen pen(Qt::white, 3);
        pen.setStyle(Qt::DashLine);
        pen.setDashPattern({10,10});

        pen.setDashOffset(-m_aspOffsets[1]);

        p.setPen(pen);

        p.drawLine(
            pipe2.left(),
            pipe2.center().y(),
            pipe2.right(),
            pipe2.center().y()
            );
        p.restore();
    }

    // левая установка
    drawAspiration(p, m_pipeEnds[0].x() - 110, y, "АУ-1", 0);

    QFont font = p.font();
    font.setPointSize(10);
    font.setBold(true);
    p.setFont(font);
    p.setPen(Qt::black);

    // правая установка
    drawAspiration(p, m_pipeEnds[9].x() + 110, y, "АУ-2", 1);
}

void ProcessWidget::drawAspiration(QPainter& p, int x, int y, const QString& name, int index)
{
    int size = 80;

    QRect body(
        x - size/2,
        y - size/2,
        size,
        size
        );

    p.setBrush(QColor(0,100,0));
    p.setPen(Qt::black);
    p.drawRect(body);

    QPoint c = body.center();

    QPolygon fan;
    fan << QPoint(c.x(), c.y() - 18)
        << QPoint(c.x() - 12, c.y() + 12)
        << QPoint(c.x() + 12, c.y() + 12);

    bool isOn = m_aspirations[index]->state() == AspirationState::On;
    bool isCLeaning = m_aspirations[index]->state() == AspirationState::Cleaning;

    QColor fanColor = Qt::black;

    if(isOn)
    {
        fanColor = m_aspBlink ? Qt::red : QColor(120,0,0);
    }

    if(isCLeaning)
    {
        fanColor = m_aspBlink ? Qt::blue : QColor(0, 0, 80);
    }

    p.setBrush(fanColor);
    p.drawPolygon(fan);

    p.setPen(Qt::black);
    p.drawText(body.adjusted(0, size/2 + 15, 0, 0),
               Qt::AlignCenter,
               "ASP");

    QFont font = p.font();
    font.setPointSize(20);
    font.setBold(true);
    p.setFont(font);

    p.drawText(
        body.center().x() - 30,
        body.top() - 10,
        name
        );
}

void ProcessWidget::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);
    p.setPen(Qt::black);
    p.setBrush(Qt::NoBrush);

    int w = width();
    int h = height();
    int centerY = h / 4;

    m_pipeEnds.clear();

    for(int conveyor = 0; conveyor < CONVEYOR_COUNT; conveyor++)
    {
        drawConveyor(p, conveyor, centerY, w);
    }

    drawMainPipes(p);
}

void ProcessWidget::mousePressEvent(QMouseEvent *event)
{
    int h = height();
    int w = width();

    int centerY = h / 4;

    for(int conveyor = 0; conveyor < CONVEYOR_COUNT; conveyor++)
    {
        QRect motorRect =
            getMotorRect(centerY).translated(0, conveyor * CONVEYOR_SPACING);

        if(motorRect.contains(event->pos()))
        {
            emit motorClicked(conveyor);
        }
    }

    for(int conveyor = 0; conveyor < CONVEYOR_COUNT; conveyor++)
    {
        QRect beltRect =
            getBeltRect(centerY, w).translated(0, conveyor * CONVEYOR_SPACING);

        for(int i = 0; i < PLOWS_PER_CONVEYOR; i++)
        {
            double t = (double)i / PLOWS_PER_CONVEYOR;

            int plowX =
                beltRect.left() +
                beltRect.width() * (0.1 + t * 0.8);

            QRect plowFrame(
                plowX - 30,
                beltRect.top() - 10,
                60,
                beltRect.height() + 20
                );

            if(plowFrame.contains(event->pos()))
            {
                int index = conveyor * PLOWS_PER_CONVEYOR + i;

                emit plowClicked(index);
            }
        }
    }

    for(int conveyor = 0; conveyor < CONVEYOR_COUNT; conveyor++)
    {
        QRect beltRect =
            getBeltRect(centerY, w).translated(0, conveyor * CONVEYOR_SPACING);

        for(int i = 0; i < PLOWS_PER_CONVEYOR; i++)
        {
            double t = (double)i / PLOWS_PER_CONVEYOR;

            int plowX =
                beltRect.left() +
                beltRect.width() * (0.1 + t * 0.8);

            QRect plowFrame(
                plowX - 30,
                beltRect.top() - 10,
                60,
                beltRect.height() + 20
                );

            QRect chute;

            if(conveyor == 0)
            {
                chute = QRect(
                    plowFrame.left() - 15,
                    beltRect.bottom() + 10,
                    plowFrame.width(),
                    40
                    );
            }
            else
            {
                chute = QRect(
                    plowFrame.left() - 15,
                    beltRect.top() - 50,
                    plowFrame.width(),
                    40
                    );
            }

            QRect pipe;

            if(conveyor == 0)
            {
                pipe = QRect(
                    chute.left() + chute.width()/2 - 8,
                    chute.bottom(),
                    16,
                    80
                    );
            }
            else
            {
                pipe = QRect(
                    chute.left() + chute.width()/2 - 8,
                    chute.top() - 80,
                    16,
                    80
                    );
            }

            if(pipe.contains(event->pos()))
            {
                int index = conveyor * PLOWS_PER_CONVEYOR + i;

                emit damperClicked(index);
            }
        }
    }

    int y = (m_pipeEnds[0].y() + m_pipeEnds[10].y()) / 2;

    QRect asp1(
        m_pipeEnds[0].x() - 110 - 40,
        y - 40,
        80,
        80
        );

    QRect asp2(
        m_pipeEnds[9].x() + 110 - 40,
        y - 40,
        80,
        80
        );

    if(asp1.contains(event->pos()))
    {
        emit aspClicked(0);
    }

    if(asp2.contains(event->pos()))
    {
        emit aspClicked(1);
    }
}

void ProcessWidget::motorStart(int index)
{
    if(index >= 0 && index < m_motors.size())
        m_motors[index]->start();

    if(m_logger)
    {
        QString name = index == 0 ? "ЛК-А" : "ЛК-Б";

        m_logger->log(
            QString("Конвейер %1 запущен").arg(name)
        );
    }
}

void ProcessWidget::motorStop(int index)
{
    if(index >= 0 && index < m_motors.size())
        m_motors[index]->stop();

    if(m_logger)
    {
        QString name = index == 0 ? "ЛК-А" : "ЛК-Б";

        m_logger->log(
            QString("Конвейер %1 остановлен").arg(name)
            );
    }
}

void ProcessWidget::setTargetSpeed(int index, int speed)
{
    if(index >= 0 && index < m_motors.size())
        m_motors[index]->setTargetSpeed(speed);

    if(m_logger)
    {
        QString name = index == 0 ? "ЛК-А" : "ЛК-Б";

        m_logger->log(
            QString("Задана скорость %1 для конвейера %2").arg(speed).arg(name)
            );
    }
}

void ProcessWidget::motorFault(int index)
{
    if(index >= 0 && index < m_motors.size())
    {
        m_motors[index]->triggerFault();
        m_motors[index]->closeKSL();
    }
}

void ProcessWidget::motorReset(int index)
{
    if(index >= 0 && index < m_motors.size())
        m_motors[index]->resetFault();
}

void ProcessWidget::openKSL(int index)
{
    if(index >= 0 && index < m_motors.size())
        m_motors[index]->openKSL();
}

void ProcessWidget::plowLower(int index)
{
    if(index >= 0 && index < m_plows.size())
        m_plows[index]->lower();

    if(m_logger)
    {
        QString name = QString("ПС-%1%2")
                           .arg(index < 10 ? "А" : "Б")
                           .arg(index % 10 + 1);

        m_logger->log(
            QString("Опускание плужкового сбрасывателя %1").arg(name)
            );
    }
}

void ProcessWidget::plowRaise(int index)
{
    if(index >= 0 && index < m_plows.size())
        m_plows[index]->raise();

    if(m_logger)
    {
        QString name = QString("ПС-%1%2")
                           .arg(index < 10 ? "А" : "Б")
                           .arg(index % 10 + 1);

        m_logger->log(
            QString("Поднятие плужкового сбрасывателя %1").arg(name)
            );
    }
}

void ProcessWidget::plowStop(int index)
{
    if(index >= 0 && index < m_plows.size())
        m_plows[index]->stop();

    if(m_logger)
    {
        QString name = QString("ПС-%1%2")
                           .arg(index < 10 ? "А" : "Б")
                           .arg(index % 10 + 1);

        m_logger->log(
            QString("Плужковый сбрасыватель %1 остановлен").arg(name)
            );
    }
}

void ProcessWidget::damperOpen(int index)
{
    if(index >= 0 && index < m_dampers.size())
        m_dampers[index]->open();

    if(m_logger)
    {
        QString name = QString("З-%1%2")
                           .arg(index < 10 ? "А" : "Б")
                           .arg(index % 10 + 1);

        m_logger->log(
            QString("Заслонка %1 открыта").arg(name)
            );
    }
}

void ProcessWidget::damperClose(int index)
{
    if(index >= 0 && index < m_dampers.size())
        m_dampers[index]->close();

    if(m_logger)
    {
        QString name = QString("З-%1%2")
                           .arg(index < 10 ? "А" : "Б")
                           .arg(index % 10 + 1);

        m_logger->log(
            QString("Заслонка %1 закрыта").arg(name)
            );
    }
}

void ProcessWidget::damperSetMode(int index, DamperMode mode)
{
    if(index >= 0 && index < m_dampers.size())
        m_dampers[index]->setMode(mode);

    if(m_logger)
    {
        QString name = QString("З-%1%2")
                           .arg(index < 10 ? "А" : "Б")
                           .arg(index % 10 + 1);

        QString modeName = mode == DamperMode::Auto ? "автоматический" : "ручной";

        m_logger->log(
            QString("Установлен %1 режим для заслонки %2").arg(modeName, name)
            );
    }
}

void ProcessWidget::aspTurnOn(int index)
{
    if(index >= 0 && index < m_aspirations.size())
        m_aspirations[index]->turnOn();

    if(m_logger)
    {
        QString name = index == 0 ? "АУ-1" : "АУ-2";

        m_logger->log(
            QString("Аспирационная установка %1 включена").arg(name)
            );
    }
}

void ProcessWidget::aspTurnOff(int index)
{
    if(index >= 0 && index < m_aspirations.size())
        m_aspirations[index]->turnOff();

    if(m_logger)
    {
        QString name = index == 0 ? "АУ-1" : "АУ-2";

        m_logger->log(
            QString("Аспирационная установка %1 отключена").arg(name)
            );
    }
}

void ProcessWidget::aspSetMode(int index, AspirationMode mode)
{
    if(index >= 0 && index < m_aspirations.size())
        m_aspirations[index]->setMode(mode);

    if(m_logger)
    {
        QString name = index == 0 ? "АУ-1" : "АУ-2";
        QString modeName = mode == AspirationMode::Auto ? "автоматический" : "ручной";

        m_logger->log(
            QString("Установлен %1 режим для аспирационной установки %2").arg(modeName, name)
            );
    }
}

void ProcessWidget::aspRequestCleaning(int index)
{
    if(index >= 0 && index < m_aspirations.size())
        m_aspirations[index]->requestCleaning();
}

void ProcessWidget::setLogger(EventLogger *logger)
{
    m_logger = logger;
}
