#include <QSqlQuery>
//#include <QVector>
//#include <QMap>
#include <QVariant>

#include "globals.h"
#include "ztablerow.h"
#include "ztable.h"
#include "zstringhelper.h"
#include "zstringmaphelper.h"
#include "zpluralize.h"
#include "zxmlhelper.h"
#include "zsourcehelper.h"
#include "zfilenamehelper.h"
#include "ztextfilehelper.h"

#include <QDir>
#include <QRegularExpression>
#include <QXmlStreamWriter>

zTable::zTable(){};

zTable::~zTable(){};

zTable::zTable(QString _classname, QString pkn, QList<zTablerow> tr, int type, QString _tablename, QString _sourcepath){

    this->rows = tr;
    this->pkname = pkn;
    this->sourcetype = type; 
    this->sourcepath = _sourcepath;

    if(_classname.isEmpty()&&_tablename.isEmpty()){
        int n = ztables.count();
        _classname = QString("class_%1").arg(n);
        _tablename = QString("table_%1").arg(n);
    }
    else if(_classname.isEmpty()){
        _classname = _tablename;//zPluralizer::Singularize(_tablename);
    }


    bool isSingular = zPluralizer::IsSingular(_classname);

    switch (type) {
        case SQL:{ // n táblanév        
            if(isSingular){
                this->classname = zStringHelper::getClassNameCamelCase(_classname);
                this->classname_plural = zPluralizer::Pluralize(classname);
                }
            else{
                QString sn = zPluralizer::Singularize(_classname);
                this->classname = zStringHelper::getClassNameCamelCase(sn);
                this->classname_plural = zPluralizer::Pluralize(classname);
                }
            }
            break;
        case TXT:{
            if(isSingular){
                this->classname = _classname;
                this->classname_plural = zPluralizer::Pluralize(_classname);
            }else{
                this->classname = zPluralizer::Singularize(_classname);
                this->classname_plural = _classname;
            }
            }
            break;
    }
    if(!_tablename.isEmpty())
        this->tablename = _tablename;
    else
        this->tablename = classname_plural;
}




QString zTable::toString(){
    QString rs;
    QString ps;
    zforeach(r, this->rows){
        if(!rs.isEmpty()) rs+=",";
        if(r->colName==this->pkname) rs+="PK:";
        rs+=r->toString();
    }

//    zforeach(p, this->props){
//        if(!rs.isEmpty()) ps+=",";
//        //if(p->colName==this->pkname) ps+="PK:";
//        ps+=p->toString();
//    }

    return  this->tablename+"("+rs+")"+(!ps.isEmpty()?ps:"");
}


QList<QString> zTable::Validate(zTable tv){
    QList<QString> e;

    if(this->tablename!=tv.tablename)
        e.append("Tablename: NOT_EQUALS ERROR");
    else
        e.append("Tablename: OK");

    zforeach(rv,tv.rows){
        zTablerow* r = zTablerow::getByName(&rows, rv->colName);
        if(r != nullptr)
           e.append(r->Validate(rv.operator->()));
        else
           e.append(QString("row: '%1' NOT_EXIST ERROR").arg(rv->colName));
    }
    return e;
}

zTable* zTable::getByName(QList<zTable> *tables, QString rn){
    if(rn.isEmpty()) return nullptr;
    zforeach(r,*tables){
        if(!r->tablename.isEmpty())
            if(r->tablename == rn){
                zTable *r2 = r.operator->();
                return r2;
            }
        }
    return nullptr;
}


QString zTable::getPkByName(QList<zTable> *tables, QString rn){
    if(rn.isEmpty()) return nullptr;

    zforeach(r,*tables){
        if(!r->tablename.isEmpty())
            if(r->tablename == rn){
                QString r2 = r.operator->()->pkname;
                return r2;
            }
        }
    return nullptr;
}

/*
Inventory
Id,int,key,Identity
Name,String,30
OperationTypeId,int
...
*/

