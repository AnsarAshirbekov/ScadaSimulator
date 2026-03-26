#ifndef MOTORCONTROLDIALOG_H
#define MOTORCONTROLDIALOG_H

#include <QDialog>
#include "widgets/processwidget.h"

class QPushButton;
class QLabel;
class QSlider;
class QLCDNumber;

class MotorControlDialog : public QDialog
{
    Q_OBJECT

public:
    explicit MotorControlDialog(int index, const QString& name, QWidget *parent = nullptr);
    void updateState(MotorState state);
    void setSpeed(int speed);

signals:
    void startRequested(int index);
    void stopRequested(int index);
    void speedRequested(int index, int speed);
    void resetRequested(int index);

private:
    int m_index;
    QPushButton *btnStart;
    QPushButton *btnStop;
    QPushButton *btnReset;
    QString m_equipmentName;
    QLabel *titleLabel;
    QLabel *lblState;
    QSlider *speedSlider;
    QLCDNumber *speedDisplay;
};

#endif // MOTORCONTROLDIALOG_H
