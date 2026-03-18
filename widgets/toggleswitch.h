#ifndef TOGGLESWITCH_H
#define TOGGLESWITCH_H

#include <QCheckBox>
#include <QPropertyAnimation>

class ToggleSwitch : public QCheckBox
{
    Q_OBJECT
    Q_PROPERTY(int offset READ offset WRITE setOffset)

public:
    explicit ToggleSwitch(QWidget *parent = nullptr);

    int offset() const { return m_offset; }
    void setOffset(int value);

protected:
    void paintEvent(QPaintEvent *) override;
    void mousePressEvent(QMouseEvent *event) override;

private:
    int m_offset = 0;
    QPropertyAnimation *m_anim;
};

#endif // TOGGLESWITCH_H
