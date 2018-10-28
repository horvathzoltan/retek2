#include "zlogicexception.h"


zLogicException::zLogicException(const QString& _msg) :msg(_msg)
   {
//    auto a = this->msg;
//    auto b = qPrintable(a);
         qDebug() << this->msg;
   }

void zLogicException::raise() const { throw *this; }

zLogicException* zLogicException::clone() const
{
    return new zLogicException(*this);
}
