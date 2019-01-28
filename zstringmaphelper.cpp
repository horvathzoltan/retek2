#include <QFile>
#include <QTextStream>
#include <QTextCodec>
#include <QDir>

#include "globals.h"
#include "zstringhelper.h"
#include "zstringmaphelper.h"

zStringMapHelper::zStringMapHelper()
{

}

void zStringMapHelper::StringMapFeltolt(QString fn, QMap<QString, QString> *map) {
    auto ikey = zLog::openInfo(QStringLiteral("Beolvasás: %1").arg(fn));
    QFile file(fn);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        zLog::appendInfo(ikey, "error");
        zLog::closeInfo(ikey);
        return;
    }

    map->clear();
    QTextStream in(&file);

    //QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));
    in.setCodec(QTextCodec::codecForName("UTF-8"));

    while (!in.atEnd())
    {
        QString line = in.readLine();

        if(line.startsWith('#'))
        {
            zInfo(line);
        }
        else
        {
            int ix = line.indexOf(zStringHelper::SEP);
            if (ix > 0)
            {
                QString k1 = line.left(ix);//.toLower();
                QString k2 = line.right(line.length() - (ix + 1));//.toLower();

                map->insert(k1, k2);
            }
        }
    }
    file.close();

    zLog::appendInfo(ikey, zLog::OK);
    zLog::closeInfo(ikey);
}

void zStringMapHelper::StringMapSave(QString fn, QMap<QString, QString> *map) {
    QDir d = QFileInfo(fn).absoluteDir();
    if(!d.exists()) d.mkpath(d.absolutePath());

    QFile file(fn);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text)) {
        zLog::dialogError(QStringLiteral("nem menthető: %1 error").arg(fn));
        return;
    }

    QTextStream out(&file);
    //QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));
    out.setCodec(QTextCodec::codecForName("UTF-8"));

    out.setGenerateByteOrderMark(true);

    QMapIterator<QString, QString> i(*map);
    while (i.hasNext()) {
        i.next();
        out << i.key() << "," << i.value() << endl;
    }

    file.close();

    zInfo(QStringLiteral("Kiírva: %1 ok").arg(fn));
}

bool zStringMapHelper::contains(QMap<QString, QString> *map, QString k){
    auto ks = map->keys();
    zforeach(e, ks){
       if(e->toLower()==k.toLower()) return true;
    }
    return false;
}

QString zStringMapHelper::getKey(QMap<QString, QString> *map, QString k){
    auto ks = map->keys();
    zforeach(e, ks){
       if(e->toLower()==k.toLower()) return *e;
    }
    return zStringHelper::Empty;
}

