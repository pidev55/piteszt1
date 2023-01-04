#ifndef RESTARTER_H
#define RESTARTER_H

#include <QObject>
//#include <QApplication>
//#include <QAbstractEventDispatcher>


class Restarter : public QObject
{
    Q_OBJECT
private:    
    bool _isRestart;
public:
    enum Mode:int{Restart=55, Shutdown};
    Restarter();
    void restart() { emit restartSignal(Mode::Restart);}
    void shutdown() { emit restartSignal(Mode::Shutdown);}
    //int returnValue();
signals:
    void restartSignal(Mode m);
private slots:
    void restart2(Mode m);
};

#endif // RESTARTER_H