/*
A sorból kiszerzi a típust - mérettel/hosszal együtt
ezt1: típus név/leíró

ha ?-re végződik, vagy szerepel benne, hogy nullable, akkor nullable lesz - függetlenül attól, hogy required-e
ezt külön kell vizsgálni

ha isRequired, akkor nem lehet nullable
egyébként akkor nullable, ha az elő van írva
*/
bool zTable::getType(QString ezt1,  QString *dtype, int *dlen, bool *nullable, bool isRequired)
{
    auto re_dlen1 = QRegularExpression(R"((?:\(([\d]+)\)))", QRegularExpression::MultilineOption|QRegularExpression::UseUnicodePropertiesOption);
    auto re_dlen2 = QRegularExpression(R"(([\d]+))", QRegularExpression::MultilineOption|QRegularExpression::UseUnicodePropertiesOption);

    auto re_isnullable = QRegularExpression(R"(Nullable\s*<\s*(\w+)\s*>|([\w\S]+)\?)", QRegularExpression::MultilineOption|QRegularExpression::UseUnicodePropertiesOption);
    //
    auto m_isNullable = re_isnullable.match(ezt1);
    QString typeName;

    if(isRequired){
        *nullable = false;
        typeName = ezt1;
    }
    else{
        if(m_isNullable.hasMatch()){
            *nullable = true;
            typeName = getFirstNotNull(m_isNullable, 2);
        } else{
            *nullable = false;
            typeName = ezt1;
        }
    }

    bool isDtype = false;

    if(zStringMapHelper::contains(&typeMap, typeName)){
        QString k = zStringMapHelper::getKey(&typeMap, typeName);
        *dtype =  typeMap.value(k);;
        isDtype = true;
    }
    else if(zStringMapHelper::contains(&typeMapR, typeName)){
        QString k = zStringMapHelper::getKey(&typeMapR, typeName);
        *dtype =  typeMapR.value(k);;
        isDtype = true;
    }
    else {//
        auto i2 = re_dlen1.match(typeName);
        if(i2.hasMatch()){
            bool isOK;
            int n = i2.captured(1).toInt(&isOK);
            if(isOK) *dlen = n;
            }
        else{
             i2 = re_dlen2.match(typeName);
             if(i2.hasMatch()){
                 bool isOK;
                 int n = i2.captured(1).toInt(&isOK);
                 if(isOK) *dlen = n;
                 }
             }
         }

    if(typeName.isEmpty()){
        zlog.log(QString("getType: Unknown type: %1").arg(ezt1), zlog.ERROR);
    }

    return isDtype;
}

/*

*/
QStringList zTable::getFK(){
    QStringList fknames;
    zforeach(t, ztables){
        QString pn = t->classname+t->pkname;
        if(containsRow(pn))
           fknames<<pn;
        }
    return fknames;
}

QStringList zTable::getFKClassName(){
    QStringList fknames;
    zforeach(t, ztables){
        QString pn = t->classname+t->pkname;
        if(containsRow(pn))
           fknames<<t->classname;
        }
    return fknames;
}

bool zTable::containsRow(QString n){
    zforeach(r, this->rows)
        if( r->colName.toLower()==n.toLower() ) return true;
    return false;
}


void zTable::toXML(QXmlStreamWriter *s)
{  

    s->writeStartElement(nameof(zTable));
    s->writeAttribute(nameof(this->sourcetype), QString::number(this->sourcetype));
    s->writeAttribute(nameof(this->sourcepath), this->sourcepath);

    s->writeAttribute(nameof(this->tablename), this->tablename);
    s->writeAttribute(nameof(this->classname), this->classname);
    s->writeAttribute(nameof(this->classname_plural), this->classname_plural);
    s->writeAttribute(nameof(this->pkname), this->pkname);
    s->writeAttribute(nameof(this->name_formatstring), this->name_formatstring);
    s->writeAttribute(nameof(this->comment), this->comment);
    s->writeAttribute(nameof(this->updateTime), this->updateTime.toString());

    s->writeStartElement(nameof(this->rows));
    zforeach(r, this->rows){ (*r).toXML(s); }
    s->writeEndElement();

    s->writeEndElement();
}

QList<zTable> zTable::createTableByXML(const QString& txt){
   QList<zTable> tl;

    QXmlStreamReader xml(txt);

//    while(!xml.atEnd()){
//        xml.readNext();
//        if(xml.isStartElement() && (xml.name() == nameof(zTable))){

    if (xml.readNextStartElement()){
        if(xml.name() == "zTables"){
            while(xml.readNextStartElement()) {
                if(xml.name()==nameof(zTable)){
                    zTable t = fromXML(&xml);
                    tl.append(t);
                }
                else{
                    xml.skipCurrentElement();
                }
            }
        }
        else if(xml.name() == "zTable"){
            zTable t = fromXML(&xml);
            tl.append(t);
        }
        else{
            xml.skipCurrentElement();
        }
    }
    if(xml.hasError()){
        zlog.log("createTableByXML: "+xml.errorString());
        }
    return tl;
}

