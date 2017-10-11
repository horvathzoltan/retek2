#include "zstringmaphelper.h"
#include <QFile>
#include <QTextStream>
#include <QTextCodec>
#include <QDir>
#include "zlog.h"

zStringMapHelper::zStringMapHelper()
{

}

void zStringMapHelper::StringMapFeltolt(QString fn, QMap<QString, QString> *map) {
    QFile file(fn);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) return;

    map->clear();
    QTextStream in(&file);

    QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));
    in.setCodec(QTextCodec::codecForName("UTF-8"));

    while (!in.atEnd()) {
        QString line = in.readLine();
        //QString line(bline);

        int ix = line.indexOf(",");
        if (ix > 0) {
            QString k1 = line.left(ix).toLower();
            QString k2 = line.right(line.length() - (ix + 1));

            map->insert(k1, k2);
            //map->insert("id","b");
        }
    }
    file.close();
}

void zStringMapHelper::StringMapSave(QString fn, QMap<QString, QString> *map) {
    QDir d = QFileInfo(fn).absoluteDir();
    if(!d.exists()) d.mkpath(d.absolutePath());

    QFile file(fn);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text)) {
        zLog::ShowDialog("nem menthet: " + fn);
        return;
    }

    QTextStream out(&file);
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));
    out.setCodec(QTextCodec::codecForName("UTF-8"));

    out.setGenerateByteOrderMark(true);

    QMapIterator<QString, QString> i(*map);
    while (i.hasNext()) {
        i.next();
        out << i.key() << "," << i.value() << endl;
    }

    file.close();
}
