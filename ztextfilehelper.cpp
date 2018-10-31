#include "globals.h"
#include "zlog.h"
#include "ztextfilehelper.h"
#include "zstringhelper.h"

#include <QFileInfo>

QString zTextFileHelper::load(const QString& filename) {
    QFileInfo fi(filename);
    if(!fi.exists())
    {
        zlog.error(QStringLiteral("a fájl nem létezik: %2").arg(filename));
        return zStringHelper::Empty;
    }

    if(fi.isRelative())
    {
        // ha a filepath relatív, akkor az abszolút függ a művelettől  - hogy melyik könyvtárra mutat/mutathat

        zlog.error(QStringLiteral("az elérési út relatív: %2").arg(filename));
        return zStringHelper::Empty;
    }

    QFile f(filename);
    QString e;   

    // TODO ha relatív a filename, akkor abszolúttá kell tenni
    // egyébként megnyitható azaz

    if (f.open(QFile::ReadOnly | QFile::Text))  {
        zlog.ok(QStringLiteral("Beolvasva: %1").arg(filename));
        e =  QTextStream(&f).readAll();
    }
    else{
        zlog.error(QStringLiteral("A fájl nem nyitható meg: %1 ERROR").arg(filename));
        e= zStringHelper::Empty;
    }
    return e;
}

/*
A txt-t nem feltétlenül kell itt validálni
- üres fájl mentése/létrehozása lehet egy valós igény
*/

void zTextFileHelper::save(const QString& txt, const QString& fn, bool isAppend) {

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

//void zTextFileHelper::append(QString fn, QString txt){
//    save(txt, fn, true);
//}
