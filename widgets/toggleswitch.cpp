#include "toggleswitch.h"
#include <QPainter>
#include <QMouseEvent>

ToggleSwitch::ToggleSwitch(QWidget *parent)
    : QCheckBox(parent)
{
    setFixedSize(50,25);

    setCursor(Qt::PointingHandCursor);

    m_anim = new QPropertyAnimation(this,"offset",this);
    m_anim->setDuration(120);

    connect(this,&QCheckBox::toggled,this,[this](bool checked){
        m_anim->stop();
        m_anim->setStartValue(m_offset);
        m_anim->setEndValue(checked ? 25 : 2);
        m_anim->start();
    });
}

void ToggleSwitch::setOffset(int value)
{
    m_offset=value;
    update();
}

void ToggleSwitch::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    QColor bg = isChecked() ? QColor(0,191,255) : QColor(102,102,102);

    p.setBrush(bg);
    p.setPen(Qt::NoPen);
    p.drawRoundedRect(rect(),12,12);

    p.setBrush(Qt::white);
    p.drawEllipse(QRect(m_offset,2,21,21));
}

void ToggleSwitch::mousePressEvent(QMouseEvent *event)
{
    setChecked(!isChecked());
    QCheckBox::mousePressEvent(event);
}
