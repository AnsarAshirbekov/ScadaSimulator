#include "dialogs/dampercontroldialog.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QCheckBox>

DamperControlDialog::DamperControlDialog(int index, const QString &name, QWidget *parent)
    : QDialog(parent), m_index(index)
{
    setWindowTitle("Пульт заслонки");
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    m_nameLabel = new QLabel("Управление заслонкой " + name);
    m_nameLabel->setAlignment(Qt::AlignCenter);

    m_stateLabel = new QLabel("Закрыто");
    m_stateLabel->setAlignment(Qt::AlignCenter);

    setStyleSheet("background-color: #2c2c2c; color: white;");

    m_modeSwitch = new ToggleSwitch();

    QHBoxLayout *modeLayout = new QHBoxLayout();

    modeLayout->addWidget(new QLabel("Автомат"));
    modeLayout->addWidget(m_modeSwitch);
    modeLayout->addWidget(new QLabel("Ручной"));

    m_openButton = new QPushButton("Открыть");
    m_closeButton = new QPushButton("Закрыть");
    m_openButton->setStyleSheet("background-color: green; color: white;");
    m_closeButton->setStyleSheet("background-color: red; color: white;");

    QVBoxLayout *layout = new QVBoxLayout();
    layout->addWidget(m_nameLabel);
    layout->addWidget(m_stateLabel);
    layout->addLayout(modeLayout);
    layout->addWidget(m_openButton);
    layout->addWidget(m_closeButton);
    setLayout(layout);

    connect(m_openButton, &QPushButton::clicked, this, [this](){
        emit openRequested(m_index);
    });

    connect(m_closeButton, &QPushButton::clicked, this, [this](){
        emit closeRequested(m_index);
    });

    connect(m_modeSwitch, &QCheckBox::toggled,
            this, [this](bool checked)
            {
                emit modeChanged(m_index, checked ? DamperMode::Manual : DamperMode::Auto);
            });
}

void DamperControlDialog::setState(DamperState state)
{
    m_stateLabel->setText(state == DamperState::Open ? "Открыто" : "Закрыто");
}

void DamperControlDialog::setMode(DamperMode mode)
{
    m_modeSwitch->blockSignals(true);

    bool manual = mode == DamperMode::Manual;
    m_modeSwitch->setChecked(manual);

    m_modeSwitch->blockSignals(false);

    m_openButton->setEnabled(manual);
    m_closeButton->setEnabled(manual);
}
