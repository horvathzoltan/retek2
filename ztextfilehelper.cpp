#include "zlog.h"
#include "ztextfilehelper.h"

QString zTextFileHelper::load(QString filename) {
    QFile f(filename);
    if (!f.open(QFile::ReadOnly | QFile::Text)) return "";
    return QTextStream(&f).readAll();
}

void zTextFileHelper::save(QString txt, QString fn, bool isAppend) {
    QFile f(fn);

    auto om = QIODevice::WriteOnly | QIODevice::Text;
    if(isAppend) om |= QIODevice::Append;

    if (!f.open(om)){
        zLog::ShowDialog("nem menthet: "+fn);
        return;
        }

    QTextStream out(&f);
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));

    out.setCodec(QTextCodec::codecForName("UTF-8"));
    out.setGenerateByteOrderMark(true);
    out << txt.toUtf8();
    f.close();
}

void zTextFileHelper::append(QString txt, QString fn){
    save(txt, fn, true);
}
