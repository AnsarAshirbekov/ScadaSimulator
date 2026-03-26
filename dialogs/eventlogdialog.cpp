#include "dialogs/eventlogdialog.h"
#include "events/eventlogger.h"

#include <QVBoxLayout>
#include <QTextEdit>
#include <QScrollBar>

EventLogDialog::EventLogDialog(EventLogger *logger, QWidget *parent)
    : QDialog(parent), m_logger(logger)
{
    setWindowTitle("Журнал событий");
    resize(500, 400);

    setStyleSheet("background-color: #2c2c2c; color: white;");

    m_logView = new QTextEdit();
    m_logView->setReadOnly(true);

    QVBoxLayout* layout = new QVBoxLayout();
    layout->addWidget(m_logView);
    setLayout(layout);

    for(const QString& event : m_logger->events())
    {
        m_logView->append(event);
    }

    connect(m_logger, &EventLogger::newEvent,
            this, [this](const QString& text)
            {
                m_logView->append(text);

                m_logView->verticalScrollBar()->setValue(
                    m_logView->verticalScrollBar()->maximum()
                    );
            });
}
