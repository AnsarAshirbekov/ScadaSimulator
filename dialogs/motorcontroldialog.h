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
    explicit MotorControlDialog(const QString& name, QWidget *parent = nullptr);
    void updateState(ProcessState state);
    void setSpeed(int speed);

signals:
    void startRequested();
    void stopRequested();
    void speedRequested(int speed);

private:
    QString m_equipmentName;
    QLabel *titleLabel;
    QLabel *lblState;
    QSlider *speedSlider;
    QLCDNumber *speedDisplay;
};

#endif // MOTORCONTROLDIALOG_H
