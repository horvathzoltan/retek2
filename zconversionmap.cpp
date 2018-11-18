#include "globals.h"
#include "zconversionmap.h"
#include "zfilenamehelper.h"
#include "zstringhelper.h"
#include "zstringmaphelper.h"

#include <QFileInfo>
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
            zError(QStringLiteral("Nem tartalmaz beolvasható sorokat: %1").arg(*f));
       }
       else
       {
           e << m;
       }
    }

    return e;
}

zConversionMap zConversionMap::load(const QString& fileFullName){
    //zTrace();
    //zInfo(fileFullName);
    zConversionMap e;
    QString fileName = zFileNameHelper::getfileName(fileFullName);  

    load(fileFullName, &e.list);
    e.fullname = fileFullName;
    e.name = fileName;
    e.lastloaded = QDateTime::currentDateTime();

    if(e.list.isEmpty())
    {
        zWarning(QStringLiteral("Nem tartalmaz sorokat: %1").arg(fileName));
    }
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
    //zTrace();zInfo(fn);
    QFile file(fn);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) return;

    list->clear();
    QTextStream in(&file);

    //QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));
    in.setCodec(QTextCodec::codecForName("UTF-8"));

    while (!in.atEnd())
    {
        QString line = in.readLine();
        if(line.isEmpty()) continue;

        if(line.startsWith('#'))
        {
            zInfo(line);
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
                auto s = zConversionStruct(v[0].trimmed(), v[1].trimmed(), (v.length()>2)?v[2]:zStringHelper::Empty);
                list->append(s);
            }
        }
    }
    file.close();

    zInfo(QStringLiteral("Beolvasva: %1").arg(fn));
}


void zConversionMap::reLoadAll(QList<zConversionMap>& maps, const QString& filePath, const QStringList& fileNameFilters){
    //QList<zConversionMap> e;

    QStringList files = zFileNameHelper::FindFileNameInDir(filePath, zStringHelper::Empty, fileNameFilters);
    bool toLoad = true;

    zforeach(f, files){
        auto f_ix = indexByFullName(maps, *f);
        if(f_ix>-1)
        {
            toLoad = false; //  magunk gondoskodunk róla - frissítés
            auto m = maps[f_ix];
            // ha megvan, azaz van ilyen, meginfózzuk, ha van újabb, csere van
            QFileInfo fileInfo(*f);
            if((fileInfo.lastModified()) > (m.lastloaded)) // - frissítés
            {
                auto lm = load(*f);
                maps.replace(f_ix, lm);
            }
        }

        if(toLoad)
        {
            auto m = load(*f);
            maps.append(m);
        }
    }
}

int zConversionMap::indexByFullName(const QList<zConversionMap>& maps, const QString& fullName){
    for(int i = 0;i<maps.count();i++)
    {
        auto m = maps[i];
        if(m.fullname==fullName)
        {
            return i;
        }
    }
    return -1;
}
//zConversionMap zConversionMap::reLoad(const QString& fileFullName){
//    zTrace(zfunc, fileFullName);
//    QString fileName = zFileNameHelper::getfileName(fileFullName);

//    zConversionMap e;

//    load(fileFullName, &e.list);
//    e.name = fileName;
//    e.lastloaded = QDateTime::currentDateTime();

//    return e;
//}
