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
#include "zshortguid.h"

#include <QDir>
#include <QRegularExpression>
#include <QXmlStreamWriter>

zTable::zTable()= default;

zTable::~zTable()= default;

zTable::zTable(const QString& _name, const QString& pkn, const QList<zTablerow>& tr){
    this->name = _name;
    this->pkname = pkn;
    this->rows = tr;

    //this->sourcetype = type;

    //this->sourcepath = _sourcepath;
/*
    switch(type){
        case zTableSourceTypes::SQL:
            this->sql_conn = //_sourcepath;
            break;
        case zTableSourceTypes::ENTITY:
            this->class_path = _sourcepath;
            break;
    }

    if(_class_name.isEmpty()&&_tablename.isEmpty()){
        int n = ztables.count();
        _class_name = QStringLiteral("class_%1").arg(n);
        _tablename = QStringLiteral("table_%1").arg(n);
    }
    else if(_class_name.isEmpty()){
        _class_name = _tablename;//zPluralizer::Singularize(_tablename);
    }


    bool isSingular = zPluralizer::IsSingular(_class_name);

    switch (type) {
        case SQL:{ // n táblanév        
            if(isSingular){
                this->class_name = zStringHelper::getclass_nameCamelCase(_class_name);
                this->class_name_plural = zPluralizer::Pluralize(class_name);
                }
            else{
                QString sn = zPluralizer::Singularize(_class_name);
                this->class_name = zStringHelper::getclass_nameCamelCase(sn);
                this->class_name_plural = zPluralizer::Pluralize(class_name);
                }
            }
            break;
        case TXT:{
            if(isSingular){
                this->class_name = _class_name;
                this->class_name_plural = zPluralizer::Pluralize(_class_name);
            }else{
                this->class_name = zPluralizer::Singularize(_class_name);
                this->class_name_plural = _class_name;
            }
            }
            break;
    }
    if(!_tablename.isEmpty()){
        this->name = _tablename;
    }
    else{
        this->name = class_name_plural;
    }
    */
}

void zTable::initClass(const QString& className, const QString& pluralClassName)
{
    this->class_name = className;
    if(!pluralClassName.isEmpty())
        this->class_name_plural = pluralClassName;
}

QString zTable::getClassName(const QString& n, QString& cnp)
{
    if(n.isEmpty()) return zStringHelper::Empty;

    QString cn;
    bool isNameSingular = zPluralizer::IsSingular(n);

    if(isNameSingular)
    {
        cn = zStringHelper::getclass_nameCamelCase(n);
        //cnp = zPluralizer::Pluralize(n);
        //return cn;
        //this->initClass(cn,cnp);
    }
    else
    {
        QString sn = zPluralizer::Singularize(n);
        cn = zStringHelper::getclass_nameCamelCase(sn);
        //cnp = zPluralizer::Pluralize(cn);
        //this->initClass(cn, cnp);
    }
    cnp = zPluralizer::Pluralize(cn);
    return cn;
}

void zTable::initClassByName()
{
    bool isNameSingular = zPluralizer::IsSingular(name);

    if(isNameSingular)
    {
        auto cn = zStringHelper::getclass_nameCamelCase(name);
        auto cnp = zPluralizer::Pluralize(name);
        this->initClass(cn,cnp);
    }
    else
    {
        QString sn = zPluralizer::Singularize(name);
        QString cn = zStringHelper::getclass_nameCamelCase(sn);
        QString cnp = zPluralizer::Pluralize(cn);
        this->initClass(cn, cnp);
    }
}

void zTable::initSql(const QString& _sql_conn, const QString& _sql_schema, const QString& _sql_table)
{
    this->sql_conn = _sql_conn;
    this->sql_schema = _sql_schema;
    this->sql_table = _sql_table;
}


QString zTable::toString() const
{
    QString rs = zStringHelper::Empty;
    QString ps;
    zforeach(r, this->rows){
        if(!rs.isEmpty())
        {
            rs += ',';
        }
        if(r->colName==this->pkname)
        {
            rs += QStringLiteral("PK:");
        }
        rs+=r->toString();
    }

//    zforeach(p, this->props){
//        if(!rs.isEmpty()) ps+=",";
//        //if(p->colName==this->pkname) ps+="PK:";
//        ps+=p->toString();
//    }

    return  this->name+'('+rs+')'+(!ps.isEmpty()?ps:zStringHelper::Empty);
}


/**
  összehasonlítja a két táblát, és annak sorait
  név, sorok száma, azok metaadatai -  nevei, típusai hosszai, nullable required a relevánsak
*/
bool zTable::Compare(const zTable& tv, QStringList& e){
    bool v = true;

    if(this->name!=tv.name)
    {
        e.append(QStringLiteral("Tablename Not Equals (%1, %2)").arg(this->name,tv.name));
        v = false;
    }

    if(this->pkname!=tv.pkname)
    {
        e.append(QStringLiteral("PkName Not Equals: %1(%2, %3)").arg(this->name, this->pkname!=tv.pkname));
        v = false;
    }

    // azt kell vizsgálni, hogy a tábla oszlopaihoz van -e megfelelő az sql-ben
//    if(this->rows.count()!=tv.rows.count())
//    {
//        e.append(QStringLiteral("rowCount Not Equals"));
//        v = false;
//    }
//    else
//    {
        zforeach(r,this->rows)
        {
            zTablerow* rv = zTablerow::getByName(tv.rows, r->colName);
            if(rv != nullptr)
            {
               auto v2 = r->Compare(*rv, e);
               if(!v2)
               {
                   //zlog.warning(e);
                   v = false;
               }
            }            
        }
//    }
    return v;
}

