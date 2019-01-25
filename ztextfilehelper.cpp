#include "globals.h"
#include "zlog.h"
#include "ztextfilehelper.h"
#include "zstringhelper.h"
#include "zfilenamehelper.h"

#include <QFileInfo>

QString zTextFileHelper::load2(const QString& filename) {
    auto ikey = zLog::openInfo(QStringLiteral("Beolvasás: %1").arg(filename));
    QFileInfo fi(filename);    
    if(!fi.isAbsolute())
    {
        zInfo(QStringLiteral("nem abszolut path: %1").arg(filename));
        zLog::appendInfo(ikey, "error");
        zLog::closeInfo(ikey);
        return zStringHelper::Empty;
    }

    if(!fi.exists())
    {
        zInfo(QStringLiteral("a fájl nem létezik: %1").arg(filename));
        zLog::appendInfo(ikey, "error");
        zLog::closeInfo(ikey);
        return zStringHelper::Empty;
    }    

    QFile f(filename);
    QString e;   

    // TODO ha relatív a filename, akkor abszolúttá kell tenni
    // egyébként megnyitható azaz

    if (f.open(QFile::ReadOnly | QFile::Text))  {
        zLog::appendInfo(ikey, "ok");
        zLog::closeInfo(ikey);
        //zInfo(QStringLiteral("Beolvasás: %1").arg(filename));
        e =  QTextStream(&f).readAll();
    }
    else{
        zLog::appendInfo(ikey, "error");
        zLog::closeInfo(ikey);
        zInfo(QStringLiteral("A fájl nem nyitható meg: %1 ERROR").arg(filename));
        e= zStringHelper::Empty;
    }
    zLog::closeInfo(ikey);
    return e;
}

/*
A txt-t nem feltétlenül kell itt validálni
- üres fájl mentése/létrehozása lehet egy valós igény
*/

void zTextFileHelper::save(const QString& txt, const QString& fn, bool isAppend) {

    if(fn.length()>256)
    {
        zInfo("Fájlnév túl hosszú error");
        return;
    }
//    QFile logfile(lfn);
//    logfile.open(QIODevice::Append | QIODevice::Text);
//    QTextStream out(&logfile);
//    out << lftxt << endl;

    QFile f(fn);

    auto om = QIODevice::WriteOnly | QIODevice::Text; // openmode
    if(isAppend) om |= QIODevice::Append;

    if (!f.open(om)){
        zLog::dialogError("nem menthető: "+fn);
        return;
        }


    //QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));

    QTextStream out(&f);
    out.setCodec(QTextCodec::codecForName("UTF-8"));
    out.setGenerateByteOrderMark(true);
    out << txt.toUtf8();
    f.close();
}

QString zTextFileHelper::load(const QString& url)
{
    if(zFileNameHelper::isURL(url))
    {
    //zInfo("url");
    auto e = downloader.download(QStringLiteral(R"(https://docs.google.com/document/export?format=html&id=1tPwsVMObxU9QmA3XR4RpbHPpjcG7hVbd7KQqLD_ABK8&includes_info_params=true)"));
    return zStringHelper::HtmlDecode(e);
    //zTextFileHelper::save(f_txt, "/home/zoli/aa.html");
    }

    QString fn = zFileNameHelper::getCurrentProjectFileNameAbsolut(url);
    return zTextFileHelper::load2(fn);
}

//void zTextFileHelper::append(QString fn, QString txt){
//    save(txt, fn, true);
//}
