#include "zlogicexception.h"


zLogicException::zLogicException(const QString& _msg) :msg(_msg)
{
    QMessageLogger(QT_MESSAGELOG_FILE, QT_MESSAGELOG_LINE, static_cast<const char*>(__PRETTY_FUNCTION__)).debug().noquote()<<msg;
}

void zLogicException::raise() const
{
    throw *this;
}

zLogicException* zLogicException::clone() const
{
    return new zLogicException(*this);
}