const zTable* zTable::find(const QList<zTable>& tables, const QString& rn, zTableSearchBy searchType){
    if(rn.isEmpty()) return nullptr;
    zforeach(r, tables){
        switch(searchType){
            case zTableSearchBy::class_name:
                //if(!r->tablename.isEmpty() && r->tablename == rn){
                if(rn == r->class_name)
                {
                    return r.operator->();
                }
                break;
            case zTableSearchBy::class_namePlural:
                if(rn == r->class_name_plural)
                {
                    return r.operator->();
                }
                break;
            case zTableSearchBy::Name:
                if(rn == r->name)
                {
                    return r.operator->();
                }
                break;
            case zTableSearchBy::TableName:
                if(rn == r->sql_table)
                {
                    return r.operator->();
                }
                break;
        }
    }
    return nullptr;
}


QString zTable::getPkByTableName(QList<zTable> *tables, const QString& rn){
    if(rn.isEmpty()) return nullptr;

    zforeach(r,*tables)
    {
        if(!r->sql_table.isEmpty())
        {
            if(r->sql_table == rn)
            {
                QString r2 = r.operator->()->pkname;
                return r2;
            }
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

createTableByText
createTableByText_2
createTableByText_3

globalSqlMaps
globalClassMaps
*/
//bool zTable::getClassType_old(const QString& ezt1,  QString *dtype, int *dlen, bool *nullable, bool isRequired)
//{
//    auto re_dlen1 = QRegularExpression(QStringLiteral(R"((?:\(([\d]+)\)))"), QRegularExpression::MultilineOption|QRegularExpression::UseUnicodePropertiesOption);
//    auto re_dlen2 = QRegularExpression(QStringLiteral(R"(([\d]+))"), QRegularExpression::MultilineOption|QRegularExpression::UseUnicodePropertiesOption);

//    auto re_isnullable = QRegularExpression(QStringLiteral(R"(Nullable\s*<\s*(\w+)\s*>|([\w\S]+)\?)"), QRegularExpression::MultilineOption|QRegularExpression::UseUnicodePropertiesOption);
//    //
//    auto m_isNullable = re_isnullable.match(ezt1);
//    QString typeName;

//    if(isRequired)
//    {
//        *nullable = false;
//        typeName = ezt1;
//    }
//    else{
//        if(m_isNullable.hasMatch())
//        {
//            *nullable = true;
//            typeName = getFirstNotNull(m_isNullable, 2);
//        }
//        else
//        {
//            *nullable = false;
//            typeName = ezt1;
//        }
//    }

//    bool isDtype = false;

//    if(zStringMapHelper::contains(&typeMap, typeName))
//    {
//        QString k = zStringMapHelper::getKey(&typeMap, typeName);
//        *dtype =  typeMap.value(k);;
//        isDtype = true;
//    }
//    else if(zStringMapHelper::contains(&typeMapR, typeName))
//    {
//        QString k = zStringMapHelper::getKey(&typeMapR, typeName);
//        *dtype =  typeMapR.value(k);;
//        isDtype = true;
//    }
//    else
//    {
//        auto i2 = re_dlen1.match(typeName);
//        if(i2.hasMatch())
//        {
//            bool isOK;
//            int n = i2.captured(1).toInt(&isOK);
//            if(isOK) *dlen = n;
//        }
//        else
//        {
//             i2 = re_dlen2.match(typeName);
//             if(i2.hasMatch())
//             {
//                 bool isOK;
//                 int n = i2.captured(1).toInt(&isOK);
//                 if(isOK) *dlen = n;
//            }
//        }
//    }

//    if(typeName.isEmpty()){
//        zlog.error(QStringLiteral("getType: Unknown type: %1").arg(ezt1));
//    }

//    return isDtype;
//}

bool zTable::getClassType(const QList<zConversionMap>& maps, const QString& ezt2,  QString *dtype, int *dlen, bool *nullable, bool isRequired, bool noWarnings)
{
    noWarnings = false;
//    zlog.trace(getClassType)
   // auto re_dlen1 = QRegularExpression(QStringLiteral(R"((?:\(([\d]+)\)))"), QRegularExpression::MultilineOption|QRegularExpression::UseUnicodePropertiesOption);
   // auto re_dlen2 = QRegularExpression(QStringLiteral(R"(([\d]+))"), QRegularExpression::MultilineOption|QRegularExpression::UseUnicodePropertiesOption);
    auto re_dlen3 = QRegularExpression(QStringLiteral(R"((\w)\(?(\d+)\)?$)"), QRegularExpression::MultilineOption|QRegularExpression::UseUnicodePropertiesOption);

    auto re_isnullable = QRegularExpression(QStringLiteral(R"(nullable\s*<\s*(\w+)\s*>|([\w\S]+)\?)"), QRegularExpression::MultilineOption|QRegularExpression::UseUnicodePropertiesOption);

    QString ezt1 = ezt2.toLower();

    bool isDtype = false;

    auto m_isNullable = re_isnullable.match(ezt1);// ki van írva betűvel, hogy nullable

    QString typeName;
    bool inullable;
    int idlen=*dlen;

    if(isRequired)// ha kötelező, akkor nem lehet null
    {
        inullable = false;
        typeName = ezt1;
    }
    else{ // ha nem kötelező
        if(m_isNullable.hasMatch()) // és ki van írva,
        {
            inullable = true;
            typeName = getFirstNotNull(m_isNullable, 2);
        }
        else
        {
            inullable = *nullable; // ha nincs rá explicit előírás, akkor az marad amit hoz
            typeName = ezt1;
        }
    }

     auto i2 = re_dlen3.match(typeName);
     if(i2.hasMatch())
     {
         bool isOK;
         int n = i2.captured(2).toInt(&isOK);
         if(isOK) idlen = n;
         typeName = typeName.left(i2.capturedStart(1)+1);
     }

    QStringList fl = zConversionMap::internals(maps, typeName);

    if(fl.isEmpty())
    {
        if(!noWarnings)
        {
            zlog.warning(QStringLiteral("Nem található belső adatábrázolási típus: %1").arg(ezt1));
        }
    }
    else
    {
        if(fl.count()>1)
        {
            if(!noWarnings)
            {
                zlog.warning(QStringLiteral("Több típus is javasolt: %1 -> %2").arg(ezt1, fl.join(',')));
            }
        }

        *dtype =  fl.first();
        *nullable = inullable;
        *dlen = idlen;
        isDtype = true;
    }

    if(typeName.isEmpty())
    {
        zlog.error(QStringLiteral("getClassType: Unknown type: %1").arg(ezt1));
    }

    return isDtype;
}

/*

*/
QStringList zTable::getFK(){
    QStringList fknames;
    zforeach(t, ztables){
        QString pn = t->class_name+t->pkname;
        if(containsRow(pn)){
           fknames<<pn;
        }
        }
    return fknames;
}

QStringList zTable::getFKclass_name(){
    QStringList fknames;
    zforeach(t, ztables)
    {
        QString pn = t->class_name+t->pkname;
        if(containsRow(pn))
        {
           fknames<<t->class_name;
        }
    }
    return fknames;
}

bool zTable::containsRow(const QString& n)
{
    zforeach(r, this->rows)
    {
        if(r->colName.toLower()==n.toLower())
        {
            return true;
        }
    }
    return false;
}


void zTable::toXML(QXmlStreamWriter *s)
{  

    s->writeStartElement(nameof(zTable));
    //s->writeAttribute(nameof(this->sourcetype), QString::number(this->sourcetype));
    s->writeAttribute(nameof(this->name), this->name);

    s->writeAttribute(nameof(this->sql_conn), this->sql_conn);
    s->writeAttribute(nameof(this->sql_schema), this->sql_schema);
    s->writeAttribute(nameof(this->sql_table), this->sql_table);    
    s->writeAttribute(nameof(this->sql_updateTimeStamp), this->sql_updateTimeStamp.toString());
    s->writeAttribute(nameof(this->sql_isValid), zStringHelper::boolToString(this->sql_isValid));

    s->writeAttribute(nameof(this->class_path), this->class_path);
    s->writeAttribute(nameof(this->class_name), this->class_name);
    s->writeAttribute(nameof(this->class_name_plural), this->class_name_plural);
    s->writeAttribute(nameof(this->source_updateTimeStamp), this->source_updateTimeStamp.toString());
    s->writeAttribute(nameof(this->source_isValid), zStringHelper::boolToString(this->source_isValid));

    s->writeAttribute(nameof(this->document_updateTimeStamp), this->document_updateTimeStamp.toString());
    s->writeAttribute(nameof(this->document_isValid), zStringHelper::boolToString(this->document_isValid));

    s->writeAttribute(nameof(this->comment), this->comment);
    s->writeAttribute(nameof(this->pkname), this->pkname);
    s->writeAttribute(nameof(this->name_formatstring), this->name_formatstring);
    s->writeAttribute(nameof(this->updateTime), this->updateTime.toString());

    s->writeStartElement(nameof(this->rows));
    zforeach(r, this->rows){ (*r).toXML(s); }
    s->writeEndElement();

    s->writeEndElement();
}


/**
Szükséges, hogy egy fájl egy táblát tartalmazzon, külömben követhetetlenné és kezelhetetlenné válik
*/
QList<zTable> zTable::createTableByXML(const QString& txt){
   QList<zTable> tl;
   QXmlStreamReader xml(txt);
//    while(!xml.atEnd()){
//        xml.readNext();
//        if(xml.isStartElement() && (xml.name() == nameof(zTable))){

    if (xml.readNextStartElement())
    {
        if(xml.name() == QStringLiteral("zTables"))
        {
            while(xml.readNextStartElement())
            {
                if(xml.name()==nameof(zTable))
                {
                    zTable t = fromXML(&xml);
                    //t.Validate(tl);
                    tl.append(t);

                }
                else
                {
                    xml.skipCurrentElement();
                }
            }
        }
        else if(xml.name() == QStringLiteral("zTable"))
        {
            zTable t = fromXML(&xml);
            //t.Validate(tl);
            tl.append(t);
        }
        else
        {
            xml.skipCurrentElement();
        }
    }
    if(xml.hasError())
    {
        zlog.error("createTableByXML: "+xml.errorString());
    }
    return tl;
}

zTable zTable::fromXML(QXmlStreamReader* xml){
    zTable t;

    auto a = xml->attributes();

    zXmlHelper::putXmlAttr(a, nameof(name), &(t.name));

    zXmlHelper::putXmlAttr(a, nameof(sql_conn), &(t.sql_conn));
    zXmlHelper::putXmlAttr(a, nameof(sql_schema), &(t.sql_schema));
    zXmlHelper::putXmlAttr(a, nameof(sql_table), &(t.sql_table));
    zXmlHelper::putXmlAttr(a, nameof(sql_isValid), &(t.sql_isValid));
    zXmlHelper::putXmlAttr(a, nameof(sql_updateTimeStamp), &(t.sql_updateTimeStamp));

    zXmlHelper::putXmlAttr(a, nameof(class_path), &(t.class_path));
    zXmlHelper::putXmlAttr(a, nameof(class_name), &(t.class_name));
    zXmlHelper::putXmlAttr(a, nameof(class_name_plural), &(t.class_name_plural));
    zXmlHelper::putXmlAttr(a, nameof(source_isValid), &(t.source_isValid));
    zXmlHelper::putXmlAttr(a, nameof(source_updateTimeStamp), &(t.source_updateTimeStamp));

    zXmlHelper::putXmlAttr(a, nameof(document_isValid), &(t.document_isValid));
    zXmlHelper::putXmlAttr(a, nameof(document_updateTimeStamp), &(t.document_updateTimeStamp));

    zXmlHelper::putXmlAttr(a, nameof(comment), &(t.comment));
    zXmlHelper::putXmlAttr(a, nameof(pkname), &(t.pkname));
    zXmlHelper::putXmlAttr(a, nameof(name_formatstring), &(t.name_formatstring));
    zXmlHelper::putXmlAttr(a, nameof(updateTime), &(t.updateTime));    

    /**/
    zXmlHelper::putXmlAttr(a, QStringLiteral("tablename"), &(t.sql_table));
    zXmlHelper::putXmlAttr(a, QStringLiteral("classname"), &(t.class_name));
    zXmlHelper::putXmlAttr(a, QStringLiteral("classname_plural"), &(t.class_name_plural));


    if (xml->readNextStartElement() && xml->name() == "rows")
    {
            t.rows = QList<zTablerow>();

            while(xml->readNextStartElement())
            {
                if(xml->name()==nameof(zTablerow))
                {
                    auto r = zTablerow::fromXML(xml);
                    t.rows.append(r);
                    //QString txt =
                            xml->readElementText();
                }
                else
                {
                    xml->skipCurrentElement();
                }

            }
            xml->readNextStartElement();
        }

    //t.props = QList<zTablerow>();
    //tl.append(t);
    //zlog.log("XML beolvasva: "+ t.name +xml->errorString());
    return t;
}

/*
Minden entitás minden mezőjének figyelembevételével
meg kell keresni névegyezés alapján
hogy az elsődleges kulcs milyen entitásokban szerepel mezőként
*/
QStringList zTable::getRFK(){
    QStringList rfknames;
    QString pn = class_name+pkname;

    zforeach(t, ztables){
        if(t->containsRow(pn)){
            rfknames<<t->class_name;
        }
    }
    return rfknames;
}

QStringList zTable::getRFKclass_namePlural(){
    QStringList rfknames;
    QString pn = class_name+pkname;

    zforeach(t, ztables){
        if(t->containsRow(pn)){
            rfknames<<t->class_name+';'+t->class_name_plural;
        }
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

QList<zTable> zTable::createTableByText(QString txt)
{
//    auto re = QRegularExpression(R"((?:^\s+)?(^(?:\s+)?\w*\s+)((?:^[\w\,\ \(\)\"\']*(?:\s+)?)+)(?:$|^\s+)?)", QRegularExpression::MultilineOption|QRegularExpression::UseUnicodePropertiesOption);
    auto re = QRegularExpression(QStringLiteral(R"(^\s*(?:(^\w*)\s+)((?:^[\w, ()\"'<>\.]+\n?)+))"), QRegularExpression::MultilineOption|QRegularExpression::UseUnicodePropertiesOption);
    //auto re_dlen1 = QRegularExpression(QStringLiteral(R"((?:\(([\d]+)\)))"), QRegularExpression::MultilineOption|QRegularExpression::UseUnicodePropertiesOption);
    auto re_dlen2 = QRegularExpression(QStringLiteral(R"(([\d]+))"), QRegularExpression::MultilineOption|QRegularExpression::UseUnicodePropertiesOption);
    auto re_dlen3 = QRegularExpression(QStringLiteral(R"((?:(\w+)\s*\(([\d]+)\)))"), QRegularExpression::MultilineOption|QRegularExpression::UseUnicodePropertiesOption);
    //(?:(\w+)\s*\(([\d]+)\))
    auto re_caption = QRegularExpression(QStringLiteral(R"((?:\"([\w]+)\"))"), QRegularExpression::MultilineOption|QRegularExpression::UseUnicodePropertiesOption);
    auto re_nullable = QRegularExpression(QStringLiteral(R"((?:((?:not\s*)?(?:nullable|null))))"), QRegularExpression::MultilineOption|QRegularExpression::UseUnicodePropertiesOption);

    auto re_macro_def = QRegularExpression(QStringLiteral(R"(^\s*(?:^#(\w*)\s+)((?:[\w, ()\"'<>\.]+\s)+))"), QRegularExpression::MultilineOption|QRegularExpression::UseUnicodePropertiesOption);
//    auto re_macro_use = QRegularExpression(R"((?:^([\w]+)\s*$))", QRegularExpression::MultilineOption|QRegularExpression::UseUnicodePropertiesOption);
    auto re_macro_use_tmp = QStringLiteral(R"((?:^(%1)\s*$))");

    auto j = re_macro_def.globalMatch(txt);

    while(j.hasNext()){
        QRegularExpressionMatch m = j.next();
        QString m_name=m.captured(1);
        QString m_txt=m.captured(2).trimmed();

        if(!macroMap.contains(m_name))
        {
            macroMap.insert(m_name, m_txt);
            zlog.trace(QStringLiteral("Macro def: %1").arg(m_name));
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
    QList<zTablerow> rl;
    if(i.hasNext()){        
        while(i.hasNext()){
            QRegularExpressionMatch m = i.next();
            QString tn=m.captured(1).trimmed();            
            QString pkn = QStringLiteral("id");
            //QString fn_line = m.captured(2);
            auto fn_lines = m.captured(2).split(QRegularExpression(QStringLiteral(R"([\n|\r\n|\r])")), QString::SkipEmptyParts);
            rl.clear();
            //QList<zTablerow> pl;
            int rl_counter = 0;
            zforeach(fn, fn_lines){
               rl_counter++;
               if(fn->isEmpty()) continue;
               if(*fn==QStringLiteral("_")) continue;

               QString dtype=zStringHelper::Empty;
               int dlen = 0;
               bool isNullable = true;
               QString caption = zStringHelper::Empty;
               QString ezt1;

               auto fns = fn->split(',', QString::SkipEmptyParts);
               QString fname = fns[0].trimmed();

               if(fns.length()>1)
               {
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
                           // csak egy típus felismerés - próbálkozásos alapon, így a warningokat elnyomjuk
                           isDtype = zTable::getClassType(globalClassMaps, ezt1, &dtype, &dlen, &isNullable, false, true);
                           }
                       // a típus tulajdonságainak meghatározása, pl.
                       // ha a sorban a szavak közt van "required" akkor isrequired true
                       // ha van not required, akkor viszont false
                       // ha van olyan ami számmá alakítható, az a hossza
                      if(!isDtype)
                       {
                            auto i2 = re_caption.match(*fn2);
                            if(i2.hasMatch())
                            {
                               caption = i2.captured(1);
                            }
                            else
                            {
                                auto i2 = re_dlen2.match(*fn2);
                                if(i2.hasMatch())
                                {
                                    bool isOK;
                                    int n = i2.captured(1).toInt(&isOK);
                                    if(isOK) dlen = n;
                                    }
                                else
                                {
                                    auto i2 = re_nullable.match(*fn2);
                                    if(i2.hasMatch()){
                                        //auto n_str = i2.captured(1);
                                        isNullable = !i2.captured(1).toLower().contains(QStringLiteral("not"));
//                                        if(n_str.toLower().contains(QStringLiteral("not")))
//                                            isNullable = false;
//                                        else
//                                            isNullable = true;
                                        }
                                    else{
                                        if(fn2->toLower()==QStringLiteral("key")){
                                            pkn = fname;
                                            }
                                        else if(fn2->toLower()==QStringLiteral("required")){
                                            isNullable = false;
                                            }
                                        }
                                    }
                                }                            
                            } /* típusmeghatározás vége*/
                        }
                    }
               if(dtype.isEmpty())
               {
                   // az egész sor releváns - nem tudjuk melyik szava milyen információt hordoz - illetve melyiknem mit kellene
                   zlog.error(QStringLiteral("%1 sor: nincs meghatározható típus: %2").arg(rl_counter).arg(*fn));
               }
               else
               {
                   if(caption.isEmpty())
                   {                                          
                       caption = getCaption(fname);
                   }
                   auto r = zTablerow(fname, dtype, dlen, isNullable, caption);
                   rl.append(r);
               }
            }

            auto t = zTable(tn, pkn, rl);//,  TXT, tn, zStringHelper::Empty);
            QString pluralClassName;
            auto className = zTable::getClassName(tn, pluralClassName);
            t.initClass(className, pluralClassName);

//
            bool isValid = t.Validate(tl);
            if(isValid)
            {
                tl.append(t);
                zlog.trace("GenerateByText: "+t.toString());
            }
            else
            {
                zlog.error(QStringLiteral("A tábla nem valid: %1").arg(t.name));
            }

        }
    }
    return tl;
}

QString zTable::getCaption(const QString& fname){
    QString caption;
    auto sp = zFileNameHelper::getSettingsDir();
    auto pp = zFileNameHelper::getProjectDir();
    
    // - caption = getCaption(fname)
    // ha a lokális tábla módosult a legutolsó betöltés óta - ezek időbélyegeit le kell tenni
    // keresás a lokális táblában
    // sikertelenség esetén
    // keresés a globális táblákban
    zConversionMap::reLoadAll(globalCaptionMaps, sp, zFileNameHelper::captionFileFilter);
    zConversionMap::reLoadAll(projectCaptionMaps, pp, zFileNameHelper::captionFileFilter);

    //globalCaptionMaps = zConversionMap::reLoadAll(sp, zFileNameHelper::captionFileFilter);
    caption = zConversionMap::external(projectCaptionMaps, fname);

    if(caption.isEmpty())
    {
        caption = zConversionMap::external(globalCaptionMaps, fname);
    }

    return caption;
}

/*
#Adm
DateCre DateMod datetime

class1
mezo1 mezo2 mezo3 string
mezo4 mezo5 int
int mezo6 mezo7
Adm

*/
QList<zTable> zTable::createTableByText_2(QString txt){
    auto re = QRegularExpression(QStringLiteral(R"(^\s*(?:(^\w*)\s+)((?:^[\w, ()\"'<>\.]+\n?)+))"), QRegularExpression::MultilineOption|QRegularExpression::UseUnicodePropertiesOption);

    auto re_macro_def = QRegularExpression(QStringLiteral(R"(^\s*(?:^#(\w*)\s+)((?:[\w, ()\"'<>\.]+\s)+))"), QRegularExpression::MultilineOption|QRegularExpression::UseUnicodePropertiesOption);
    auto re_macro_use_tmp = QStringLiteral(R"((?:^(%1)\s*$))");

    auto j = re_macro_def.globalMatch(txt);

    while(j.hasNext()){
        QRegularExpressionMatch m = j.next();
        QString m_name=m.captured(1);
        QString m_txt=m.captured(2).trimmed();

        if(!macroMap.contains(m_name)){
            macroMap.insert(m_name, m_txt);
            zlog.trace(QStringLiteral("Macro def: %1").arg(m_name));
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
    QList<zTablerow> rl;
    //QVector<zTablerow> tableRows;
    QStringList fieldNames;
    if(i.hasNext()){
        while(i.hasNext()){
            QRegularExpressionMatch m = i.next();
            QString tn=m.captured(1).trimmed();
            QString pkn = QStringLiteral("id");
            auto fns=m.captured(2).split(QRegularExpression(QStringLiteral(R"([\n|\r\n|\r])")), QString::SkipEmptyParts);

            rl.clear();//resize(0);
            //QList<zTablerow> pl;

            zforeach(fn, fns)
            { // sorok
               if(fn->isEmpty()) continue;
               if(*fn==QStringLiteral("_")) continue;

               QString dtype= zStringHelper::Empty;
               int dlen = 0;
               bool isNullable = true;

               auto fns = fn->split(' ', QString::SkipEmptyParts);

               if(fns.length()>2) //szavak (mezők) - legalább kettő/sor: egy típus és egy mező kell, hogy legyen
               {
                    bool isDtype = false;
                    fieldNames.clear();//.resize(0);//mező lista
                    // a vizsgált sor szavanként
                    zforeach(word, fns)
                    {
                        // a vizsgált szó vagy típus, vagy mező
                        // Elnyomjuk a warningokat - a szavankénti próbálkozás miatt - soronként csak egy lesz jó
                        isDtype = zTable::getClassType(globalClassMaps, *word, &dtype, &dlen, &isNullable, false, true);
                        if (isDtype)
                        {
                            zlog.trace(QStringLiteral("sortípus: %1").arg(*word));
                        }
                        else
                        {
                            fieldNames.append(*word);
                        }
                    }

                    if(!dtype.isEmpty())
                    {
                        zforeach(fn, fieldNames)
                        {
                            QString caption = zTable::getCaption(*fn);//zConversionMap::external(globalCaptionMaps, *fn);

                            zTablerow p(*fn, dtype, dlen, isNullable, caption);
                            rl.append(p);
                        }
                    }
                    else
                    {
                       zlog.trace(QStringLiteral("nem található sortípus: %1").arg(*fn));
                    }
                    // ha van mezőnév lista
                    // ha van ismert típus, akkor mezőlistához,
                    // ha nincs ismert a típus,
                    // a property listához adjuk hozzá
               }
            }
            auto t = zTable(tn, pkn, rl);//,  TXT, tn, zStringHelper::Empty);
            QString pluralClassName;
            auto className = zTable::getClassName(tn, pluralClassName);
            t.initClass(className, pluralClassName);

            tl.append(t);
            zlog.error("GenerateByText2: "+t.toString());
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

QList<zTable> zTable::createTableByText_3(const QString& txt, QMap<QString, QString>* constMap, QMap<QString, QString>* valueMap)
{
    QList<zTable> tl;
    zlog.error("GenerateByText3: ");
    // attributumok és az osztály
    // 1:név, 2:definíció
    auto re = QRegularExpression(QStringLiteral(R"((?:\[[(.\w)]*\])|class\s+(\w+)\s+(\{(?>[^{}]+|(?2))*\}))"), QRegularExpression::MultilineOption|QRegularExpression::UseUnicodePropertiesOption);

    // attributumok és propertyk
    // 1: típus, 2: név
    auto r_attrOrProp = QRegularExpression(QStringLiteral(R"(\[[(.\w)]*\]|public\s+(\S+)\s+(\w+)(?:\s*{.*}))"), QRegularExpression::MultilineOption|QRegularExpression::UseUnicodePropertiesOption);

   // auto r_tablanev = QRegularExpression(R"(\"([\p{L}]+)\"|(?:[\p{L}0-9.]+)\.([^.][\w]+))");
   //\[[(.\w)]*\]|public\s+(\p{L}[\p{L}0-9_\-]+)\s+(\w+)(?:\s*{.*})

    //auto r_tablanev2 = QRegularExpression(QStringLiteral(R"(\"([\p{L}]+)\"|(?:[\p{L}0-9.]+))"));

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
                QString class_name = m.captured(1);
                QString class_txt = m.captured(2);
                QString pkName = zStringHelper::Empty;
                zlog.error("class: " + class_name);

                if(!classAttrs.isEmpty()){
                    zforeach(a, classAttrs){
                        //zlog.log("classAttr: "+(*a));
                        auto attrParams = getAttrAndParams((*a));
                        QString attrname = attrParams[0];
                        if(attrname==QStringLiteral("Table")){
                            // ha szöveg konstans, az, ha nem, akkor osztály.
                            //tableName = getConstFromArgument(attrParams[1]);
                            tableName = attrParams[1];
                            if(zStringHelper::isclass_name(tableName)){
                                QString key = class_name+','+attrname;
                                if(constMap->contains(key)){
                                    QString key2 = constMap->value(key);
                                    tableName = valueMap->value(key2);
                                }else{
                                    constMap->insert(key, tableName);
                                    tableName=zStringHelper::Empty;
                                    }
                                }
                            if(!tableName.isEmpty())
                            {
                                tableName = getConstFromArgument(tableName);
                            }

                        }
                        else
                        {
                            zlog.error(QStringLiteral("Unknown TableAttr: %1").arg(*a));
                            }
                        }
                    classAttrs.clear();//resize(0);
                    }

                QList<zTablerow> rl;

                auto i_attrOrProp = r_attrOrProp.globalMatch(class_txt);
                //if(i2.hasNext()){
                    while(i_attrOrProp.hasNext()){
                        QRegularExpressionMatch m_attrOrProp = i_attrOrProp.next();

                        QString attrOrProp=m_attrOrProp.captured(0);
                        if(attrOrProp[0]=='[')
                        {
                            propAttrs.append(attrOrProp);
                            //zlog.log("attr: "+attrOrProp);
                        }
                        else if(attrOrProp[0]=='p'){
                            //bool isPk = false;
                            bool isRequired = false;
                           // QString MaxLength;
                            QString Caption  = zStringHelper::Empty;
                            QString dtype=zStringHelper::Empty;
                            int dlen = 0;
                            bool isNullable= false;

                            QString propType = m_attrOrProp.captured(1);
                            QString propName = m_attrOrProp.captured(2);

                            if(!propAttrs.isEmpty())
                            {
                                zforeach(a, propAttrs)
                                {
                                    //zlog.log("propAttr: "+(*a));
                                    auto attrParams = getAttrAndParams((*a));
                                    QString attrname = attrParams[0];
                                    if(attrname==QStringLiteral("Key")){
                                        //isPk = true;
                                        pkName = propName;
                                    }
                                    else if(attrname==QStringLiteral("Required")) {
                                        isRequired = true;
                                    }
                                    else if(attrname==QStringLiteral("MaxLength")){
                                        QString MaxLength = attrParams[1];
                                        if(zStringHelper::isclass_name(MaxLength)){
                                            QString key = propName+','+attrname;
                                            if(constMap->contains(key)){
                                                QString key2 = constMap->value(key);
                                                MaxLength = valueMap->value(key2);
                                            }else{
                                                constMap->insert(key, MaxLength);
                                                MaxLength=zStringHelper::Empty;
                                                }
                                            }
                                        if(!MaxLength.isEmpty())
                                        {
                                            dlen = MaxLength.toInt();
                                        }

                                        }
                                    else{
                                        zlog.error(QStringLiteral("Unknown PropertyAttr: %1").arg(*a));
                                        }
                                    }
                                propAttrs.clear();//.resize(0);
                                }


                            //QString row = m_attrOrProp.captured(0);
                            //bool isDtype =

                                 zTable::getClassType(globalClassMaps, propType, &dtype, &dlen, &isNullable, isRequired);

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
                            if(Caption.isEmpty()){
                                Caption = zTable::getCaption(propName);//zConversionMap::external(globalCaptionMaps, propName);
                            }
                            auto r = zTablerow(propName, dtype, dlen, isNullable, Caption);
                            rl.append(r);
                        }
                        else{
                            zlog.error("undefined: "+attrOrProp);
                        }

                    }
                //}                   
                   //auto t = zTable(class_name, QStringLiteral("id"), rl,  TXT, tableName, zStringHelper::Empty);
                   auto t = zTable(tableName, QStringLiteral("id"), rl);//,  TXT, tableName, zStringHelper::Empty);
                   QString pluralClassName;
                   auto className = zTable::getClassName(class_name, pluralClassName);
                   t.initClass(className, pluralClassName);

                   //t.initClass(class_name);
                   tl.append(t);
                   zlog.error("GenerateByEntity: "+t.toString());
            }
            else{
                zlog.error("undefined: "+g);
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

QStringList zTable::getAttrAndParams(const QString& str){
    //attributum és az argumentumlista
    auto r= QRegularExpression(QStringLiteral(R"(\[\s*(\w+)(\((?>[^)(]+|(?2))*\))?\s*\])"));
    auto r2= QRegularExpression(QStringLiteral(R"((\"?\b[\p{L}0-9. ]+\b\"?(\([\w(), "]*\))*))"));

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

QString zTable::getFirstNotNull(const QRegularExpressionMatch& m, int max){
    for(int j=1;j<=max;j++){
        if(!m.captured(j).isEmpty()){
            return m.captured(j);
        }
    }
    return zStringHelper::Empty;
}

/*
  (\w+)\s*=\s*(\"?[^\"][\p{L} \S]*\"?)\;
 */

QString zTable::getConstFromArgument(const QString& str){
    auto r_class = zSourceHelper::getRegex_r_class();
    QString eredmeny;

    if(str.startsWith('\"')||str.endsWith('\"'))
    {
        eredmeny =  str.mid(1, str.length()-2);
    }
    else
    {
        QString class_name = str.section('.', 0);

        QStringList constlist;
        /**/


        auto i_class = r_class.globalMatch(str);
        while(i_class.hasNext())
        {
           QRegularExpressionMatch m_class = i_class.next();
        }

    }
    eredmeny =  str;
    return eredmeny;
}


QList<zTable> zTable::createTableByClassTxt(const QString& txt){

    QMap<QString, QString> constNameMap;
    
    auto tl = zTable::createTableByText_3(txt, &constNameMap);
    
    if(tl.length()==0)
    {
        zlog.error(QStringLiteral("nem jött létre adat"));
        return tl;
    }

    if(!beallitasok.currentProjectName.isEmpty()){
        return tl;
    }

    // konstanstábla beolvasása
    auto path = zFileNameHelper::getCurrentProjectDir();
            //append(QDir::homePath(),beallitasok.projectdir,beallitasok.currentProjectName);

    // key az attrName, value a constName
    QStringList class_nameFilter;

    QStringList constNameList;// = constNameMap.values();


    zforeach(m, constNameMap){
        auto class_name = m.value().split('.').first();
        auto clf = class_name+".c?";
        if(!class_nameFilter.contains(clf)) {
            class_nameFilter.append(clf);
            }

        if(!constNameList.contains(m.value())){
            constNameList.append(m.value());
        }
    }

    QStringList files = zFileNameHelper::FindFileNameInDir(path, QStringLiteral("Data"), class_nameFilter);

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
    if(!constNameMap.isEmpty()){
        tl = zTable::createTableByText_3(txt, &constNameMap, &constValueMap);
    }

    return tl;
}

/*
lementi a  táblát - nem caption, hanem teljes xml, így ez később átnevezendő
*/
void zTable::saveTablaToXML() {
    if(beallitasok.currentProjectName.isEmpty())
    {
        return;
    }
    if(this->rows.isEmpty())
    {
        zlog.error(QStringLiteral("Nem tartalmaz sorokat: ").arg(this->name));
        return;
    }
    QString fn = zFileNameHelper::getCurrentProjectFileName(this->name + ".xml");
            //append(QDir::homePath(),beallitasok.projectdir,beallitasok.currentProjectName, this->name + ".xml");

    QString e;
    QXmlStreamWriter s(&e);
    s.setAutoFormatting(true);
    s.writeStartDocument();

    this->toXML(&s);
    s.writeEndDocument();

    zTextFileHelper::save(e, fn);
}


/**
  Az adott táblát validálja -
  név nem lehet üres - kötelező,
  a névnek egyedinek kell lennie
   azok a belső osztályok vannak, amik ezekben kulcsként szerepelnek: globalSqlMaps, globalClassMaps
*/

bool zTable::Validate(const QList<zTable>& tables){
    bool v= true;

    if(name.isEmpty()){
        zlog.error(QStringLiteral("Nincs név"));
        v= false;
    }
    if(zTable::find(tables, name, zTableSearchBy::Name))
    {
        zlog.error(QStringLiteral("Név nem egyedi: %1").arg(name));
        v= false;
    }
    if(rows.isEmpty())
    {
        zlog.error(QStringLiteral("Nincsenek sorok"));
        v= false;
    }    
    else{
        QStringList colNames;
        colNames = zTablerow::colNames(rows);

        zforeach(r,rows)
        {
            bool is_rv = r->Validate2(colNames);
            if(!is_rv)
            {
                v= false;
            }
        }
    }
    return v;
}

QDateTime zTable::getSqlUpdateTimestamp()
{
    zSQL zsql;
    auto dbconn = beallitasok.findDbConnection(this->sql_conn);
    //QString driver, QString adatbazisNev, QString server, QString user, QString password
    if(dbconn == nullptr)
    {
        zlog.warning(QStringLiteral("dbconn nem található: %1").arg(this->sql_conn));
        return QDateTime();
    }
    if(!dbconn->isValid())
    {
        zlog.warning(QStringLiteral("dbconn nem valid: %1").arg(this->sql_conn));
        return QDateTime();
    }

    if(zsql.init(*dbconn))
    {
        auto e = zsql.getTableUTIME(this->sql_schema, this->sql_table);
        zlog.trace(QStringLiteral("updated: %1").arg(e.toString()));
    }

    return QDateTime();
}

//TODO fájl utolsó szerkesztés idejének megszerzése
QDateTime zTable::getSourceUpdateTimestamp()
{    
    return QDateTime::currentDateTime();
}

// TODO fájl utolsó szerkesztés idejének megszerzése
QDateTime zTable::getDocUpdateTimestamp()
{
    return QDateTime::currentDateTime();
}

/*
Validálja a táblát annak sql kötése alapján
*/
bool zTable::validateSQL(){
    zSQL zsql;
    auto dbconn = beallitasok.findDbConnection(this->sql_conn);


    if(zsql.init(*dbconn))
    {
        zTable t_sql = zsql.getTable(this->sql_schema, this->sql_table);
        QStringList e;
        auto isOK = Compare(t_sql, e);
        if(!isOK)
        {
            zlog.warning(e);
        }
        return isOK;
    }

    return false;
}

// TODO validálás a forrás file  alapján
// be kell olvasni a forrást, ztablevé kell alakítani, majd compare
// void retek2::on_pushButton_srcimport_clicked() alapján
bool zTable::validateSource(){

    if(!this->class_path.isEmpty())
    {

    }
    //QString f_txt = zTextFileHelper::load(srcName);
    //auto tl = zTable::createTableByClassTxt(f_txt);
    bool v = true;
    return v;
}

// TODO validálás a dokumentáció  alapján
bool zTable::validateDocument(){
    bool v = true;
    return v;
}
