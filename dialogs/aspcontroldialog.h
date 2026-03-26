#ifndef ASPCONTROLDIALOG_H
#define ASPCONTROLDIALOG_H

#include <QDialog>
#include <QCheckBox>
#include "equipment/aspiration.h"
#include "widgets/toggleswitch.h"

class QLabel;
class QPushButton;
class QCheckBox;

class AspControlDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AspControlDialog(int index, const QString& name, QWidget *parent = nullptr);

    void setState(AspirationState state);
    void setMode(AspirationMode mode);

signals:
    void turnOnRequested(int index);
    void turnOffRequested(int index);
    void modeChanged(int index, AspirationMode mode);

private:
    int m_index;
    QLabel *m_nameLabel;
    QLabel *m_stateLabel;
    QCheckBox *m_modeSwitch;
    QPushButton *m_turnOnButton;
    QPushButton *m_turnOffButton;
};

#endif // ASPCONTROLDIALOG_H
