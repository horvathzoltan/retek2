#ifndef ZFILENAMEHELPER_H
#define ZFILENAMEHELPER_H

#include <QString>
#include <QWidget>

class zFileNameHelper
{
private:
    static const QString dxMap;
    static QString append(const QString& h, QString p0, const QString& = nullptr, const QString& = nullptr);
public:
    zFileNameHelper();
//    static QString getContextFilename(QString munkadir, QString tfname);
//    static QString getCClassFilename(QString munkadir, QString adatbazisNev, QString tfname);


    static QStringList FindFileNameInDir(const QString&, const QString&, const QStringList&);
    static QString getfileName(const QString& fullPath);
    static QStringList GetSubdirs(const QString& dirName);

    static const QString urlpattern;
    static const QRegularExpression urlregexp;
    static const QStringList xmlFilter;
    static const QStringList captionFileFilter;    
    static const QStringList sqlmapFileFilter;
    static const QStringList classmapFileFilter;    

    static QString getDxMap();

    static QString getProjectDir();
    static QString getProjectSubDir(const QString &sd);
    static QString getCurrentProjectDir();
    static QString getCurrentProjectSubDir(const QString& dn);
    static QString getCurrentProjectFileName(const QString& fn);
    static QString getCurrentProjectFileNameAbsolut(const QString& fn);


    static QString getTmpDir();
    static QString getTmpSubDir(const QString& dn);
    static QString getCurrentTmpSubDir(const QString& dn);

    static QString getHomeSubDir(const QString&);
    static QString getSettingsDir();
    static QString getSettingsFileName();
    static QString getDbConnFileName();
    static QString getSourceConnFileName();
    static QString getDocumentConnFileName();

    static QString getSettingsSubDir(const QString& sd);
    static QString getSettingsFileName(const QString& fn);

    static QStringList getSourceFilenames(const QString& path);
    static QStringList getDocumentFilenames(const QString& path);

    static bool isURL(const QString& path);
    static bool isAppLocal(const QString& path);
    static bool isRelative(const QString& path);

    static QString fileNameDialog(QWidget *w, const QString &caption, const QString &filter, const QString &dir = QString());
    static QString getDirName(const QString &fn);
};

#endif // ZFILENAMEHELPER_H
