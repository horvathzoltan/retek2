#include "globals.h"
#include "zconversionmap.h"
#include "zfilenamehelper.h"
#include "zstringhelper.h"
#include "zstringmaphelper.h"

zConversionMap::zConversionMap() = default;


//const QString zConversionMap::Empty = QStringLiteral("?");

QList<zConversionMap> zConversionMap::loadAll(const QString& filePath, const QStringList& fileNameFilters){
    QList<zConversionMap> e;

    QStringList files = zFileNameHelper::FindFileNameInDir(filePath, zStringHelper::Empty, fileNameFilters);

    zforeach(f, files){
       auto m = load(*f);
       if(m.map.isEmpty())
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

    zStringMapHelper::StringMapFeltolt(fileFullName, &e.map);
    return e;
}

/**
ha az lenne a cél, hogy tovább keres, és  felülírja, akkor kellene egy sorrend,
amit leginkább egy fa ábrázolna, és annak az ágnak, ami a keresett kulcsot tartalmazza,
kellene kiszedni az értékét
*/

QString zConversionMap::value(const QList<zConversionMap>& maps, const QString& c){
    if(c.isEmpty()) return zStringHelper::Empty;
    QString e;
//QString v = zStringHelper::
    zforeach(m, maps)
    {
        e = m->value(c);
        if(!e.isEmpty()) break;
    }
    return e;
}

/*
*/

QString zConversionMap::value(const QString& c) const {
    QString nk = zStringHelper::zNormalize(c);
    //QString e;
    /*if(this->map.contains(cn))
        e = this->map[cn];*/
    zforeach(m, map)
    {
        auto nk0 = zStringHelper::zNormalize(m.key());
        if(nk0==nk) return m.value();
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

QStringList zConversionMap::keys(const QList<zConversionMap>& maps, const QString& value){
    QStringList e;
    zforeach(m, maps)
    {
        if(value.isEmpty())
        {
             e << m->map.keys();
        }
        else
        {
            e << m->map.keys(value);
        }
    }
    e.removeDuplicates();
    return e;
}

QStringList zConversionMap::keys(const QString& c) {
    QStringList e;
    //auto e = this->keys(value);
    //return e;
    QString nv = zStringHelper::zNormalize(c);
    zforeach(m, map)
    {
        auto nv0 = zStringHelper::zNormalize(m.value());
        if(nv0==nv) e << m.key();
    }
    return e;
}
