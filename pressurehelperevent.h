#ifndef RECORDWORKEREVENT_H
#define RECORDWORKEREVENT_H

#include <QObject>

class PressureHelperEvent : public QObject
{
    Q_OBJECT
public:
    explicit PressureHelperEvent(QObject *parent = nullptr);

    Q_SIGNAL void started();
    Q_SIGNAL void stopped();
    Q_SIGNAL void message(const QString& msg);
    Q_SIGNAL void newframe(const QString&);
};

#endif // RECORDWORKEREVENT_H
