#include "zfilenamehelper.h"
#include <QDir>

zFileNameHelper::zFileNameHelper()
{

}


//QString zFileNameHelper::getContextFilename(QString munkadir ,QString tfname) {
//    QDir dir(munkadir);
//    //auto s = dir.relativeFilePath("Model");

//    return QString(munkadir+R"(\Model\%1)").arg(tfname);
//}

//QString zFileNameHelper::getCClassFilename(QString munkadir, QString adatbazisNev, QString tfname) {
//    //return QString(munkadir+R"(\%1\%2)").arg(adatbazisNev).arg(tfname);
//}

QString zFileNameHelper::append(QString h, QString p0, QString p1, QString p2){
    auto s = QDir::separator();
    auto e = h+s+p0;

    if(p1.isEmpty()) return e;
    e += s+p1;
    if(p2.isEmpty()) return e;
    e += s+p2;
    return e;
}

