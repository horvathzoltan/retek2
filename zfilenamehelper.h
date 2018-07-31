#ifndef ZFILENAMEHELPER_H
#define ZFILENAMEHELPER_H

#include <QString>

class zFileNameHelper
{
private:
    static const QString dxMap;
    static QString append(QString h, QString p0, QString = nullptr, QString = nullptr);
public:
    zFileNameHelper();
//    static QString getContextFilename(QString munkadir, QString tfname);
//    static QString getCClassFilename(QString munkadir, QString adatbazisNev, QString tfname);


    static QStringList FindFileNameInDir(QString, QString, QStringList);
    static QString getfileName(QString fullPath);
    static QStringList GetSubdirs(QString& dirName);

    static const QStringList xmlFilter;
    static const QStringList captionFileFilter;    
    static QString getDxMap();

    static QString getProjectDir();
    static QString getProjectSubDir(QString sd);
    static QString getCurrentProjectDir();
    static QString getCurrentProjectSubDir(QString dn);
    static QString getCurrentProjectFileName(QString fn);

    static QString getTmpDir();
    static QString getTmpSubDir(QString dn);
    static QString getCurrentTmpSubDir(QString dn);

    static QString getHomeSubDir(QString);
    static QString getSettingsDir();
    static QString getSettingsFileName();
    static QString getDbconnFileName();
    static QString getSettingsSubDir(QString sd);
    static QString getSettingsFileName(QString fn);
};

#endif // ZFILENAMEHELPER_H
