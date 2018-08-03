#include "globals.h"
#include "zconversionmap.h"
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

    //QString c = fileName.right(fileName.length()-7);
//    if(c.isEmpty()){
//        e.name = Empty;
//    }
//    else
//    {
//        if(c.startsWith('_')){
//            e.name = c.right(c.length()-1);
//        }
//        else{
//            zlog.error(QStringLiteral("Hibás caption file név: %1").arg(fileName));
//        }
    //}
    e.name = fileName;
    //if(!e.name.isEmpty()){

        zStringMapHelper::StringMapFeltolt(fileFullName, &e.map);
    //}


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
        e = m->value(c);
        return e;
        }
    return e;
}

QString zConversionMap::value(const QString& c) const {
    QString cn = c.toLower();
    QString e;
    if(this->map.contains(cn))
        e = this->map[cn];
    return e;
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

//QStringList zConversionMap::keys() {
//    QStringList e;
//    this->keys();
//}
