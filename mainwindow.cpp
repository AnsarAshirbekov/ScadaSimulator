#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include <QDebug>
#include "widgets/processwidget.h"
#include "dialogs/motorcontroldialog.h"
#include "dialogs/plowcontroldialog.h"
#include "dialogs/dampercontroldialog.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    showMaximized();

    QVBoxLayout *mainLayout = new QVBoxLayout();

    ProcessWidget *process = new ProcessWidget();

    mainLayout->addWidget(process);

    QVector<MotorControlDialog*> motorDialogs;

    QStringList conveyorNames = {"ЛК-А", "ЛК-Б"};

    for(const QString& name : conveyorNames)
    {
        motorDialogs.append(new MotorControlDialog(name, this));
    }

    QVector<PlowControlDialog*> plowDialogs;

    for(int i = 1; i <= 10; i++)
    {
        plowDialogs.append(new PlowControlDialog(QString("ПС-А%1").arg(i), this));
    }

    for(int i = 1; i <= 10; i++)
    {
        plowDialogs.append(new PlowControlDialog(QString("ПС-Б%1").arg(i), this));
    }

    QVector<DamperControlDialog*> damperDialogs;

    for(int i = 1; i <= 10; i++)
        damperDialogs.append(new DamperControlDialog(QString("З-А%1").arg(i), this));

    for(int i = 1; i <= 10; i++)
        damperDialogs.append(new DamperControlDialog(QString("З-Б%1").arg(i), this));

    ui->centralwidget->setLayout(mainLayout);

    // когда клик по мотору - открыть пульт
    connect(process, &ProcessWidget::motorClicked, this, [this, motorDialogs](int index) {
        motorDialogs[index]->show();
    });

    // сигналы управления
    for(int i = 0; i < motorDialogs.size(); i++)
    {
        MotorControlDialog* dialog = motorDialogs[i];

        connect(dialog, &MotorControlDialog::startRequested,
                process,
                [process, i]()
                {
                    process->setRunning(i, true);
                });

        connect(dialog, &MotorControlDialog::stopRequested,
                process,
                [process, i]()
                {
                    process->setRunning(i, false);
                });

        connect(dialog, &MotorControlDialog::speedRequested,
                process,
                [process, i](int speed)
                {
                    process->setTargetSpeed(i, speed);
                });
    }

    connect(process, &ProcessWidget::stateChanged,
            this,
            [motorDialogs](int index, ProcessState state)
            {
                motorDialogs[index]->updateState(state);
            });

    connect(process, &ProcessWidget::speedChanged,
            this,
            [motorDialogs](int index, int speed)
            {
                motorDialogs[index]->setSpeed(speed);
            });


    connect(process, &ProcessWidget::plowClicked,
            this, [this, plowDialogs](int index) {
                plowDialogs[index]->show();
            });

    for(int i = 0; i < plowDialogs.size(); i++)
    {
        PlowControlDialog* dialog = plowDialogs[i];

        connect(dialog, &PlowControlDialog::lowerRequested,
                process, &ProcessWidget::plowLower);

        connect(dialog, &PlowControlDialog::raiseRequested,
                process, &ProcessWidget::plowRaise);

        connect(dialog, &PlowControlDialog::stopRequested,
                process, &ProcessWidget::plowStop);

        connect(process, &ProcessWidget::plowPositionChanged,
                dialog,
                [dialog, i](int index, int percent)
                {
                    if(index == i)
                        dialog->setPosition(percent);
                });

        connect(process, &ProcessWidget::plowStateChanged,
                dialog,
                [dialog, i](int index, PlowState state)
                {
                    if(index == i)
                        dialog->updateState(state);
                });
    }

    connect(process, &ProcessWidget::damperClicked,
            this, [damperDialogs](int index)
            {
                damperDialogs[index]->show();
            });

    for(int i = 0; i < damperDialogs.size(); i++)
    {
        auto dialog = damperDialogs[i];

        connect(dialog, &DamperControlDialog::openRequested,
                process, &ProcessWidget::damperOpen);

        connect(dialog, &DamperControlDialog::closeRequested,
                process, &ProcessWidget::damperClose);

        connect(dialog, &DamperControlDialog::modeChanged,
                process, [process, i](DamperMode mode)
                {
                    process->damperSetMode(i, mode);
                });

        connect(process, &ProcessWidget::damperStateChanged,
                this,
                [dialog, i](int index, DamperState state)
                {
                    if(index == i)
                        dialog->setState(state);
                });

        connect(process, &ProcessWidget::damperModeChanged,
                this,
                [dialog, i](int index, DamperMode mode)
                {
                    if(index == i)
                        dialog->setMode(mode);
                });
    }

}

MainWindow::~MainWindow()
{
    delete ui;
}
