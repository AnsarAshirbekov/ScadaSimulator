#include "motorcontroldialog.h"
#include <QPushButton>
#include <QVBoxLayout>
#include <QLabel>
#include "widgets/processwidget.h"
#include <QSlider>
#include <QLCDNumber>

MotorControlDialog::MotorControlDialog(int index, const QString& name, QWidget *parent)
    : QDialog(parent), m_index(index), m_equipmentName(name)
{
    setWindowTitle("Пульт конвейера");
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    titleLabel = new QLabel("Управление конвейером " + m_equipmentName);
    titleLabel->setAlignment(Qt::AlignCenter);

    setStyleSheet("background-color: #2c2c2c; color: white;");

    lblState = new QLabel();
    lblState->setAlignment(Qt::AlignCenter);

    btnStart = new QPushButton("ПУСК");
    btnStop = new QPushButton("СТОП");
    btnReset = new QPushButton("СБРОС АВАРИИ");
    btnStart->setStyleSheet("background-color: green; color: white;");
    btnStop->setStyleSheet("background-color: red; color: white;");
    btnReset->setStyleSheet("background-color: orange; color: black;");

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
    layout->addWidget(btnReset);
    layout->addWidget(new QLabel("Скорость (%)"));
    layout->addWidget(speedSlider);
    layout->addWidget(speedDisplay);

    setLayout(layout);

    connect(btnStart, &QPushButton::clicked, this, [this](){
        emit startRequested(m_index);
    });

    connect(btnStop, &QPushButton::clicked, this, [this](){
        emit stopRequested(m_index);
    });

    connect(speedSlider, &QSlider::valueChanged,
            speedDisplay, QOverload<int>::of(&QLCDNumber::display));

    connect(speedSlider, &QSlider::valueChanged, this, [this](int value){
        emit speedRequested(m_index, value);
    });

    connect(btnReset, &QPushButton::clicked, this, [this](){
        emit resetRequested(m_index);
    });

    updateState(MotorState::Stopped);
}

void MotorControlDialog::updateState(MotorState state)
{
    switch (state)
    {
        case MotorState::Stopped:
            lblState->setText("Остановлен");
            btnStart->setEnabled(true);
            btnStop->setEnabled(false);
            btnReset->setEnabled(false);
            speedSlider->setEnabled(false);
            speedSlider->setValue(10); // Сброс в минимум
            speedDisplay->display(speedSlider->value());
            break;

        case MotorState::PreStart:
            lblState->setText("Предпусковая сигнализация..");
            btnStart->setEnabled(false);
            btnStop->setEnabled(true);
            btnReset->setEnabled(false);
            speedSlider->setEnabled(false);
            break;

        case MotorState::Running:
            lblState->setText("В работе");
            btnStart->setEnabled(false);
            btnStop->setEnabled(true);
            btnReset->setEnabled(false);
            speedSlider->setEnabled(true);
            break;

        case MotorState::Fault:
            lblState->setText("АВАРИЯ (КСЛ)");
            btnStart->setEnabled(false);
            btnStop->setEnabled(false);
            btnReset->setEnabled(true);
            speedSlider->setEnabled(false);
            break;
    }
}

void MotorControlDialog::setSpeed(int speed)
{
    speedDisplay->display(speed);
}