zTable zTable::fromXML(QXmlStreamReader* xml){
    zTable t;

    auto a = xml->attributes();

    zXmlHelper::putXmlAttr(a, nameof(tablename), &(t.tablename));
    zXmlHelper::putXmlAttr(a, nameof(sourcetype), &(t.sourcetype));
    zXmlHelper::putXmlAttr(a, nameof(sourcepath), &(t.sourcepath));
    zXmlHelper::putXmlAttr(a, nameof(classname), &(t.classname));
    zXmlHelper::putXmlAttr(a, nameof(classname_plural), &(t.classname_plural));
    zXmlHelper::putXmlAttr(a, nameof(pkname), &(t.pkname));
    zXmlHelper::putXmlAttr(a, nameof(name_formatstring), &(t.name_formatstring));
    zXmlHelper::putXmlAttr(a, nameof(updateTime), &(t.updateTime));

    if (xml->readNextStartElement() && xml->name() == "rows"){
            t.rows = QList<zTablerow>();

            while(xml->readNextStartElement()) {
                if(xml->name()==nameof(zTablerow)){
                    auto r = zTablerow::fromXML(xml);
                    t.rows.append(r);
                    QString txt = xml->readElementText();
                    }
                else{
                    xml->skipCurrentElement();
                }

            }
            xml->readNextStartElement();
        }

    //t.props = QList<zTablerow>();
    //tl.append(t);
    zlog.log("XML beolvasva: "+ t.tablename +xml->errorString());
    return t;
}

/*
Minden entitás minden mezőjének figyelembevételével
meg kell keresni névegyezés alapján
hogy az elsődleges kulcs milyen entitásokban szerepel mezőként
*/
QStringList zTable::getRFK(){
    QStringList rfknames;
    QString pn = classname+pkname;

    zforeach(t, ztables){
        if(t->containsRow(pn))
            rfknames<<t->classname;
    }
    return rfknames;
}

QStringList zTable::getRFKClassNamePlural(){
    QStringList rfknames;
    QString pn = classname+pkname;

    zforeach(t, ztables){
        if(t->containsRow(pn))
            rfknames<<t->classname+';'+t->classname_plural;
    }
    return rfknames;
}
    //QString pk = zStringHelper::toCamelCase(this->tablename+'.'+this->pkname);

//    zforeach(r, this->rows){ml<<(*(r)).colName;}

//    zforeach(pk, pks){

//        QString pn = zStringHelper::toCamelCase(*(pk));

//        if(ml.contains(pn))
//            this->fknames<<pn;
//        else{
//            auto pks = zStringHelper::singularizeAll(*(pk));
//            pn = zStringHelper::toCamelCase(pks);
//            if(ml.contains(pn))
//                this->fknames<<pn;
//        }
//    }
//}
/*
#Adm
DateCre,DateTime
DateMod,datetime


Inventory
Id,int,key,Identity
Name,String,30
OperationTypeId,int
InventoryStatusId,int
StorageId,int
DateStart,DateTime,nullable
DateEnd,datetime,nullable
Adm
RecountRequired,bool
UserId,guid
Comments,string,200

osztálynév
mezőnév,további leírók(típus, hossz, nullable, végsősoron a típust írják le), a legutolsó a caption?
*/

