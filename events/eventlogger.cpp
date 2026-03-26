#include "events/eventlogger.h"

EventLogger::EventLogger(QObject *parent)
    : QObject(parent)
{
}

void EventLogger::log(const QString &text, bool isError)
{
    QString formatted;

    if(isError)
    {
        formatted = QString("<span style='color:red;'>%1</span>").arg(text);
    } else {
        formatted = text;
    }

    QString record =
        QDateTime::currentDateTime().toString("[hh:mm:ss] ") + formatted;

    m_events.append(record);

    emit newEvent(record);
}

const QVector<QString>& EventLogger::events() const
{
    return m_events;
}
