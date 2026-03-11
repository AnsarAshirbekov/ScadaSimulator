#include "plowcontroldialog.h"
#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QLCDNumber>

PlowControlDialog::PlowControlDialog(const QString& name, QWidget *parent)
    : QDialog(parent), m_plowName(name)
{
    setWindowTitle("Плужковый сбрасыватель");
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    titleLabel = new QLabel("Пульт управления " + m_plowName);
    titleLabel->setAlignment(Qt::AlignCenter);

    setStyleSheet("background-color: #2c2c2c; color: white;");

    lblState = new QLabel();
    lblState->setAlignment(Qt::AlignCenter);

    positionDisplay = new QLCDNumber();
    positionDisplay->display(0);
    positionDisplay->setStyleSheet(R"(
        QLCDNumber {
            background-color: black;
            color: lime;
            border: 2px solid #00bfff;
            border-radius: 8px;
        }
    )");
    positionDisplay->setFixedHeight(50);
    positionDisplay->setSegmentStyle(QLCDNumber::Flat);

    btnRaise = new QPushButton("Поднять");
    btnLower = new QPushButton("Опустить");
    btnStop = new QPushButton("Остановить");
    btnRaise->setStyleSheet("background-color: green; color: white;");
    btnLower->setStyleSheet("background-color: red; color: white;");
    btnStop->setStyleSheet("background-color: gray; color: white;");

    QVBoxLayout *layout = new QVBoxLayout();

    layout->addWidget(titleLabel);
    layout->addWidget(lblState);
    layout->addWidget(new QLabel("Опущен на (%)"));
    layout->addWidget(positionDisplay);
    layout->addWidget(btnRaise);
    layout->addWidget(btnLower);
    layout->addWidget(btnStop);

    setLayout(layout);

    connect(btnRaise,&QPushButton::clicked,this,[this](){
        emit raiseRequested();
    });

    connect(btnLower,&QPushButton::clicked,this,[this](){
        emit lowerRequested();
    });

    connect(btnStop,&QPushButton::clicked,this,[this](){
        emit stopRequested();
    });

    updateState(PlowState::Raised);
    positionDisplay->display(0);
}

void PlowControlDialog::setPosition(int percent)
{
    positionDisplay->display(percent);
}

void PlowControlDialog::updateState(PlowState state)
{
    switch(state)
    {
        case PlowState::Raised:
            btnStop->setEnabled(false);
            btnRaise->setEnabled(false);
            btnLower->setEnabled(true);
            lblState->setText("Поднят");
            break;

        case PlowState::Lowered:
            btnStop->setEnabled(false);
            btnRaise->setEnabled(true);
            btnLower->setEnabled(false);
            lblState->setText("Опущен");
            break;

        case PlowState::Raising:
            btnStop->setEnabled(true);
            btnRaise->setEnabled(true);
            btnLower->setEnabled(true);
            lblState->setText("Поднятие..");
            break;

        case PlowState::Lowering:
            btnStop->setEnabled(true);
            btnRaise->setEnabled(true);
            btnLower->setEnabled(true);
            lblState->setText("Опускание..");
            break;

        case PlowState::Stopped:
            btnStop->setEnabled(false);
            btnRaise->setEnabled(true);
            btnLower->setEnabled(true);
            lblState->setText("Остановлен");
            break;
    }
}