// TODO globális caption tábla, ha ezáltal feloldható a mezőnév, és nincs egyéb caption,  akkor ez lesz
QList<zTable> zTable::createTableByText(QString txt)
{
//    auto re = QRegularExpression(R"((?:^\s+)?(^(?:\s+)?\w*\s+)((?:^[\w\,\ \(\)\"\']*(?:\s+)?)+)(?:$|^\s+)?)", QRegularExpression::MultilineOption|QRegularExpression::UseUnicodePropertiesOption);
    auto re = QRegularExpression(R"(^\s*(?:(^\w*)\s+)((?:^[\w, ()\"'<>\.]+\n?)+))", QRegularExpression::MultilineOption|QRegularExpression::UseUnicodePropertiesOption);
    auto re_dlen1 = QRegularExpression(R"((?:\(([\d]+)\)))", QRegularExpression::MultilineOption|QRegularExpression::UseUnicodePropertiesOption);
    auto re_dlen2 = QRegularExpression(R"(([\d]+))", QRegularExpression::MultilineOption|QRegularExpression::UseUnicodePropertiesOption);
    auto re_dlen3 = QRegularExpression(R"((?:(\w+)\s*\(([\d]+)\)))", QRegularExpression::MultilineOption|QRegularExpression::UseUnicodePropertiesOption);
    //(?:(\w+)\s*\(([\d]+)\))
    auto re_caption = QRegularExpression(R"((?:\"([\w]+)\"))", QRegularExpression::MultilineOption|QRegularExpression::UseUnicodePropertiesOption);
    auto re_nullable = QRegularExpression(R"((?:((?:not\s*)?(?:nullable|null))))", QRegularExpression::MultilineOption|QRegularExpression::UseUnicodePropertiesOption);

    auto re_macro_def = QRegularExpression(R"(^\s*(?:^#(\w*)\s+)((?:[\w, ()\"'<>\.]+\s)+))", QRegularExpression::MultilineOption|QRegularExpression::UseUnicodePropertiesOption);
//    auto re_macro_use = QRegularExpression(R"((?:^([\w]+)\s*$))", QRegularExpression::MultilineOption|QRegularExpression::UseUnicodePropertiesOption);
    auto re_macro_use_tmp = QString(R"((?:^(%1)\s*$))");

    auto j = re_macro_def.globalMatch(txt);

    while(j.hasNext()){
        QRegularExpressionMatch m = j.next();
        QString m_name=m.captured(1);
        QString m_txt=m.captured(2).trimmed();

        if(!macroMap.contains(m_name)){
            macroMap.insert(m_name, m_txt);
            zlog.trace(QString("Macro def: %1").arg(m_name));
            }
    }

    //QString txt2;
    auto keys = macroMap.keys();
    zforeach(m, keys){
        auto mcr = QRegularExpression(re_macro_use_tmp.arg(*m), QRegularExpression::MultilineOption|QRegularExpression::UseUnicodePropertiesOption);

        //if(txt.contains(mcr)){
            txt.replace(mcr,macroMap.value(*m));
        //}
    }



    auto i = re.globalMatch(txt);
    QList<zTable> tl;
    if(i.hasNext()){
        while(i.hasNext()){
            QRegularExpressionMatch m = i.next();
            QString tn=m.captured(1).trimmed();
            QString pkn = "id";
            auto fns=m.captured(2).split(QRegularExpression(R"([\n|\r\n|\r])"), QString::SkipEmptyParts);
            QList<zTablerow> rl;
            QList<zTablerow> pl;

            zforeach(fn, fns){
               if(fn->isEmpty()) continue;
               if(*fn=="_") continue;

               QString dtype="";
               int dlen = 0;
               bool isNullable = true;
               QString caption = "";
               QString ezt1;

               auto fns = fn->split(',', QString::SkipEmptyParts);
               QString fname = fns[0].trimmed();

               if(fns.length()>1){
                   zforeach_from(fn2, fns, 1){
                       auto fn3s= fn2->split(' ', QString::SkipEmptyParts);
                       bool isDtype = false;
                       zforeach(fn3, fn3s){

                           auto i3 = re_dlen3.match(*fn3);
                           if(i3.hasMatch()){
                               ezt1 = i3.captured(1);
                               bool isOK;
                               int n = i3.captured(2).toInt(&isOK);
                               if(isOK) dlen = n;
                               }
                           else{
                               ezt1 = *fn3;
                           }
                           //típus vizsgálat
                           isDtype = zTable::getType(ezt1, &dtype, &dlen, &isNullable, false);
                           }
                       // TODO bonyolult típusmeghatározás
                       if(isDtype==false){
                            auto i2 = re_caption.match(*fn2);
                            if(i2.hasMatch())
                               caption = i2.captured(1);
                            else{
                                auto i2 = re_dlen2.match(*fn2);
                                if(i2.hasMatch()){
                                    bool isOK;
                                    int n = i2.captured(1).toInt(&isOK);
                                    if(isOK) dlen = n;
                                    }
                                else{
                                    auto i2 = re_nullable.match(*fn2);
                                    if(i2.hasMatch()){
                                        auto n_str = i2.captured(1);
                                        if(n_str.toLower().contains("not"))
                                            isNullable = false;
                                        else
                                            isNullable = true;
                                        }
                                    else{
                                        if((*fn2).toLower()=="key"){
                                            pkn = fname;
                                            }
                                        else if((*fn2).toLower()=="required"){
                                            isNullable = false;
                                            }
                                        }
                                    }
                                }
                            /**/
                            }
                        }
                    }
               if(dtype.isEmpty()){
                   // auto p = zTablerow(fname, "property", dlen, isNullable, caption);
                   // pl.append(p);
                   zlog.log(QString("nem meghatárizható típus: %1").arg(ezt1));
                   }
               else{
                   auto r = zTablerow(fname, dtype, dlen, isNullable, caption);
                   rl.append(r);
                   }
            }
            auto t = zTable("", pkn, rl,  TXT, tn, "");
            tl.append(t);
            zlog.log("GenerateByText: "+t.toString());
        }
    }
    return tl;
}


