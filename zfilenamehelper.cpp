#include "globals.h"
#include "zfilenamehelper.h"
#include "ztextfilehelper.h"

#include <QDir>
#include <QDirIterator>




//QString zFileNameHelper::getContextFilename(QString munkadir ,QString tfname) {
//    QDir dir(munkadir);
//    //auto s = dir.relativeFilePath("Model");

//    return QString(munkadir+R"(\Model\%1)").arg(tfname);
//}

//QString zFileNameHelper::getCClassFilename(QString munkadir, QString adatbazisNev, QString tfname) {
//    //return QString(munkadir+R"(\%1\%2)").arg(adatbazisNev).arg(tfname);
//}

const QStringList zFileNameHelper::xmlFilter = QStringList(QStringLiteral("*.xml"));
const QStringList zFileNameHelper::captionFileFilter = QStringList(QStringLiteral("caption*.csv"));
const QStringList zFileNameHelper::sqlmapFileFilter = QStringList(QStringLiteral("sqlmap*.csv"));
const QStringList zFileNameHelper::classmapFileFilter = QStringList(QStringLiteral("classmap*.csv"));

const QString zFileNameHelper::dxMap = QStringLiteral("dxMap.csv");


QString zFileNameHelper::append(const QString& h, QString p0, const QString& p1, const QString& p2){
    auto s = QDir::separator();

    QString e;

    if(p0.length()>3 && QStringLiteral("CDEFGHIJKL").contains(p0[0]) && p0[1]==QChar(':')){
        e = p0;
    }else{
        e = h+s+p0;
    }


    if(p1.isEmpty()) return e;
    e += s+p1;
    if(p2.isEmpty()) return e;
    e += s+p2;
    return e;
}


QStringList zFileNameHelper::GetSubdirs(const QString& dirName){
    QDir d = QDir(dirName);
    QStringList datadirs  = d.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
    return datadirs;
}

QString zFileNameHelper::getDxMap()
{
    return getCurrentProjectFileName(zFileNameHelper::dxMap);
    //return zFileNameHelper::append(beallitasok.projectPath, beallitasok.currentProjectName, zFileNameHelper::dxMap);
    //return zFileNameHelper::append(cp, zFileNameHelper::dxMap);
}

QString zFileNameHelper::getProjectSubDir(const QString& sd)
{
    auto td = getProjectDir();
    return append(td, sd);
}

QString zFileNameHelper::getCurrentProjectDir()
{
    return getProjectSubDir(beallitasok.currentProjectName);
    //return cp;//zFileNameHelper::append(cp, beallitasok.currentProjectName);
}


QString zFileNameHelper::getCurrentProjectSubDir(const QString& dn)
{
    //auto cp = getCurrentProjectDir();
    //return zFileNameHelper::append(QDir::homePath(),projectdir, beallitasok.currentProjectName, dirname);
    //return zFileNameHelper::append(cp, dirname);
    return getCurrentProjectFileName(dn);
}

QString zFileNameHelper::getCurrentProjectFileName(const QString& fn)
{
    auto cp = getCurrentProjectDir();
    //return append(QDir::homePath(),beallitasok.projectdir,beallitasok.currentProjectName, fn);
    return append(cp, fn);
}

QString zFileNameHelper::getTmpSubDir(const QString& tfname)
{
    auto td = getTmpDir();
    return append(td, tfname);
}


QString zFileNameHelper::getCurrentTmpSubDir(const QString& sd)
{
    auto td = getTmpSubDir(beallitasok.currentProjectName);
    return append(td, sd);
}

/*alapdirek*/

QString zFileNameHelper::getHomeSubDir(const QString& sd){
    return append(QDir::homePath(),sd);
}

QString zFileNameHelper::getSettingsDir()
{
    return getHomeSubDir(beallitasok.settingsdir);
}

QString zFileNameHelper::getProjectDir()
{
    return getHomeSubDir(beallitasok.projectdir);
}

QString zFileNameHelper::getTmpDir()
{
    return getHomeSubDir(beallitasok.tmpDir);
}

QString zFileNameHelper::getSettingsSubDir(const QString& sd){
    //auto settingsDir = getSettingsDir();
    //return append(settingsDir,sd);
    return getSettingsFileName(sd);
}

QString zFileNameHelper::getSettingsFileName(const QString& fn){
    auto settingsDir = getSettingsDir();
    return append(settingsDir,fn);
}


QString zFileNameHelper::getSettingsFileName()
{
    return getSettingsFileName(beallitasok.settings_filename);
}

QString zFileNameHelper::getDbConnFileName()
{
    return getSettingsFileName(beallitasok.dbconnections_filename);
}

QString zFileNameHelper::getSourceConnFileName()
{
    return getSettingsFileName(beallitasok.source_connections_filename);
}

//zTable::r_class 
//  class\s+(\w+)\s+(\{(?>[^{}]+|(?2))*\})

//https://stackoverflow.com/questions/8052460/recursively-iterate-over-all-the-files-in-a-directory-and-its-subdirectories-in
QStringList zFileNameHelper::FindFileNameInDir(const QString& dirName, const QString& fn_pattern, const QStringList& nameFilters ){
    QString fn = "*."+fn_pattern;
    QStringList ql;    

    QDir d = QDir(dirName);
    QStringList datadirs  = d.entryList(QStringList() << fn, QDir::Dirs);

    if(datadirs.isEmpty()) return ql;

    d.cd(datadirs.first());
    d.setNameFilters(nameFilters);//QStringList()<<"*.c"<<"*.cs");
    
    QDirIterator it(d, QDirIterator::Subdirectories);
    while (it.hasNext())
    {
        auto n =  it.next();
        ql << n;
        zlog.trace(zfn(), n);
    }
    return ql;

}

///
/// \brief zFileNameHelper::getfileName
/// A fájl kiterjesztés nélküli-első . előtti nevét adja
/// \param fullPath
/// \return
///
QString zFileNameHelper::getfileName(const QString& fullPath)
{
    QFileInfo fi(fullPath);
    QString fileName = fi.baseName();
    return fileName;
}


QStringList zFileNameHelper::getSourceFilenames(const QString& path)
{
    QStringList nameFilters(QStringLiteral("*.c?"));
    auto e = FindFileNameInDir(path, zStringHelper::Empty, nameFilters);

    return e;
}


