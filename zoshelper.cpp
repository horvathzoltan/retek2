#include "zoshelper.h"

#include <QTextCodec>
#include "zlog.h"

QString zosHelper::name()
{
    switch(type())
    {
    case Android:
        return QStringLiteral("Android");
    case Blackberry:
        return QStringLiteral("Blackberry");
    case IOS:
        return QStringLiteral("IOS");
    case OSX:
        return QStringLiteral("OSX");
    case WinCE:
        return QStringLiteral("WinCE");
    case Windows:
        return QStringLiteral("Windows");
    case Linux:
        return QStringLiteral("Linux");
    case UNIX:
        return QStringLiteral("UNIX");
    default:
        return QStringLiteral("unknown");
    }
}


osType zosHelper::type()
{
#if defined(Q_OS_ANDROID)
    return osType::Android;
#elif defined(Q_OS_BLACKBERRY)
    return osType::Blackberry;
#elif defined(Q_OS_IOS)
    return osType::IOS;
#elif defined(Q_OS_MAC)
    return osType::OSX;
#elif defined(Q_OS_WINCE)
    return osType::WinCE;
#elif defined(Q_OS_WIN)
    return osType::Windows;
#elif defined(Q_OS_LINUX)
    return osType::Linux;
#elif defined(Q_OS_UNIX)
    return osType::UNIX;
#else
    return osType::unknown;
#endif
}

void zosHelper::setLocale()
{
//    zError("a");
    switch(type())
    {
    case Android:
        QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));
        break;
    case Blackberry:
        QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));
        break;
    case IOS:
        QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));
        break;
    case OSX:
        QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));
        break;
    case WinCE:
        QTextCodec::setCodecForLocale(QTextCodec::codecForName("Windows-1250"));
        break;
    case Windows:
        QTextCodec::setCodecForLocale(QTextCodec::codecForName("Windows-1250"));
        break;
    case Linux:
        QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));
        break;
    case UNIX:
        QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));
        break;
    default:
        QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));
        break;
    }
}