/*
#Adm
DateCre DateMod datetime

class1
mezo1 mezo2 mezo3 string
mezo4 mezo5 int
int mezo6 mezo7
Adm

// TODO globális caption táblát kell használni
*/
QList<zTable> zTable::createTableByText_2(QString txt){
    auto re = QRegularExpression(R"(^\s*(?:(^\w*)\s+)((?:^[\w, ()\"'<>\.]+\n?)+))", QRegularExpression::MultilineOption|QRegularExpression::UseUnicodePropertiesOption);

    auto re_macro_def = QRegularExpression(R"(^\s*(?:^#(\w*)\s+)((?:[\w, ()\"'<>\.]+\s)+))", QRegularExpression::MultilineOption|QRegularExpression::UseUnicodePropertiesOption);
    auto re_macro_use_tmp = QString(R"((?:^(%1)\s*$))");

    auto j = re_macro_def.globalMatch(txt);

    while(j.hasNext()){
        QRegularExpressionMatch m = j.next();
        QString m_name=m.captured(1);
        QString m_txt=m.captured(2).trimmed();

        if(!macroMap.contains(m_name)){
            macroMap.insert(m_name, m_txt);
            zlog.trace(QString("Macro def: %1").arg(m_name));
            }
        }

    auto keys = macroMap.keys();
    zforeach(m, keys){
        auto mcr = QRegularExpression(re_macro_use_tmp.arg(*m), QRegularExpression::MultilineOption|QRegularExpression::UseUnicodePropertiesOption);

        //if(txt.contains(mcr)){
            txt.replace(mcr,macroMap.value(*m));
        //}
    }


    auto i = re.globalMatch(txt);
    QList<zTable> tl;
    if(i.hasNext()){
        while(i.hasNext()){
            QRegularExpressionMatch m = i.next();
            QString tn=m.captured(1).trimmed();
            QString pkn = "id";
            auto fns=m.captured(2).split(QRegularExpression(R"([\n|\r\n|\r])"), QString::SkipEmptyParts);
            QList<zTablerow> rl;
            //QList<zTablerow> pl;

            zforeach(fn, fns){ // sorok
               if(fn->isEmpty()) continue;
               if(*fn=="_") continue;

               QString dtype="";
               int dlen = 0;
               bool isNullable = true;
              // QString caption = "";

               auto fns = fn->split(' ', QString::SkipEmptyParts);

               //zlog.trace("sor:"+*fn);

               if(fns.length()>2){ //szavak (mezők)
                    bool isDtype = false;
                    QList<zTablerow> pls;

                    zforeach(fn2, fns){
                        //zlog.trace("szó:"+*fn2);
                        // todo vizsgálni, típus-e, ha igen, megvan. (string - hossz)
                        // amúgy mezőnév lesz
                        // ezért megy a mezőnév listába
                        isDtype = zTable::getType(*fn2, &dtype, &dlen, &isNullable, false);
                        if (isDtype){
                            zlog.trace("sortípus:"+*fn2);
                        }
//                        if (!isDtype){
//                            //dtype = "property";
//                            //dlen = 0;

//                            auto r = zTablerow(*fn2, "property", 0, false, "");
//                            pls.append(r);
//                        }
//                        else{
//                            zlog.trace("sortípus:"+*fn2);
//                        }
                    }

                    zforeach(p, pls){
                        if(!dtype.isEmpty()){
                            p->colType = dtype;
                            p->dlen = dlen;
                            rl.append(*p);
                            }
                        else{
                           // pl.append(*p);
                        }

                    }


                    // ha van mezőnév lista
                    // ha van ismert típus, akkor mezőlistához,
                    // ha nincs ismert a típus,
                    // a property listához adjuk hozzá

               }


            }
            auto t = zTable("", pkn, rl,  TXT, tn, "");
            tl.append(t);
            zlog.log("GenerateByText2: "+t.toString());
        }
    }

    return tl;
}

