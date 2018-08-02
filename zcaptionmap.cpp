#include "globals.h"
#include "zcaptionmap.h"
#include "zfilenamehelper.h"
#include "zstringhelper.h"
#include "zstringmaphelper.h"

zConversionMap::zConversionMap() = default;


const QString zConversionMap::Empty = QStringLiteral("?");

QList<zConversionMap> zConversionMap::loadAll(const QString& filePath, const QStringList& fileNameFilters){
    QList<zConversionMap> e;

    QStringList files = zFileNameHelper::FindFileNameInDir(filePath, zStringHelper::Empty, fileNameFilters);

    zforeach(f, files){
       auto m = load(*f);
       if(!m.map.isEmpty()){
           e << m;
       }
    }

    return e;
}

zConversionMap zConversionMap::load(const QString& fileFullName){
    zConversionMap e;

    QString fileName = zFileNameHelper::getfileName(fileFullName);

    QString c = fileName.right(fileName.length()-7);
    if(c.isEmpty()){
        e.name = Empty;
    }
    else{
        if(c.startsWith('_')){
            e.name = c.right(c.length()-1);
        }
        else{
            zlog.error(QStringLiteral("Hibás caption file név: %1").arg(fileName));
        }
    }

    if(!e.name.isEmpty()){
        zlog.error("load captions: " + e.name);
        zStringMapHelper::StringMapFeltolt(fileFullName, &e.map);
    }

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
    zforeach(m, maps){
        e = m->value2(c);
        return e;
        }
    return e;
}

QString zConversionMap::value2(const QString& c) const {
    QString cn = c.toLower();
    QString e;
    if(this->map.contains(cn))
        e = this->map[cn];
    return e;
}
