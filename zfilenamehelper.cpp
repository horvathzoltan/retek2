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

QString zFileNameHelper::append(QString h, QString p0, QString p1, QString p2){
    auto s = QDir::separator();

    QString e;

    if(p0.length()>3 && QString("CDEFGHIJKL").contains(p0[0]) && p0[1]==QChar(':')){
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


QStringList zFileNameHelper::GetSubdirs(QString& dirName){
    QDir d = QDir(dirName);
    QStringList datadirs  = d.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
    return datadirs;
}
//zTable::r_class 
//  class\s+(\w+)\s+(\{(?>[^{}]+|(?2))*\})

//https://stackoverflow.com/questions/8052460/recursively-iterate-over-all-the-files-in-a-directory-and-its-subdirectories-in
QStringList zFileNameHelper::FindFileNameInDir(QString dirName, QString fn_pattern, QStringList nameFilters ){
    QString fn = "*."+fn_pattern;
    QStringList ql;    

    QDir d = QDir(dirName);
    QStringList datadirs  = d.entryList(QStringList() << fn, QDir::Dirs);

    if(datadirs.isEmpty()) return ql;

    d.cd(datadirs.first());
    d.setNameFilters(nameFilters);//QStringList()<<"*.c"<<"*.cs");
    
    QDirIterator it(d, QDirIterator::Subdirectories);
    while (it.hasNext()){
        auto n =  it.next();
        ql << n;
        zlog.log("file: "+n);
    }
    return ql;

}

///
/// \brief zFileNameHelper::getfileName
/// A fájl kiterjesztés nélküli-első . előtti nevét adja
/// \param fullPath
/// \return
///
QString zFileNameHelper::getfileName(QString fullPath){
    QFileInfo fi(fullPath);
    QString fileName = fi.baseName();
    return fileName;
}



