#include "zlogicexception.h"


zLogicException::zLogicException(const QString& _msg) :msg(_msg)
{
#ifdef Q_OS_WIN
    auto a = __FUNCSIG__;
#elif defined(Q_OS_LINUX)
    auto a = __PRETTY_FUNCTION__;
#endif
    QMessageLogger(QT_MESSAGELOG_FILE, QT_MESSAGELOG_LINE, static_cast<const char*>(a)).debug().noquote()<<msg;
}

void zLogicException::raise() const
{
    throw *this;
}

zLogicException* zLogicException::clone() const
{
    return new zLogicException(*this);
}