//QString zTable::p_class = zSourceHelper::p_class.arg(R"(\w+)");//QString(R"(class\s+(\w+)\s+(\{(?>[^{}]+|(?2))*\}))");
//QString zTable::p_attr = QString(R"((?:\[[(.\w)]*\]))");

//QRegularExpression zTable::r_class_or_attr = QRegularExpression(p_attr+"|"+p_class, QRegularExpression::MultilineOption|QRegularExpression::UseUnicodePropertiesOption);
//QRegularExpression zTable::r_class = QRegularExpression(p_class, QRegularExpression::MultilineOption|QRegularExpression::UseUnicodePropertiesOption);
//QRegularExpression zTable::r_attr = QRegularExpression(p_attr, QRegularExpression::MultilineOption|QRegularExpression::UseUnicodePropertiesOption);

// attributumok és az osztály
// (?:\[[(.\w)]*\])|class\s+(\w+)\s+(\{(?>[^{}]+|(?2))*\})
// 1:név, 2:definíció

// attributumok és propertyk
// \[[(.\w)]*\]|public\s+(\w+)\s+(\w+)(?:\s*{.*})
// 1: típus, 2: név

// string vagy osztály utolsó tagja
//\"([\p{L}]+)\"|(?:[\p{L}0-9.]+)\.([^.][\w]+)
// 1:string 2:osztály

// a project Entity könyvtárának fáljai - felolvasás
// attributumok a => Entity konverziónak megfelelően

// ha egy attribútum értéke osztálynév literális helyett, akkor mégegy menetre lesz szükség,
// az elsőben kiértékelünk és begyűjtjük a (feloldatlan) konstansokat
// persze ha vannak konstansok, el kell dobjuk a kiértékelés eredményét
// begyűjtjük a konstans értékeket a kiértékeléshoz gyűjtött kulcsokhoz
// így azok a továbbiakban kulcs-érték párt alkotva
// egy második menetben  - teljesen újrafeldolgozva az adatokat
// már olyan feldolgozási eredményt kapunk, ami teljes, így nem kell már eldobnunk
// ugyanaz a feldolgozó - ugyanaz az implementáció fut kétszer, tehát nem szükséges
// egy feldolgozatlan string - attribútumérték és egy már feldolgozott (pl dlen esetén int) konstansfeldolgozó
// bár arra garanciát nem ad, hogy egy int attribútum argumentum helyére nem-e rakunk be string konstanst
// azzal a számvetéssel, hogy a forrás rendszerben ez hibás kódot eredményez
//
// illetve az első - attribútum argumentum vizsgálat, azaz konstans kulcsok legyűjtése
// kiszervezhető önálló függvényként is, így az első menet szerepét a továbbiakban az átveheti

