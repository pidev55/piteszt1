//#define _XOPEN_SOURCE 700
//#define _POSIX_SOURCE
//#define _POSIX_C_SOURCE

#include "globals.h"
#include "mainwindow.h"
#include "httpthreadedserver.h"
#include "recordhelper.h"
//#include "zhttpserver.h"
//#include "zlog.h"
#include "request.h"
#include "actionresult.h"
#include "dowork.h"
//#include "zstringhelper.h"
#include <QApplication>
#ifdef RPI
#include <QTimer>
#include <wiringPi.h>
#endif
#include "pressurehelper.h"
#include "ztcpsocket.h"

#include "testhelper.h"
#include "common/logger/log.h"
//#include "shutdowner.h"
#include <QMetaType>
//#include <stdlib.h>
//#include <stdint.h>
//#include <stdio.h>
#include <csignal>
//#include <unistd.h>
#include "zudpsocket.h"
#include "command.h"
#include "doworkcmd.h"
#include "aliveworker.h"
#include "settings.h"
#include "restarter.h"
#include "updater.h"
#include "instance.h"
#include "buildnumber.h"

///home/anti/pizero/tools/arm-bcm2708/gcc-linaro-arm-linux-gnueabihf-raspbian/bin/arm-linux-gnueabihf-gcc
///
int i;
//zLog zlog;
//const QString inifn("settings.ini");
//const QString aliveAddressKey("AliveAddress");
//const QString alivePortKey("AlivePort");
//const QString aliveStringKey("AliveString");
HTTPThreadedServer *server = nullptr;
Restarter *restarter = nullptr;
Updater *updater = nullptr;
Settings settings;
Instance _instance;

volatile sig_atomic_t gSignalStatus;


//void ledsLow(){
//    digitalWrite(LED_ALIVE, LOW);
//    digitalWrite(LED_REC, LOW);
//    digitalWrite(LED_API, LOW);
//}

void signal_handler( int signalId )
{
    gSignalStatus = signalId;
    server->stop();
    //ledsLow();
    zInfo(QStringLiteral("EXIT: %1").arg(signalId));

    QApplication::exit(0);
}

void setShutDownSignal( int signalId )
{
    struct sigaction sa;
    sa.sa_flags = 0;
    sigemptyset(&sa.sa_mask);
    sa.sa_handler = signal_handler;
    if (sigaction(signalId, &sa, nullptr) == -1)
    {
        perror("setting up termination signal");
        exit(1);
    }
}


