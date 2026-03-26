#ifndef EVENTLOGDIALOG_H
#define EVENTLOGDIALOG_H

#include <QDialog>

class QTextEdit;
class EventLogger;

class EventLogDialog : public QDialog
{
    Q_OBJECT

public:
    explicit EventLogDialog(EventLogger* logger, QWidget *parent = nullptr);

private:
    QTextEdit* m_logView;
    EventLogger* m_logger;
};

#endif // EVENTLOGDIALOG_H
