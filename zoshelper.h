#ifndef ZOSHELPER_H
#define ZOSHELPER_H

#include <QString>


enum osType {Android, Blackberry, IOS, OSX, WinCE, Windows, Linux, UNIX, unknown};

class zosHelper
{
public:

    static QString name();
    static osType type();
    static void setLocale();
};

#endif // ZOSHELPER_H
