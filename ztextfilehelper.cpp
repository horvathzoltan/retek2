#include "globals.h"
#include "zlog.h"
#include "ztextfilehelper.h"
#include "zstringhelper.h"

#include <QFileInfo>

QString zTextFileHelper::load(const QString& filename) {
    QFile f(filename);
    QString e;
    if (f.open(QFile::ReadOnly | QFile::Text))  {
        e =  QTextStream(&f).readAll();
    }
    else{
        zlog.log(QStringLiteral("A fájl nem található: %1 ERROR").arg(filename));
        e= zStringHelper::Empty;
    }
    return e;
}

void zTextFileHelper::save(QString txt, QString fn, bool isAppend) {
    QFile f(fn);

//    QFileInfo check_file(fn);

//    if (check_file.exists()){}


    auto om = QIODevice::WriteOnly | QIODevice::Text;
    if(isAppend) om |= QIODevice::Append;

    if (!f.open(om)){
        zLog::ShowDialog("nem menthető: "+fn);
        return;
        }
//    else{
//        zLog.("mentve: "+fn);
//    }

    QTextStream out(&f);
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));

    out.setCodec(QTextCodec::codecForName("UTF-8"));
    out.setGenerateByteOrderMark(true);
    out << txt.toUtf8();
    f.close();
}

void zTextFileHelper::append(QString fn, QString txt){
    save(txt, fn, true);
}
