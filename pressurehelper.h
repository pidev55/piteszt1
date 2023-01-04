#ifndef PRESSURES_H
#define PRESSURES_H

#include <QString>

struct PressureValues
{
public:
    int i1, i2, i3, i4, i5, i6, i7, i8;
    int i11, i12, i13, i14, i15, i16, i17, i18;    
    static const int data_length = 8+8;
    QString toString();    
private:
    static const QChar SEP;
};

class PressureHelper
{

public:
    static const int GPIO_BATT = 27;

    static PressureValues getAll();
    static int getBatt();
    static bool getAll2();
    static void init();
    static int get0();
    static bool isInited() {return inited;}

private:
    static bool inited;
    static int a;
};

#endif // PRESSURES_H
