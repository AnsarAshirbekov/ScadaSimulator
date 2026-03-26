#include "dialogs/aspcontroldialog.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QCheckBox>

AspControlDialog::AspControlDialog(int index, const QString &name, QWidget *parent)
    : QDialog(parent), m_index(index)
{
    setWindowTitle("Пульт аспирационной установки");
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    m_nameLabel = new QLabel("Управление аспирационной установкой " + name);
    m_nameLabel->setAlignment(Qt::AlignCenter);

    m_stateLabel = new QLabel("Отключена");
    m_stateLabel->setAlignment(Qt::AlignCenter);

    setStyleSheet("background-color: #2c2c2c; color: white;");

    m_modeSwitch = new ToggleSwitch();

    QHBoxLayout *modeLayout = new QHBoxLayout();

    modeLayout->addWidget(new QLabel("Автомат"));
    modeLayout->addStretch();
    modeLayout->addWidget(m_modeSwitch);
    modeLayout->addStretch();
    modeLayout->addWidget(new QLabel("Ручной"));
    modeLayout->setContentsMargins(20, 0, 20, 0);

    m_turnOnButton = new QPushButton("Включить");
    m_turnOffButton = new QPushButton("Отключить");
    m_turnOnButton->setStyleSheet("background-color: red; color: white;");
    m_turnOffButton->setStyleSheet("background-color: green; color: white;");

    QVBoxLayout *layout = new QVBoxLayout();
    layout->addWidget(m_nameLabel);
    layout->addWidget(m_stateLabel);
    layout->addLayout(modeLayout);
    layout->addWidget(m_turnOnButton);
    layout->addWidget(m_turnOffButton);
    setLayout(layout);

    connect(m_turnOnButton, QPushButton::clicked, this, [this](){
        emit turnOnRequested(m_index);
    });

    connect(m_turnOffButton, QPushButton::clicked, this, [this](){
        emit turnOffRequested(m_index);
    });

    connect(m_modeSwitch, &QCheckBox::toggled,
            this, [this](bool checked)
            {
                emit modeChanged(m_index, checked ? AspirationMode::Manual : AspirationMode::Auto);
            });
}

void AspControlDialog::setState(AspirationState state)
{
    switch(state)
    {
        case AspirationState::On:
            m_stateLabel->setText("Включена");
            break;

        case AspirationState::Off:
            m_stateLabel->setText("Отключена");
            break;

        case AspirationState::Cleaning:
            m_stateLabel->setText("Режим очистки");
            break;
    }
}

void AspControlDialog::setMode(AspirationMode mode)
{
    m_modeSwitch->blockSignals(true);

    bool manual = mode == AspirationMode::Manual;
    m_modeSwitch->setChecked(manual);

    m_modeSwitch->blockSignals(false);

    m_turnOnButton->setEnabled(manual);
    m_turnOffButton->setEnabled(manual);
}
