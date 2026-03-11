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
            if(m_motors[i]->state() == ProcessState::Running)
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
            if(m_motors[i]->state() == ProcessState::PreStart)
            {
                prestart = true;
                break;
            }
        }
        if(prestart)
            m_lampOn = !m_lampOn;

        m_plowBlink = !m_plowBlink;
        m_arrowBlink = !m_arrowBlink;

        update();
    });

    m_animationTimer.start(80);

    for(int i = 0; i < CONVEYOR_COUNT; i++)
    {
        Motor* motor = new Motor(this);

        connect(motor, &Motor::stateChanged,
                this,
                [this, i](ProcessState state)
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
        m_motors[index]->state() == ProcessState::Running ? Qt::red : Qt::green;

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

    if (m_motors[index]->state() == ProcessState::Running)
        lampColor = Qt::yellow;

    else if (m_motors[index]->state() == ProcessState::PreStart)
        lampColor = m_lampOn ? Qt::red : QColor(100,0,0);

    else
        lampColor = QColor(60,60,60);

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

    if (m_motors[index]->state() == ProcessState::Running)
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

    if (m_motors[motorIndex]->state() == ProcessState::Running &&
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

    drawDamper(p, chute, mirror, damperName);
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

void ProcessWidget::drawDamper(QPainter& p, const QRect& chuteRect, bool mirror, const QString& name)
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

    // труба
    p.setBrush(QColor(0,100,0));
    p.setPen(Qt::black);
    p.drawRect(pipe);

    // заслонка
    QPen damperPen(QColor(0,60,0), 5);
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

    // левая установка
    drawAspiration(p, m_pipeEnds[0].x() - 110, y, "АУ-1");

    QFont font = p.font();
    font.setPointSize(10);
    font.setBold(true);
    p.setFont(font);
    p.setPen(Qt::black);

    // правая установка
    drawAspiration(p, m_pipeEnds[9].x() + 110, y, "АУ-2");
}

void ProcessWidget::drawAspiration(QPainter& p, int x, int y, const QString& name)
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

    // вентилятор
    QPoint c = body.center();

    QPolygon fan;
    fan << QPoint(c.x(), c.y() - 18)
        << QPoint(c.x() - 12, c.y() + 12)
        << QPoint(c.x() + 12, c.y() + 12);

    p.setBrush(Qt::black);
    p.drawPolygon(fan);

    // подпись
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

                m_selectedPlow = index;
                emit plowClicked(index);
            }
        }
    }
}

void ProcessWidget::setRunning(int index, bool running)
{
    if(index < 0 || index >= m_motors.size())
        return;

    if (running)
        m_motors[index]->start();
    else
        m_motors[index]->stop();
}

void ProcessWidget::setTargetSpeed(int index, int speed)
{
    if(index < 0 || index >= m_motors.size())
        return;

    m_motors[index]->setTargetSpeed(speed);
}

void ProcessWidget::plowLower()
{
    if(m_selectedPlow >= 0)
        m_plows[m_selectedPlow]->lower();
}

void ProcessWidget::plowRaise()
{
    if(m_selectedPlow >= 0)
        m_plows[m_selectedPlow]->raise();
}

void ProcessWidget::plowStop()
{
    if(m_selectedPlow >= 0)
        m_plows[m_selectedPlow]->stop();
}
