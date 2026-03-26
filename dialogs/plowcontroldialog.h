#ifndef PLOWCONTROLDIALOG_H
#define PLOWCONTROLDIALOG_H

#include <QDialog>
#include "widgets/processwidget.h"

class QPushButton;
class QLabel;
class QLCDNumber;

class PlowControlDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PlowControlDialog(int index, const QString& name, QWidget *parent = nullptr);
    void setPosition(int percent);
    void updateState(PlowState state);

signals:
    void raiseRequested(int index);
    void lowerRequested(int index);
    void stopRequested(int index);

private:
    int m_index;
    QString m_plowName;
    QLabel *titleLabel;
    QLabel *lblState;
    QPushButton *btnRaise;
    QPushButton *btnLower;
    QPushButton *btnStop;
    QLCDNumber *positionDisplay;
};

#endif // PLOWCONTROLDIALOG_H
