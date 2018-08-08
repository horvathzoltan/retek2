#include "globals.h"
#include "zconversionmap.h"
#include "zfilenamehelper.h"
#include "zstringhelper.h"
#include "zstringmaphelper.h"

#include <QTextCodec>

zConversionMap::zConversionMap() = default;


//const QString zConversionMap::Empty = QStringLiteral("?");

QList<zConversionMap> zConversionMap::loadAll(const QString& filePath, const QStringList& fileNameFilters){
    QList<zConversionMap> e;

    QStringList files = zFileNameHelper::FindFileNameInDir(filePath, zStringHelper::Empty, fileNameFilters);

    zforeach(f, files){
       auto m = load(*f);
       if(m.list.isEmpty())
       {
            zlog.error(QStringLiteral("Nem tartalmaz beolvasható sorokat: %1").arg(*f));
       }
       else
       {
           e << m;
       }
    }

    return e;
}

zConversionMap zConversionMap::load(const QString& fileFullName){
    zlog.trace("zConversionMapLoad: " + fileFullName);
    zConversionMap e;
    QString fileName = zFileNameHelper::getfileName(fileFullName);  
    e.name = fileName;

    load(fileFullName, &e.list);
    return e;
}

/**
ha az lenne a cél, hogy tovább keres, és  felülírja, akkor kellene egy sorrend,
amit leginkább egy fa ábrázolna, és annak az ágnak, ami a keresett kulcsot tartalmazza,
kellene kiszedni az értékét
*/

QString zConversionMap::external(const QList<zConversionMap>& maps, const QString& c){
    if(c.isEmpty()) return zStringHelper::Empty;
    QString e;
//QString v = zStringHelper::
    zforeach(m, maps)
    {
        e = m->external(c);
        if(!e.isEmpty()) break;
    }
    return e;
}

/*
*/

QString zConversionMap::external(const QString& c) const {
    QString nk = zStringHelper::zNormalize(c);
    //QString e;
    /*if(this->map.contains(cn))
        e = this->map[cn];*/
    zforeach(m, list)
    {
        if(m->direction.isEmpty()||m->direction.contains('>'))
        {
            auto nk0 = zStringHelper::zNormalize(m->internal);
            if(nk0==nk) return m->external;
        }
    }
    return zStringHelper::Empty;
}

//QStringList zConversionMap::keys(const QList<zConversionMap>& maps){
//    QStringList e;
//    zforeach(m, maps){
//        e << m->map.keys();
//        }
//    return e;
//}

QStringList zConversionMap::internals(const QList<zConversionMap>& maps, const QString& value){
    QStringList e;
    zforeach(m, maps)
    {
//        if(value.isEmpty())
//        {
//             e << m->key();
//        }
//        else
//        {
//            e << m->key(value);
//        }
        e << m->internals(value);
    }
    e.removeDuplicates();
    return e;
}

QStringList zConversionMap::internals(const QString& c) const {
    QStringList e;
    //auto e = this->keys(value);
    //return e;
    QString nv = zStringHelper::zNormalize(c);
    zforeach(m, list)
    {
        if(m->direction.isEmpty()||m->direction.contains('<'))
        {
            auto nv0 = zStringHelper::zNormalize(m->external);
            if(c.isEmpty() || nv0==nv) e << m->internal;
        }
    }
    return e;
}

void zConversionMap::load(const QString& fn, QList<zConversionStruct> *list) {
    zlog.trace(QStringLiteral("Beolvasás: %1").arg(fn));
    QFile file(fn);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) return;

    list->clear();
    QTextStream in(&file);

    QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));
    in.setCodec(QTextCodec::codecForName("UTF-8"));

    while (!in.atEnd())
    {
        QString line = in.readLine();
        if(line.isEmpty()) continue;

        if(line.startsWith('#'))
        {
            zlog.trace(line);
        }
        else
        {
            auto v = line.split(zStringHelper::SEP);

            //int ix = line.indexOf(zStringHelper::SEP);
            if (!v.empty()&&v.count()>1)
            {
                //QString v0 = v[0];
                //QString v1 = v[1];
                //QString v2 = (v.length()>2)?v[1]:zStringHelper::Empty;
                auto s = zConversionStruct(v[0], v[1], (v.length()>2)?v[2]:zStringHelper::Empty);
                list->append(s);
            }
        }
    }
    file.close();

    zlog.ok(QStringLiteral("Beolvasva: %1").arg(fn));
}
