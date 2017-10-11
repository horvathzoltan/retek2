#include "zfilenamehelper.h"

zFileNameHelper::zFileNameHelper()
{

}


QString zFileNameHelper::getContextFilename(QString munkadir ,QString tfname) {
    return QString(munkadir+R"(\Model\%1)").arg(tfname);
}

QString zFileNameHelper::getCClassFilename(QString munkadir, QString adatbazisNev, QString tfname) {
    return QString(munkadir+R"(\%1\%2)").arg(adatbazisNev).arg(tfname);
}
