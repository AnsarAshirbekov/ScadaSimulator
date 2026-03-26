#ifndef EVENTLOGGER_H
#define EVENTLOGGER_H

#include <QObject>
#include <QVector>
#include <QString>
#include <QDateTime>

class EventLogger : public QObject
{
    Q_OBJECT

public:
    explicit EventLogger(QObject *parent = nullptr);

    void log(const QString& text, bool isError = false);
    const QVector<QString>& events() const;

signals:
    void newEvent(const QString& text);

private:
    QVector<QString> m_events;
};

#endif // EVENTLOGGER_H
