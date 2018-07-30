#ifndef ZFILENAMEHELPER_H
#define ZFILENAMEHELPER_H

#include <QString>

class zFileNameHelper
{
public:
    zFileNameHelper();
//    static QString getContextFilename(QString munkadir, QString tfname);
//    static QString getCClassFilename(QString munkadir, QString adatbazisNev, QString tfname);
    static QString append(QString h, QString p0, QString = nullptr, QString = nullptr);

    static QStringList FindFileNameInDir(QString, QString, QStringList);
    static QString getfileName(QString fullPath);
    static QStringList GetSubdirs(QString& dirName);

    static const QStringList xmlFilter;
    static const QStringList captionFileFilter;
};

#endif // ZFILENAMEHELPER_H
