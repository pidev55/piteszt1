#include "zlogicexception.h"
#include <QDebug>

zLogicException::zLogicException(const QString& _msg) :msg(_msg)
{
       qDebug() << this->msg;
}

void zLogicException::raise() const
{
    throw *this;
}

zLogicException* zLogicException::clone() const
{
     return new zLogicException(*this);
}
