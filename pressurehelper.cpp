#include "pressurehelper.h"
#include "globals.h"
#include "pressurehelperevent.h"
#ifdef RPI
#include <wiringPi.h>
//#include <wiringPiSPI.h>
#include <mcp3004.h>
#else
#include <QRandomGenerator>
#endif

#define BASE 100
#define SPI_CHAN 0
#define BASE1 140
#define SPI_CHAN1 1

extern PressureHelperEvent events;

bool PressureHelper::inited = false;

PressureValues PressureHelper::getAll()
{
    PressureValues v;
#ifdef RPI
    v.i1 = analogRead (BASE + 0);
    v.i2 = analogRead (BASE + 1);
    v.i3 = analogRead (BASE + 2);
    v.i4 = analogRead (BASE + 3);
    v.i5 = analogRead (BASE + 4);
    v.i6 = analogRead (BASE + 5);
    v.i7 = analogRead (BASE + 6);
    v.i8 = analogRead (BASE + 7);

    v.i11 = analogRead (BASE1 + 0);
    v.i12 = analogRead (BASE1 + 1);
    v.i13 = analogRead (BASE1 + 2);
    v.i14 = analogRead (BASE1 + 3);
    v.i15 = analogRead (BASE1 + 4);
    v.i16 = analogRead (BASE1 + 5);
    v.i17 = analogRead (BASE1 + 6);
    v.i18 = analogRead (BASE1 + 7);
#else
    static QRandomGenerator rnd(16);
    v.i1 = 14+rnd.bounded(4);
    v.i2 = 128+rnd.bounded(500);
    v.i3 = 128+rnd.bounded(500);
    v.i4 = 128+rnd.bounded(500);
    v.i5 = 128+rnd.bounded(500);
    v.i6 = 128+rnd.bounded(500);
    v.i7 = 128+rnd.bounded(500);
    v.i8 = 128+rnd.bounded(500);

    v.i11 = 128+rnd.bounded(500);
    v.i12 = 128+rnd.bounded(500);
    v.i13 = 128+rnd.bounded(500);
    v.i14 = 128+rnd.bounded(500);
    v.i15 = 128+rnd.bounded(500);
    v.i16 = 128+rnd.bounded(500);
    v.i17 = 128+rnd.bounded(500);
    v.i18 = 128+rnd.bounded(500);
#endif


    return v;
}

int PressureHelper::getBatt()
{
#ifdef RPI
    return digitalRead(GPIO_BATT);
#else
    return -1;
#endif
}

bool PressureHelper::getAll2()
{
    auto v = getAll();
    emit events.newframe(v.toString());
    return true;
}

int PressureHelper::get0()
{
#ifdef RPI
    return analogRead (BASE + 0);
#endif
}

void PressureHelper::init()
{
#ifdef RPI
    //wiringPiSetupGpio ();
    wiringPiSetup() ;
    mcp3004Setup(BASE, SPI_CHAN); // 3004 and 3008 are the same 4/8 channels
    mcp3004Setup(BASE1, SPI_CHAN1);
    inited = true;
#endif
}

const QChar PressureValues::SEP = ';';

QString PressureValues::toString()
{
    return QString::number(i1)+SEP+
            QString::number(i2)+SEP+
            QString::number(i3)+SEP+
            QString::number(i4)+SEP+
            QString::number(i5)+SEP+
            QString::number(i6)+SEP+
            QString::number(i7)+SEP+
            QString::number(i8)
    +SEP+
            QString::number(i11)+SEP+
            QString::number(i12)+SEP+
            QString::number(i13)+SEP+
            QString::number(i14)+SEP+
            QString::number(i15)+SEP+
            QString::number(i16)+SEP+
            QString::number(i17)+SEP+
            QString::number(i18);
}
