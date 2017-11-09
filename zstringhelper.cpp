#include <QFile>
#include <QTextStream>
#include <QTextCodec>

#include "zlog.h"
#include "zstringhelper.h"

/*
zStringHelper::zStringHelper()
{

}
*/

bool zStringHelper::toBool(QString ezt){
    if(ezt.isEmpty()) return false;
    ezt = ezt.toLower();
    if(!ezt.compare("yes", Qt::CaseInsensitive)) return true;
    if(!ezt.compare("true", Qt::CaseInsensitive)) return true;
    if(!ezt.compare("ok", Qt::CaseInsensitive)) return true;
    return false;
}

QString zStringHelper::Load(QString filename) {
    QFile f(filename);
    if (!f.open(QFile::ReadOnly | QFile::Text)) return "";
    return QTextStream(&f).readAll();
}

void zStringHelper::Save(QString *txt, QString fn) {
    QFile f(fn);

    if (!f.open(QIODevice::WriteOnly | QIODevice::Text)){
        zLog::ShowDialog("nem menthet: "+fn);
        return;
        }

    QTextStream out(&f);
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));

    out.setCodec(QTextCodec::codecForName("UTF-8"));
    out.setGenerateByteOrderMark(true);
    out << (*txt).toUtf8();
    f.close();
}
