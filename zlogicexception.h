#ifndef ZLOGICEXCEPTION_H
#define ZLOGICEXCEPTION_H

#include <QException>
//#include <QDebug>

class zLogicException: public QException
{
private:
    //Q_DISABLE_COPY(zLogicException)
    const QString& msg;

public:
    explicit zLogicException(const QString& _msg);

    void raise() const override;// { throw *this; }
    zLogicException* clone() const override;// { return new zLogicException(*this); }

};

#endif // ZLOGICEXCEPTION_H
