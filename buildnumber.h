#ifndef BUILDNUMBER_H
#define BUILDNUMBER_H

#define BUILDNUMBER 1499

#include <QString>

struct Buildnumber{ static QString toString(); };

#endif // BUILDNUMBER_H

/*
// buildnumber.cpp should contain this lines below to forcing build with makefile depencencies
#include <QString>
#include "buildnumber.h"

QString Buildnumber::toString(){return QString::number(BUILDNUMBER);}
*/