auto main(int argc, char *argv[]) -> int
{
    qRegisterMetaType<qintptr>(qPrintable(nameof(qintptr)));
    qRegisterMetaType<ActionResult>(qPrintable(nameof(ActionResult)));
    qRegisterMetaType<Restarter::Mode>(qPrintable(nameof(Restarter::Mode)));

    Log::init(Errlevels::ERROR_,
              Dbglevels::DEBUG, nullptr, false, nullptr, false);


    zInfo(QString(QT_TARGET)+':'+Buildnumber::toString()+':'+_instance.ToString());

    setShutDownSignal(SIGINT); // shut down on ctrl-c
    setShutDownSignal(SIGTERM); // shut down on killall

#ifdef RPI
    wiringPiSetup();
    pinMode (1, OUTPUT);
    digitalWrite (1, HIGH);

    pinMode (16, INPUT);
#endif

    QApplication app(argc, argv);

    if(!settings.Load()) settings.Save();

     AliveWorker aliveWorker;

    // ha lenne ablak, és lenne listWidget amibe a logger logolhat
    // konzolalkalmazásnál ez a standard err
    // MainWindow w;tarsnak egy szörnyü rezsimben kell el
    // w.show();
    //zlog.init(w.logListWidget());
//    zlog.init(nullptr);
//    zlog.isTrace = false;
//    zlog.ok("started");

    RecordHelper r;

    zTcpSocket *socket = nullptr;

    if(settings.issendalive())
    {
        socket = new zTcpSocket();
        socket->timeoutTimerToThread(aliveWorker.thread());

        socket->setFn(zTcpSocket::responseOk);
        socket->init(settings.aliveaddr(), TestHelper::alivePort);

        if(!aliveWorker.init(socket,
                              &zTcpSocket::sendAlive,
                              settings.aliveinterval())){
            zInfo("AliveWorker Init Error");
            return 1;
        }

        aliveWorker.start();
    }

    restarter = new Restarter();
    updater = new Updater();

    server = new HTTPThreadedServer("piapiinsole");
    server->setHostAddress(QHostAddress::Any, 8080);
    PressureHelper::init();

    server->addAction(Request::Method::GET, nameof(&doWork::get_data), &doWork::get_data);
    server->addAction(Request::Method::GET, nameof(&doWork::get_batt), &doWork::get_batt);
    server->addAction(Request::Method::GET, nameof(&doWork::get_data_length), &doWork::get_data_length);
    server->addAction(Request::Method::GET, nameof(&doWork::active), &doWork::active);
    server->addAction(Request::Method::GET, nameof(&doWork::version), &doWork::version);
    server->addAction(Request::Method::GET, nameof(&doWork::version4), &doWork::version4);//éppen fut
    server->addAction(Request::Method::GET, nameof(&doWork::version5), &doWork::version5);//telepitve, ha ez fut, ha nem
    server->addAction(Request::Method::GET, nameof(&doWork::instance), &doWork::instance);
    server->addAction(Request::Method::GET, nameof(&doWork::hwinfo), &doWork::hwinfo);
    server->addAction(Request::Method::GET, nameof(&doWork::get_storage_status), &doWork::get_storage_status);
    server->addAction(Request::Method::GET, nameof(&doWork::set_storage_mount), &doWork::set_storage_mount);
    server->addAction(Request::Method::GET, nameof(&doWork::update4), &doWork::update4);
    server->addAction(Request::Method::GET, nameof(&doWork::restart), &doWork::restart);
    server->addAction(Request::Method::GET, nameof(&doWork::set_rec_start), &doWork::set_rec_start); //set_rec_start
    server->addAction(Request::Method::GET, nameof(&doWork::set_rec_stop), &doWork::set_rec_stop); //set_rec_stop
    server->addAction(Request::Method::GET, nameof(&doWork::get_insole_status), &doWork::get_insole_status);
    server->addAction(Request::Method::GET, nameof(&doWork::shutdown), &doWork::shutdown);

    server->addAction(Request::Method::GET, nameof(&doWork::set_time_ntp), &doWork::set_time_ntp);

    Command::addAction(Command::Commands::Test, &doWorkCmd::TestCmd0);
    Command::addAction(Command::Commands::RecStartSync, &doWorkCmd::RecStartSync);
    Command::addAction(Command::Commands::StopRec, &doWorkCmd::StopRec);

    server->start();

//    zTcpSocket *socket = new zTcpSocket();

//    socket->setFn(zTcpSocket::responseOk);

//    //socket->init(s->getValue(aliveAddressKey, "172.16.1.222"), s->getValueInt(alivePortKey, 12345));
//    socket->init(TestHelper::aliveAddr, TestHelper::alivePort);
//    socket->setAliveString(s->getValue(aliveStringKey, "insole"));

//    QTimer *aliveTimer = new QTimer(nullptr);

//    QObject::connect(aliveTimer, &QTimer::timeout, [&]{
//        socket->sendTcpAlive();
//    });

//    aliveTimer->start(1000);

//    zUdpSocket *socket2 = new zUdpSocket();


    zUdpSocket *socket2 = new zUdpSocket();


    auto returnValue = app.exec();
    zInfo(QStringLiteral("exiting(%1)..").arg(returnValue))
    settings.Save();
    zInfo("stopping alive..")
    aliveWorker.stop();
    zInfo("stopping server..")
    server->stop();
    zInfo("quit..")
    qApp->processEvents();

    delete server;// server = nullptr;
    delete socket;// socket = nullptr;
    delete socket2;// socket2=nullptr;
    delete updater;
    delete restarter;
//    delete aliveTimer;
//    delete socket;
//    delete s;


    //if(u==Restarter::Mode::Shutdown) Shutdowner::Shutdown();

#ifdef RPI
    digitalWrite (1, LOW);
#endif

    return returnValue;
}




