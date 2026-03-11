#include "motorcontroldialog.h"
#include <QPushButton>
#include <QVBoxLayout>
#include <QLabel>
#include "widgets/processwidget.h"
#include <QSlider>
#include <QLCDNumber>

MotorControlDialog::MotorControlDialog(const QString& name, QWidget *parent)
    : QDialog(parent), m_equipmentName(name)
{
    setWindowTitle("Пульт управления конвейером");
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    titleLabel = new QLabel("Пульт управления " + m_equipmentName);
    titleLabel->setAlignment(Qt::AlignCenter);

    setStyleSheet("background-color: #2c2c2c; color: white;");

    lblState = new QLabel();
    lblState->setAlignment(Qt::AlignCenter);

    QPushButton *btnStart = new QPushButton("ПУСК");
    QPushButton *btnStop = new QPushButton("СТОП");
    btnStart->setStyleSheet("background-color: green; color: white;");
    btnStop->setStyleSheet("background-color: red; color: white;");

    speedSlider = new QSlider(Qt::Horizontal);
    speedSlider->setRange(10, 100);
    speedSlider->setValue(10);
    speedSlider->setStyleSheet(R"(
        QSlider::groove:horizontal {
            height: 8px;
            background: #444;
            border-radius: 4px;
        }
        QSlider::handle:horizontal {
            background: #00bfff;
            width: 20px;
            margin: -6px 0;
            border-radius: 10px;
        }
        QSlider::sub-page:horizontal {
            background: #00bfff;
            border-radius: 4px;
        }
    )");

    speedDisplay = new QLCDNumber();
    speedDisplay->display(10);
    speedDisplay->setStyleSheet(R"(
        QLCDNumber {
            background-color: black;
            color: lime;
            border: 2px solid #00bfff;
            border-radius: 8px;
        }
    )");
    speedDisplay->setFixedHeight(50);
    speedDisplay->setSegmentStyle(QLCDNumber::Flat);

    QVBoxLayout *layout = new QVBoxLayout();
    layout->addWidget(titleLabel);
    layout->addWidget(lblState);
    layout->addWidget(btnStart);
    layout->addWidget(btnStop);
    layout->addWidget(new QLabel("Скорость (%)"));
    layout->addWidget(speedSlider);
    layout->addWidget(speedDisplay);

    setLayout(layout);

    connect(btnStart, &QPushButton::clicked, this, [this](){
        emit startRequested();
    });

    connect(btnStop, &QPushButton::clicked, this, [this](){
        emit stopRequested();
    });

    connect(speedSlider, &QSlider::valueChanged,
            speedDisplay, QOverload<int>::of(&QLCDNumber::display));

    connect(speedSlider, &QSlider::valueChanged, this, [this](int value){
        emit speedRequested(value);
    });

    updateState(ProcessState::Stopped);
}

void MotorControlDialog::updateState(ProcessState state)
{
    switch (state)
    {
        case ProcessState::Stopped:
            lblState->setText("Остановлен");
            speedSlider->setEnabled(false);
            speedSlider->setValue(10); // Сброс в минимум
            speedDisplay->display(speedSlider->value());
            break;

        case ProcessState::PreStart:
            lblState->setText(("Предпусковая сигнализация.."));
            speedSlider->setEnabled(false);
            break;

        case ProcessState::Running:
            lblState->setText("В работе");
            speedSlider->setEnabled(true);
            break;
    }
}

void MotorControlDialog::setSpeed(int speed)
{
    speedDisplay->display(speed);
}
