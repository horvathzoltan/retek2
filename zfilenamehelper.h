#ifndef ZFILENAMEHELPER_H
#define ZFILENAMEHELPER_H

#include <QString>

class zFileNameHelper
{
public:
    zFileNameHelper();
//    static QString getContextFilename(QString munkadir, QString tfname);
//    static QString getCClassFilename(QString munkadir, QString adatbazisNev, QString tfname);
    static QString append(QString h, QString p0, QString = NULL, QString = NULL);

    static QStringList FindFileNameInDir(QString, QString, QStringList);
    static QString getfileName(QString fullPath);
};

#endif // ZFILENAMEHELPER_H
