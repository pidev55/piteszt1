#include "recordworker.h"
#include <QAbstractEventDispatcher>

workFn RecordWorker::wf1;

RecordWorker::RecordWorker()
{
    connect(this, &RecordWorker::start_signal, this, &RecordWorker::start_slot);
    connect(this, &RecordWorker::stop_signal, this, &RecordWorker::stop_slot);
}

RecordWorker::~RecordWorker()
{
    delete timer;
}

bool RecordWorker::init(workFn d)
{
    isinit = false;
    if(d == nullptr) return false;
    wf1 = d;
    return isinit = true;
}

bool RecordWorker::start(int i)
{
    if(!isinit) return false;
    if(timer) return false;
    if(i<3) return false;
    emit start_signal(i);
    return true;
}

bool RecordWorker::stop()
{
    if(!isinit) return false;
    if(!timer) return false;

    emit stop_signal();
    return true;
}

void RecordWorker::start_slot(int i)
{
    if(!isinit) return;
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, [](){wf1();});
    timer->start(i);
}

void RecordWorker::stop_slot()
{
    if(!isinit) return;
    if(!timer) return;
    timer->stop();
    delete timer; timer = nullptr;
}

void RecordWorker::processEvents(){
    if(!isinit) return;
    if(!timer) return;
    auto evd = timer->thread()->eventDispatcher();
    if(evd) evd->processEvents(QEventLoop::ProcessEventsFlag::AllEvents);
}
