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
#include "dialogs/aspcontroldialog.h"
#include "events/eventlogger.h"
#include "dialogs/eventlogdialog.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    showMaximized();

    QVBoxLayout *mainLayout = new QVBoxLayout();

    ProcessWidget *process = new ProcessWidget();

    EventLogger *logger = new EventLogger(this);

    process->setLogger(logger);

    QPushButton* logButton = new QPushButton();
    logButton->setFixedSize(40, 40);
    logButton->setStyleSheet(R"(
        QPushButton {
            background-color: #444;
            border-radius: 10px;
        }
        QPushButton:hover {
            background-color: #666;
        }
    )");
    logButton->setIcon(QIcon(":/icons/icons/log.png"));
    logButton->setIconSize(QSize(24,24));
    logButton->setToolTip("Журнал событий");

    QLabel* title = new QLabel("Галерея ленточных конвейеров");
    title->setAlignment(Qt::AlignCenter);
    title->setStyleSheet(R"(
        QLabel {
            color: black;
            font-size: 24px;
            font-weight: bold;
            letter-spacing: 1px;
        }
    )");

    QHBoxLayout *headerLayout = new QHBoxLayout();
    QWidget *leftSpacer = new QWidget();
    leftSpacer->setFixedWidth(80);
    headerLayout->addWidget(leftSpacer);
    headerLayout->addStretch();
    headerLayout->addWidget(title);
    headerLayout->addStretch();
    headerLayout->addWidget(logButton);
    headerLayout->setContentsMargins(10, 5, 30, 0);

    mainLayout->addLayout(headerLayout);
    mainLayout->addWidget(process, 1);
    mainLayout->setContentsMargins(0, 0, 0, 40);
    mainLayout->setSpacing(5);

    QHBoxLayout *sensorLayout = new QHBoxLayout();

    QPushButton* kslAButton = new QPushButton();
    kslAButton->setFixedSize(45, 45);
    kslAButton->setCheckable(true);
    kslAButton->setStyleSheet(R"(
    QPushButton {
        background-color: #444;
        border-radius: 10px;
        }
        QPushButton:hover {
            background-color: #666;
        }
        QPushButton:checked {
            background-color: red;
            border: 2px solid #ff5555;
        }
    )");
    kslAButton->setIcon(QIcon(":/icons/icons/ksl.png"));
    kslAButton->setIconSize(QSize(24,24));
    kslAButton->setToolTip("КСЛ-А (сход ленты)");

    QPushButton* kslBButton = new QPushButton();
    kslBButton->setFixedSize(45, 45);
    kslBButton->setCheckable(true);
    kslBButton->setStyleSheet(R"(
        QPushButton {
            background-color: #444;
            border-radius: 10px;
        }
        QPushButton:hover {
            background-color: #666;
        }
        QPushButton:checked {
            background-color: red;
            border: 2px solid #ff5555;
        }
    )");
    kslBButton->setIcon(QIcon(":/icons/icons/ksl.png"));
    kslBButton->setIconSize(QSize(24,24));
    kslBButton->setToolTip("КСЛ-Б (сход ленты)");

    QPushButton* cleanAU1Button = new QPushButton();
    cleanAU1Button->setFixedSize(45, 45);
    cleanAU1Button->setCheckable(true);
    cleanAU1Button->setStyleSheet(R"(
        QPushButton {
            background-color: #444;
            border-radius: 10px;
        }
        QPushButton:hover {
            background-color: #666;
        }
    )");
    cleanAU1Button->setIcon(QIcon(":/icons/icons/clean.png"));
    cleanAU1Button->setIconSize(QSize(24,24));
    cleanAU1Button->setToolTip("Очистка АУ-1");

    QPushButton* cleanAU2Button = new QPushButton();
    cleanAU2Button->setFixedSize(45, 45);
    cleanAU2Button->setCheckable(true);
    cleanAU2Button->setStyleSheet(R"(
        QPushButton {
            background-color: #444;
            border-radius: 10px;
        }
        QPushButton:hover {
            background-color: #666;
        }
    )");
    cleanAU2Button->setIcon(QIcon(":/icons/icons/clean.png"));
    cleanAU2Button->setIconSize(QSize(24,24));
    cleanAU2Button->setToolTip("Очистка АУ-2");

    sensorLayout->addStretch();
    sensorLayout->addWidget(kslAButton);
    sensorLayout->addSpacing(20);
    sensorLayout->addWidget(kslBButton);
    sensorLayout->addSpacing(40);
    sensorLayout->addWidget(cleanAU1Button);
    sensorLayout->addSpacing(20);
    sensorLayout->addWidget(cleanAU2Button);
    sensorLayout->addStretch();

    mainLayout->addLayout(sensorLayout);

    connect(logButton, &QPushButton::clicked, this, [this, logger]()
            {
                EventLogDialog* dialog = new EventLogDialog(logger, this);
                dialog->show();
            });

    QVector<MotorControlDialog*> motorDialogs;

    QStringList conveyorNames = {"ЛК-А", "ЛК-Б"};

    for(int i = 0; i < conveyorNames.size(); i++)
    {
        motorDialogs.append(new MotorControlDialog(i, conveyorNames[i], this));
    }

    QVector<PlowControlDialog*> plowDialogs;

    for(int i = 0; i < 20; i++)
    {
        QString name = QString("ПС-%1%2")
                           .arg(i < 10 ? "А" : "Б")
                           .arg(i % 10 + 1);

        plowDialogs.append(new PlowControlDialog(i, name, this));
    }

    QVector<DamperControlDialog*> damperDialogs;

    for(int i = 0; i < 20; i++)
    {
        QString name = QString("З-%1%2")
                           .arg(i < 10 ? "А" : "Б")
                           .arg(i % 10 + 1);

        damperDialogs.append(new DamperControlDialog(i, name, this));
    }

    QVector<AspControlDialog*> aspDialogs;

    QStringList aspNames = {"АУ-1", "АУ-2"};

    for(int i = 0; i < aspNames.size(); i++)
    {
        aspDialogs.append(new AspControlDialog(i, aspNames[i], this));
    }

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
                [process](int index)
                {
                    process->motorStart(index);
                });

        connect(dialog, &MotorControlDialog::stopRequested,
                process,
                [process](int index)
                {
                    process->motorStop(index);
                });

        connect(dialog, &MotorControlDialog::speedRequested,
                process,
                [process](int index, int speed)
                {
                    process->setTargetSpeed(index, speed);
                });

        connect(dialog, &MotorControlDialog::resetRequested,
                process,
                [process](int index)
                {
                    process->motorReset(index);
                });
    }

    connect(process, &ProcessWidget::stateChanged,
            this,
            [motorDialogs](int index, MotorState state)
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
                process,
                [process](int index)
                {
                    process->plowLower(index);
                });

        connect(dialog, &PlowControlDialog::raiseRequested,
                process,
                [process](int index)
                {
                    process->plowRaise(index);
                });

        connect(dialog, &PlowControlDialog::stopRequested,
                process,
                [process](int index)
                {
                    process->plowStop(index);
                });

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
        DamperControlDialog* dialog = damperDialogs[i];

        connect(dialog, &DamperControlDialog::openRequested,
                process,
                [process](int index)
                {
                    process->damperOpen(index);
                });

        connect(dialog, &DamperControlDialog::closeRequested,
                process,
                [process](int index)
                {
                    process->damperClose(index);
                });

        connect(dialog, &DamperControlDialog::modeChanged,
                process, [process](int index, DamperMode mode)
                {
                    process->damperSetMode(index, mode);
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

    connect(process, &ProcessWidget::aspClicked,
            this, [aspDialogs](int index)
            {
                aspDialogs[index]->show();
            });

    for(int i = 0; i < aspDialogs.size(); i++)
    {
        AspControlDialog* dialog = aspDialogs[i];

        connect(dialog, &AspControlDialog::turnOnRequested,
                process,
                [process](int index)
                {
                    process->aspTurnOn(index);
                });

        connect(dialog, &AspControlDialog::turnOffRequested,
                process,
                [process](int index)
                {
                    process->aspTurnOff(index);
                });

        connect(dialog, &AspControlDialog::modeChanged,
                process, [process](int index, AspirationMode mode)
                {
                    process->aspSetMode(index, mode);
                });

        connect(process, &ProcessWidget::aspStateChanged,
                dialog,
                [dialog, i](int index, AspirationState state)
                {
                    if(index == i)
                        dialog->setState(state);
                });

        connect(process, &ProcessWidget::aspModeChanged,
                dialog,
                [dialog, i](int index, AspirationMode mode)
                {
                    if(index == i)
                        dialog->setMode(mode);
                });
    }

    connect(kslAButton, &QPushButton::toggled, this,
            [this, process, logger](bool checked)
    {
        if(checked)
        {
            process->motorFault(0);

            if(logger)
                logger->log("АВАРИЯ: сход ленты конвейера ЛК-А", true);
        } else {
            process->openKSL(0);
            if(logger)
                logger->log("Сход ленты конвейера ЛК-А в норме");
        }
    });

    connect(kslBButton, &QPushButton::toggled, this,
            [this, process, logger](bool checked)
    {
        if(checked)
        {
            process->motorFault(1);

            if(logger)
                logger->log("АВАРИЯ: сход ленты конвейера ЛК-Б", true);
        } else {
            process->openKSL(1);
            if(logger)
                logger->log("Сход ленты конвейера ЛК-Б в норме");
        }
    });

    connect(cleanAU1Button, &QPushButton::clicked, this,
            [process, logger, cleanAU1Button]()
    {
        if(cleanAU1Button->property("active").toBool())
            return;

        cleanAU1Button->setProperty("active", true);

        cleanAU1Button->setStyleSheet(R"(
            QPushButton {
                background-color: blue;
                border-radius: 10px;
                border: 2px solid #ff5555;
            }
        )");

        process->aspRequestCleaning(0);

        if(logger)
            logger->log("Запрос очистки АУ-1");
    });

    connect(cleanAU2Button, &QPushButton::clicked, this,
            [process, logger, cleanAU2Button]()
    {
        if(cleanAU2Button->property("active").toBool())
            return;

        cleanAU2Button->setProperty("active", true);

        cleanAU2Button->setStyleSheet(R"(
            QPushButton {
                background-color: blue;
                border-radius: 10px;
                border: 2px solid #ff5555;
            }
        )");

        process->aspRequestCleaning(1);

        if(logger)
            logger->log("Запрос очистки АУ-2");
    });

    connect(process, &ProcessWidget::cleaningFinished,
            this, [cleanAU1Button, cleanAU2Button, logger](int index)
    {
        QPushButton* btn = (index == 0) ? cleanAU1Button : cleanAU2Button;

        btn->setProperty("active", false);

        btn->setStyleSheet(R"(
            QPushButton {
                background-color: #444;
                border-radius: 10px;
            }
            QPushButton:hover {
                background-color: #666;
            }
        )");

        if(logger)
        {
            QString name = (index == 0) ? "АУ-1" : "АУ-2";
            logger->log(QString("Процесс очистки %1 завершен").arg(name));
        }
    });
}

MainWindow::~MainWindow()
{
    delete ui;
}