QList<zTable> zTable::createTableByText_3(QString txt, QMap<QString, QString>* constMap, QMap<QString, QString>* valueMap)
{
    QList<zTable> tl;
    zlog.log("GenerateByText3: ");
    // attributumok és az osztály
    // 1:név, 2:definíció
    auto re = QRegularExpression(R"((?:\[[(.\w)]*\])|class\s+(\w+)\s+(\{(?>[^{}]+|(?2))*\}))", QRegularExpression::MultilineOption|QRegularExpression::UseUnicodePropertiesOption);

    // attributumok és propertyk
    // 1: típus, 2: név
    auto r_attrOrProp = QRegularExpression(R"(\[[(.\w)]*\]|public\s+(\S+)\s+(\w+)(?:\s*{.*}))", QRegularExpression::MultilineOption|QRegularExpression::UseUnicodePropertiesOption);

   // auto r_tablanev = QRegularExpression(R"(\"([\p{L}]+)\"|(?:[\p{L}0-9.]+)\.([^.][\w]+))");
   //\[[(.\w)]*\]|public\s+(\p{L}[\p{L}0-9_\-]+)\s+(\w+)(?:\s*{.*})

    auto r_tablanev2 = QRegularExpression(R"(\"([\p{L}]+)\"|(?:[\p{L}0-9.]+))");

    QStringList classAttrs;

    auto i = re.globalMatch(txt);
        while(i.hasNext()){

            QRegularExpressionMatch m = i.next();

            QString g=m.captured(0);
            if(g[0]=='['){

                classAttrs.append(g);
                }
            else if(g[0]=='c'){
                QString tableName;
                QStringList propAttrs;
                QString className = m.captured(1);
                QString class_txt = m.captured(2);
                QString pkName = "";
                zlog.log("class: " + className);

                if(!classAttrs.isEmpty()){
                    zforeach(a, classAttrs){
                        //zlog.log("classAttr: "+(*a));
                        auto attrParams = getAttrAndParams((*a));
                        QString attrname = attrParams[0];
                        if(attrname=="Table"){
                            // ha szöveg konstans, az, ha nem, akkor osztály.
                            //tableName = getConstFromArgument(attrParams[1]);
                            tableName = attrParams[1];
                            if(zStringHelper::isClassName(tableName)){
                                QString key = className+','+attrname;
                                if(constMap->contains(key)){
                                    QString key2 = constMap->value(key);
                                    tableName = valueMap->value(key2);
                                }else{
                                    constMap->insert(key, tableName);
                                    tableName="";
                                    }
                                }
                            if(!tableName.isEmpty())
                                tableName = getConstFromArgument(tableName);

                        }
                        else{
                            zlog.log(QString("Unknown TableAttr: %1").arg(*a), zlog.ERROR);
                            }
                        }
                    classAttrs.clear();
                    }

                QList<zTablerow> rl;

                auto i_attrOrProp = r_attrOrProp.globalMatch(class_txt);
                //if(i2.hasNext()){
                    while(i_attrOrProp.hasNext()){
                        QRegularExpressionMatch m_attrOrProp = i_attrOrProp.next();

                        QString attrOrProp=m_attrOrProp.captured(0);
                        if(attrOrProp[0]=='['){
                            propAttrs.append(attrOrProp);
                            //zlog.log("attr: "+attrOrProp);
                        }
                        else if(attrOrProp[0]=='p'){
                            bool isPk = false;
                            bool isRequired = false;
                           // QString MaxLength;
                            QString Caption  = "";
                            QString dtype="";
                            int dlen = 0;
                            bool isNullable= false;

                            QString propType = m_attrOrProp.captured(1);
                            QString propName = m_attrOrProp.captured(2);

                            if(!propAttrs.isEmpty()){
                                zforeach(a, propAttrs){
                                    //zlog.log("propAttr: "+(*a));
                                    auto attrParams = getAttrAndParams((*a));
                                    QString attrname = attrParams[0];
                                    if(attrname=="Key"){
                                        isPk = true;
                                        pkName = propName;
                                    }
                                    else if(attrname=="Required") {
                                        isRequired = true;
                                    }
                                    else if(attrname=="MaxLength"){
                                        QString MaxLength = attrParams[1];
                                        if(zStringHelper::isClassName(MaxLength)){
                                            QString key = propName+','+attrname;
                                            if(constMap->contains(key)){
                                                QString key2 = constMap->value(key);
                                                MaxLength = valueMap->value(key2);
                                            }else{
                                                constMap->insert(key, MaxLength);
                                                MaxLength="";
                                                }
                                            }
                                        if(!MaxLength.isEmpty())
                                            dlen = MaxLength.toInt();

                                        }
                                    else{
                                        zlog.log(QString("Unknown PropertyAttr: %1").arg(*a), zlog.ERROR);
                                        }
                                    }
                                propAttrs.clear();
                                }


                            //QString row = m_attrOrProp.captured(0);
                            bool isDtype = zTable::getType(propType, &dtype, &dlen, &isNullable, isRequired);

//                            if(isRequired){
//                                isNullable = false;
//                            }
//                            if (isDtype){
//                                zlog.trace("sortípus:"+dtype);
//                            }


//                            zlog.log("prop: "+propType + " " +propName);
//                            zlog.log("   isPK: "+((isPk)?QString("true"):QString("false")));
//                            zlog.log("   isRequired: "+((isRequired)?QString("true"):QString("false")));
//                            zlog.log("   MaxLength: "+MaxLength);

                            auto r = zTablerow(propName, dtype, dlen, isNullable, Caption);
                            rl.append(r);
                        }
                        else{
                            zlog.log("undefined: "+attrOrProp);
                        }

                    }
                //}                   
                   auto t = zTable(className, "id", rl,  TXT, tableName, "");
                   tl.append(t);
                   zlog.log("GenerateByEntity: "+t.toString());
            }
            else{
                zlog.log("undefined: "+g);
            }

            // attributumok és az osztály
        }   
    return tl;
}

