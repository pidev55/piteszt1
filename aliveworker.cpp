#include "aliveworker.h"
#include "common/logger/log.h"
#include "QAbstractEventDispatcher"

AliveWorker::AliveWorker(){
    zTrace();
    isinit = false;
    _thread.setObjectName(QStringLiteral("alive_thread"));

    //connect(this, SIGNAL(start_signal(int)), this, SLOT(start_slot(int)));
    //connect(this, SIGNAL(stop_signal()), this, SLOT(stop_slot()));
    connect(this, &AliveWorker::start_signal, this, &AliveWorker::start_slot);
    connect(this, &AliveWorker::stop_signal, this, &AliveWorker::stop_slot);
    this->moveToThread(&_thread);
    _thread.start(QThread::Priority::NormalPriority);
    _fn = nullptr;
    _object = nullptr;
}


AliveWorker::~AliveWorker()
{
    _thread.quit();
    _thread.wait();
    delete _timer;
}

auto AliveWorker::init(QObject* o, aliveFn fn, int i) -> bool
{
    isinit = false;
    if(!o) return false;
    if(!fn) return false;
    if(i<30) return false;
    _interval=i;
    _fn = fn;
    _object = o;
    return isinit = true;
}

void AliveWorker::on_timeout()
{
    _fn(_object);
}

auto AliveWorker::start() -> bool
{
    if(!isinit) return false;
    if(_timer) return false;
    //if(_timer->isActive()) return false;
    emit start_signal();
    processEvents();
    return true;
}

void AliveWorker::start_slot()
{
    if(!isinit) return;
    if(_timer) return;
    _timer = new QTimer();
    _timer->setTimerType(Qt::VeryCoarseTimer);
    connect(_timer, &QTimer::timeout, this, &AliveWorker::on_timeout);

    _timer->moveToThread(&_thread);

    _timer->start(_interval);
}

void AliveWorker::stop_slot()
{
    if(!isinit) return;
    if(!_timer) return;
    QMetaObject::invokeMethod(_timer, "stop", Qt::QueuedConnection);
    //_timer->stop();
    //_thread.quit();
    //_thread.wait();

    delete _timer; _timer = nullptr;
}

auto AliveWorker::stop() -> bool
{
    if(!isinit) return false;
    if(!_timer) return false;
    if(!_timer->isActive()) return false;
    emit stop_signal();
    processEvents();
    return true;
}


void AliveWorker::processEvents(){
    auto evd = _thread.eventDispatcher();
    if(evd) evd->processEvents(QEventLoop::ProcessEventsFlag::AllEvents);
}
