#include "globals.h"
#include "zfilenamehelper.h"
#include "ztextfilehelper.h"
#include <QDir>
#include <QDirIterator>

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


//zTable::r_class 
//  class\s+(\w+)\s+(\{(?>[^{}]+|(?2))*\})

//https://stackoverflow.com/questions/8052460/recursively-iterate-over-all-the-files-in-a-directory-and-its-subdirectories-in
QStringList zFileNameHelper::FindFileNameInDir(QString dirName, QString fn_pattern, QStringList nameFilters ){

    QString fn = "*."+fn_pattern;
    QStringList ql;

    //zlog.log("dirName: "+dirName + ","+ fn);

    QDir d = QDir(dirName);
    QStringList datadirs  = d.entryList(QStringList() << fn, QDir::Dirs);

    if(datadirs.isEmpty()) return ql;

    //zlog.log("datadir: "+datadirs.first());

    d.cd(datadirs.first());
    d.setNameFilters(nameFilters);//QStringList()<<"*.c"<<"*.cs");

    //QRegularExpression r_regexp(r_pattern);
    
    QDirIterator it(d, QDirIterator::Subdirectories);
    while (it.hasNext()){
        auto n =  it.next();
        
//        iregexpf(!r_.pattern().isEmpty()){
//            QString txt = zTextFileHelper::load(n);
//            if(!txt.isEmpty()){
//            auto i_regexp = r_regexp.globalMatch(txt);
//            if(i_regexp.hasNext()){
                
//                }
//            }
//        }

        ql << n;
        zlog.log("file: "+n);
    }
    return ql;

}