// attributum és az argumentumlista
// \[\s*(\w+)(\((?>[^)(]+|(?2))*\))\s*\]
// 1: argName, 2: argumentumok
// (\"?\b[\p{L}0-9. ]+\b\"?(\([\w(), "]*\))*)
// 1: paraméter, 2: paraméterargumentum
// az egyes argumentumok

QStringList zTable::getAttrAndParams(QString str){
    //attributum és az argumentumlista
    auto r= QRegularExpression(R"(\[\s*(\w+)(\((?>[^)(]+|(?2))*\))?\s*\])");
    auto r2= QRegularExpression(R"((\"?\b[\p{L}0-9. ]+\b\"?(\([\w(), "]*\))*))");

    QStringList e;

    auto i = r.globalMatch(str);
    if(i.hasNext()){
        QRegularExpressionMatch m = i.next();
        e.append(m.captured(1));
        QString paramstr = m.captured(2);
        if(!paramstr.isEmpty()){
            auto i2 = r2.globalMatch(paramstr);
            while(i2.hasNext()){
                QRegularExpressionMatch m2 = i2.next();
                e.append(m2.captured(1));
            }
        }        
    }
    return e;
}

QString zTable::getFirstNotNull(QRegularExpressionMatch m, int max){
    for(int j=1;j<=max;j++){
        if(!m.captured(j).isEmpty()){
            return m.captured(j);
        }
    }
    return "";
}

/*
  (\w+)\s*=\s*(\"?[^\"][\p{L} \S]*\"?)\;
 */

QString zTable::getConstFromArgument(QString str){
    auto r_class = zSourceHelper::getRegex_r_class();

    if(str.startsWith('\"')||str.endsWith('\"')){
        return str.mid(1, str.length()-2);
    } else{
        QString classname = str.section('.', 0);

        QStringList constlist;
        /**/


        auto i_class = r_class.globalMatch(str);
        while(i_class.hasNext()){
           QRegularExpressionMatch m_class = i_class.next();

        }

    }
    return str;
}


QList<zTable> zTable::createTableByClassTxt(QString txt){

    QMap<QString, QString> constNameMap;

    auto tl = zTable::createTableByText_3(txt, &constNameMap);

    if(tl.length()==0) { zlog.log("nem jött létre adat"); return tl;}

    auto db = beallitasok.getSelectedDbConnection();
    if(db==nullptr){
        // szükség van adatbázisra, a project könyvtár meghatározásához - a project könyvtár neve az adatbáziséval egyezik meg
        return tl;
    }

    // konstanstábla beolvasása
    auto path = zFileNameHelper::append(QDir::homePath(),beallitasok.munkadir,db->adatbazisNev);

    // key az attrName, value a constName
    QStringList classNameFilter;

    QStringList constNameList;// = constNameMap.values();


    zforeach(m, constNameMap){
        auto className = m.value().split('.').first();
        auto clf = className+".c?";
        if(!classNameFilter.contains(clf))
            classNameFilter.append(clf);

        if(!constNameList.contains(m.value()))
            constNameList.append(m.value());
    }

    QStringList files = zFileNameHelper::FindFileNameInDir(path, "Data", classNameFilter);

    QMap<QString, QString> constValueMap;

    if(files.count()>0){
        zforeach(f, files){
            zSourceHelper::getConstValuesFromFile(*f, constNameList, &constValueMap);
        }
    }

    // constNameMap.value mint constValueMap.key ad egy értéket, ez alapján kell rendezni a
    // tl constNameMap.key propertyket, attribútumokat
    // createTableByText_3 - az attributum értékadást függvénybe ki kell emelni, így utólag is hívható lesz

    //
    if(!constNameMap.isEmpty())
        tl = zTable::createTableByText_3(txt, &constNameMap, &constValueMap);

    return tl;
}

/*
lementi a  táblát - nem caption, hanem teljes xml, így ez később átnevezendő
*/
void zTable::saveTablaToXML() {
    auto b = beallitasok.getSelectedDbConnection();
    if(b==nullptr) return;

    QString fn = zFileNameHelper::append(QDir::homePath(),beallitasok.munkadir,b->adatbazisNev, this->tablename + ".xml");

    QString e;
    QXmlStreamWriter s(&e);
    s.setAutoFormatting(true);
    s.writeStartDocument();

    this->toXML(&s);
    s.writeEndDocument();

    zTextFileHelper::save(e, fn);
}


