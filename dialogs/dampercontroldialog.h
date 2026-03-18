#ifndef DAMPERCONTROLDIALOG_H
#define DAMPERCONTROLDIALOG_H

#include <QDialog>
#include <QCheckBox>
#include "equipment/damper.h"
#include "widgets/toggleswitch.h"

class QLabel;
class QPushButton;
class QCheckBox;

class DamperControlDialog : public QDialog
{
    Q_OBJECT

public:
    explicit DamperControlDialog(const QString& name, QWidget *parent = nullptr);

    void setState(DamperState state);
    void setMode(DamperMode mode);

signals:
    void openRequested();
    void closeRequested();
    void modeChanged(DamperMode mode);

private:
    QLabel *m_nameLabel;
    QLabel *m_stateLabel;
    QCheckBox *m_modeSwitch;
    QPushButton *m_openButton;
    QPushButton *m_closeButton;
};

#endif // DAMPERCONTROLDIALOG_H
