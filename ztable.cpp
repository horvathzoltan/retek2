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
#include "ztableerror.h"

#include <QDir>
#include <QRegularExpression>
#include <QXmlStreamWriter>

const QString zTable::PKNAME = QStringLiteral("pkname");

zTable::zTable()= default;

zTable::~zTable()= default;

zTable::zTable(const QString& _name, const QString& pkn, const QList<zTablerow>& tr){
    this->name = _name;
    //this->pkname = pkn;
    this->rows = tr;

    this->pkrowix = zTablerow::findIx(tr, pkn);

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
        if(r->colName==this->pkname())
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
/*
tname..name:noteq : táblanév..property:hibakód leírás // ha a sornév empty, akkor az prop
tname..pkname:noteq
tname..row:unknown

tname.rname.cname:noteq : táblanév.sornév.oszlopnév:hibakód leírás
tname.rname.ctype:noteq
tname.rname.clen:noteq
tname.rname.cisnull:noteq
*/

enum class zTable::ErrCode:int{noteq, unknown};

const QMap<zTable::ErrCode, QString> zTable::ErrCodeDescriptions
{
    {ErrCode::noteq, QStringLiteral("Not Equals")},
    {ErrCode::unknown, QStringLiteral("Unknown")},
};

const QMap<zTable::ErrCode, QString> zTable::ErrCodeNames
{
    {ErrCode::noteq, QStringLiteral("noteq")},
    {ErrCode::unknown, QStringLiteral("unknown")},
};

//QString zTable::GetErrorMessage(const QString& cn, ErrCode code)
//{
//    auto c = zTable::ErrCodeNames[code];
//    auto l = QStringLiteral("[%1..%3:%4]").arg(this->name, cn, c);
//    return l;
//}

zTableError zTable::GetFullError(const QString& cn, ErrCode code, const QStringList&  p, const QString& source)
{   
    auto err = zTableError(this->name, zStringHelper::Empty, cn, ErrCodeNames[code],source, ErrCodeDescriptions[code],  p);
    return err;
}

QString zTable::SqlConnToString() const
{
    QString sqlconn;
    if(!this->sql_conn.isEmpty())
    {
        sqlconn = this->sql_conn;
        if(!this->sql_conn.isEmpty())
        {
            sqlconn += QStringLiteral("; Db=")+this->sql_schema;
        }
    }
    return sqlconn;
}



bool zTable::Compare(const zTable& tv, QList<zTableError>& e, const QString& source){
    zInfo(zfn());

//    if(this->name.toLower()=="fue"){
//        int x = 0;
//    }

    bool v = true;

    if(!tv.name.isEmpty() && this->name != tv.name)
    {
        auto err = GetFullError(nameof(this->name), ErrCode::noteq, {"name",this->name, tv.name}, source);
        e.append(err);
        v = false;
    }

    auto pkname = this->pkname();
    if(this->pkname()!=tv.pkname())
    {

        auto err = GetFullError(nameof(this->pkname()), ErrCode::noteq, {"pkname",this->pkname(), tv.pkname()}, source);
        e.append(err);
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
    //vesszük tv összes mezőjének nevét
    // ami a thisben talál, azt kivesszük
    QSet<QString> tvrows;
    zforeach(r,tv.rows)
    {
        tvrows.insert(r->colName);
    }

    zforeach(r,this->rows)
    {
        zTablerow* rv = zTablerow::getByName(tv.rows, r->colName);
//            if(tv.name.toLower()=="fue"){
//                int x =0;
//            }

        if(rv != nullptr)
        {
           tvrows.remove(r->colName);
           auto v2 = r->Compare(*rv, e, source);
           if(!v2)
           {
               v = false;
           }
        }
        else // ez itt a célban, azaz a tvben ismeretlen, azaz a doksi, src, sql
        {
            auto err = GetFullError(nameof(this->name), ErrCode::unknown, {"SourceField", r->colName}, source);
            e.append(err);
            v=false;
        }
    }
    if(!tvrows.isEmpty()){ // a forrásban ismeretlenek
        zforeach(r,tvrows){
            auto err = GetFullError(nameof(this->name), ErrCode::unknown, {"DestField", *r}, source);
            e.append(err);
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
                QString r2 = r.operator->()->pkname();
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

// TODO kellene c syntax, sql syntax, txt syntax
// c: int? illetve nullable<int>, int[]
// https://stackoverflow.com/questions/11914473/c-sharp-regex-select-class-properties-names-methods-name-and-fields-from-clas
// methods: (?:public\s|private\s|protected\s|internal\s)?[\s\w]*\s+(?<methodName>\w+)\s*\(\s*(?:(ref\s|/in\s|out\s)?\s*(?<parameterType>\w+)\s+(?<parameter>\w+)\s*,?\s*)+\)
// fields: (?:public\s|private\s|protected\s)\s*(?:readonly\s+)?(?<type>\w+)\s+(?<name>\w+)
// sql int(55)
// text
// Name,String(44),30,nullable
// Name,String(44),30,not nullable

bool zTable::getClassType(const QList<zConversionMap>& maps, const QString& ezt2,  QString *dtype, int *dlen, bool *nullable, bool isRequired, bool noWarnings)
{
    noWarnings = false;
//    zTrace(getClassType)
   // auto re_dlen1 = QRegularExpression(QStringLiteral(R"((?:\(([\d]+)\)))"), QRegularExpression::MultilineOption|QRegularExpression::UseUnicodePropertiesOption);
   // auto re_dlen2 = QRegularExpression(QStringLiteral(R"(([\d]+))"), QRegularExpression::MultilineOption|QRegularExpression::UseUnicodePropertiesOption);
    auto re_dlen3 = QRegularExpression(QStringLiteral(R"((\w)\s*\(?(\d+)\)?)"), QRegularExpression::MultilineOption|QRegularExpression::UseUnicodePropertiesOption);

    auto re_isnullable = QRegularExpression(QStringLiteral(R"(nullable\s*<\s*(\w+)\s*>|([\w\S]+)\?)"), QRegularExpression::MultilineOption|QRegularExpression::UseUnicodePropertiesOption);

    QString ezt1 = ezt2.trimmed().toLower();

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
            // createTableByHtml -> a típus neve tartalmazhat adathosszt is, bár nem kellene, mert ide már szétszedve kellene kerülnie
            zInfo(QStringLiteral("Nem található belső adatábrázolási típus: %1 warning").arg(ezt1));
            //zDebug();
        }
    }
    else
    {
        if(fl.count()>1)
        {
            if(!noWarnings)
            {
                zInfo(QStringLiteral("Több típus is javasolt: %1 -> %2 warning").arg(ezt1, fl.join(',')));
                /*if(fl.contains(ezt1))
                {
                     //int ix = fl.indexOf(ezt1);
                     fl.insert(0, ezt1);
                }*/
            }
        }

        *dtype =  fl.first();
        *nullable = inullable;
        *dlen = idlen;
        isDtype = true;
    }

    if(typeName.isEmpty())
    {
        zError(QStringLiteral("getClassType: Unknown type: %1").arg(ezt1));
    }

    return isDtype;
}

/*

*/
QStringList zTable::getFK(){
    QStringList fknames;
    zforeach(t, ztables){
        QString pn = t->class_name+t->pkname();
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
        QString pn = t->class_name+t->pkname();
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

    s->writeAttribute(nameof(this->docName), this->docName);
    s->writeAttribute(nameof(this->document_path), this->document_path);
    s->writeAttribute(nameof(this->document_updateTimeStamp), this->document_updateTimeStamp.toString());
    s->writeAttribute(nameof(this->document_isValid), zStringHelper::boolToString(this->document_isValid));

    s->writeAttribute(nameof(this->comment), this->comment);

    s->writeAttribute(PKNAME, this->pkname());
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
        zError("createTableByXML: "+xml.errorString());
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

    zXmlHelper::putXmlAttr(a, nameof(docName), &(t.docName));
    zXmlHelper::putXmlAttr(a, nameof(document_path), &(t.document_path));
    zXmlHelper::putXmlAttr(a, nameof(document_isValid), &(t.document_isValid));
    zXmlHelper::putXmlAttr(a, nameof(document_updateTimeStamp), &(t.document_updateTimeStamp));

    zXmlHelper::putXmlAttr(a, nameof(comment), &(t.comment));
    QString pkname;
    zXmlHelper::putXmlAttr(a, PKNAME, &(pkname));
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

    t.pkrowix = zTablerow::findIx(t.rows, pkname);

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
    QString pn = class_name+this->pkname();

    zforeach(t, ztables){
        if(t->containsRow(pn)){
            rfknames<<t->class_name;
        }
    }
    return rfknames;
}

QStringList zTable::getRFKclass_namePlural(){
    QStringList rfknames;
    QString pn = class_name+pkname();

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
            zInfo(QStringLiteral("Macro def: %1").arg(m_name));
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
                   zforeach_from(fn2, fns, 1)
                   {
                       auto fn3s= fn2->split(' ', QString::SkipEmptyParts);
                       bool isDtype = false;
                       zforeach(fn3, fn3s)
                       {
                           // ez megfelel az sql típusúnak
                           //tipusnev(128)
                           auto i3 = re_dlen3.match(*fn3);
                           if(i3.hasMatch())
                           {
                               ezt1 = i3.captured(1);
                               bool isOK;
                               int n = i3.captured(2).toInt(&isOK);
                               if(isOK) dlen = n;
                           }
                           else
                           {
                               ezt1 = *fn3;
                           }                           
                           //típus vizsgálat
                           // csak egy típus felismerés - próbálkozásos alapon, így a warningokat elnyomjuk
                           isDtype = zTable::getClassType(globalClassMaps, ezt1, &dtype, &dlen, &isNullable, false, true);
                       }
                       // a típus tulajdonságainak meghatározása, pl.
                       // tovább folytatjuk a szavak egyenkénti vizsgálatát, és ha nem típust írnak le, akkor a megtaláltat módosítják:
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
                                    if(i2.hasMatch())
                                    {
                                        //auto n_str = i2.captured(1);
                                        isNullable = !i2.captured(1).toLower().contains(QStringLiteral("not"));
//                                        if(n_str.toLower().contains(QStringLiteral("not")))
//                                            isNullable = false;
//                                        else
//                                            isNullable = true;
                                    }
                                    else
                                    {
                                        if(fn2->toLower()==QStringLiteral("key"))
                                        {
                                            pkn = fname;
                                        }
                                        else if(fn2->toLower()==QStringLiteral("required"))
                                        {
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
                   zError(QStringLiteral("%1 sor: nincs meghatározható típus: %2").arg(rl_counter).arg(*fn));
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
            bool isValid = t.Validate(tl, t.eval, zfn());
            if(isValid)
            {
                tl.append(t);
                zInfo("GenerateByText: "+t.toString());
            }
            else
            {
                zInfo(QStringLiteral("A tábla nem valid: %1").arg(t.name));
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

    while(j.hasNext())
    {
        QRegularExpressionMatch m = j.next();
        QString m_name=m.captured(1);
        QString m_txt=m.captured(2).trimmed();

        if(!macroMap.contains(m_name))
        {
            macroMap.insert(m_name, m_txt);
            zInfo(QStringLiteral("Macro def: %1").arg(m_name));
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
                            zInfo(QStringLiteral("sortípus: %1").arg(*word));
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
                       zInfo(QStringLiteral("nem található sortípus: %1").arg(*fn));
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
            zError("GenerateByText2: "+t.toString());
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
    zTrace();
    QList<zTable> tl;

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
                zInfo("class: " + class_name);

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
                            zInfo(QStringLiteral("Unknown TableAttr: %1").arg(*a));
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
                                        zInfo(QStringLiteral("Unknown PropertyAttr: %1").arg(*a));
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
//                                zTrace("sortípus:"+dtype);
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
                            zInfo("undefined: "+attrOrProp);
                        }

                    }
                //}                   
                   //auto t = zTable(class_name, QStringLiteral("id"), rl,  TXT, tableName, zStringHelper::Empty);
                   auto t = zTable(tableName, pkName, rl);//,  TXT, tableName, zStringHelper::Empty);
                   QString pluralClassName;
                   auto className = zTable::getClassName(class_name, pluralClassName);
                   t.initClass(className, pluralClassName);

                   //t.initClass(class_name);
                   tl.append(t);
                   zInfo("GenerateByEntity: "+t.toString());
            }
            else{
                zInfo("undefined: "+g);
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
        zError(QStringLiteral("nem jött létre adat"));
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
        zError(QStringLiteral("Nem tartalmaz sorokat: ").arg(this->name));
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

bool zTable::Validate(const QList<zTable>& tables, QList<zTableError>& e, const QString& source){
    bool v= true;

    if(this->pkrowix==-1)
    {
        zInfo(QStringLiteral("Nincs PK error"));
        v=false;
    }

    if(name.isEmpty()){
        zInfo(QStringLiteral("Nincs név error"));
        v= false;
    }
    if(zTable::find(tables, name, zTableSearchBy::Name))
    {
        zInfo(QStringLiteral("Név nem egyedi: %1 error").arg(name));
        v= false;
    }
    if(rows.isEmpty())
    {
        zInfo(QStringLiteral("Nincsenek sorok error"));
        v= false;
    }        
    else
    {
        QStringList colNames;
        colNames = zTablerow::colNames(rows);

        for(int i=0;i<rows.length();i++)
        //zforeach(r,rows)
        {
            auto r = &(rows[i]);
            if(i==pkrowix && r->isNullable)
            {
                zInfo(QStringLiteral("PK nem lehet nullable error"));
                v=false;
            }
            bool is_rv = r->Validate2(colNames, e, source);
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
        zWarning(QStringLiteral("dbconn nem található: %1").arg(this->sql_conn));
        return QDateTime();
    }
    if(!dbconn->isValid())
    {
        zWarning(QStringLiteral("dbconn nem valid: %1").arg(this->sql_conn));
        return QDateTime();
    }

    if(zsql.init(*dbconn))
    {
        auto e = zsql.getTableUTIME(this->sql_schema, this->sql_table);
        zInfo(QStringLiteral("updated: %1").arg(e.toString()));
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

QString zTable::pkname() const
{
    if(hasPkname())
    {
        return this->rows[this->pkrowix].colName;
    }

    return zStringHelper::Empty;
}

bool zTable::hasPkname() const
{
    return this->pkrowix>-1;
}

/*
Validálja a táblát annak sql kötése alapján
*/
//void zTable::validateEval(bool isOK, const QStringList& e, const QString &ez)
//{
//    if(!isOK)
//    {
//        zInfo(ez+" "+this->name+QStringLiteral(" error"));
//        zInfo(e);
//    }
//    else
//    {
//        zInfo(ez+" "+this->name+QStringLiteral(" ok"));
//    }
//}

bool zTable::validateSQL(){
    zTrace();
    zSQL zsql;
    auto dbconn = beallitasok.findDbConnection(this->sql_conn);

    if(!zsql.init(*dbconn))
    {
        return false;
    }

    zTable t_sql = zsql.getTable(this->sql_schema, this->sql_table);

    //QStringList e;
    auto isOK = Compare(t_sql, this->eval, nameof(zTable::validateSQL()));
    //validateEval(isOK, e, QStringLiteral("sql"));
    return isOK;
}

bool zTable::validateSource(){
    //zTrace();
    if(!this->class_path.isEmpty())
    {
        QString fn = zFileNameHelper::getCurrentProjectFileNameAbsolut(this->class_path);

        QString f_txt = zTextFileHelper::load(fn);
        if(f_txt==zStringHelper::Empty) return false;

        auto tl = zTable::createTableByClassTxt(f_txt);

        zforeach(t,tl)
        {
            t->name = this->name;
            if(t->class_name == this->class_name)
            {
                //QStringList e;
                auto isOK = Compare(*t, eval, nameof(zTable::validateSource()));
                //validateEval(isOK, e, QStringLiteral("src"));
                return isOK;
            }
        }
    }
    return false;
}

bool zTable::validateDocument(){    
    zTrace();
    if(!this->document_path.isEmpty())
    {
//        if(zFileNameHelper::isURL(this->document_path))
//        {
//            //zInfo("url");
//            auto e = downloader.download(QStringLiteral(R"(https://docs.google.com/document/export?format=html&id=1tPwsVMObxU9QmA3XR4RpbHPpjcG7hVbd7KQqLD_ABK8&includes_info_params=true)"));
//            f_txt = zStringHelper::HtmlDecode(e);
//            //zTextFileHelper::save(f_txt, "/home/zoli/aa.html");
//        }
//        else
//        {
//            QString fn = zFileNameHelper::getCurrentProjectFileNameAbsolut(this->document_path);
//            f_txt = zTextFileHelper::load(fn);
//        }

        QString f_txt = zTextFileHelper::load(this->document_path);
        if(f_txt.isEmpty()) return false;

        auto tl = zTable::createTableByHtml(f_txt);

        zforeach(t,tl)
        {
//            if(t->name.toLower()=="units"){
//                zTrace();
//            }
            t->name=zStringHelper::Empty;

            if(!t->docName.isEmpty() && t->docName == this->docName)
            {
                auto isOK = Compare(*t, eval, nameof(zTable::validateDocument()));
                return isOK;
            }
        }

    }    
    return false;
}

//auto rt = zTable::QStringLiteral(R"(<table.*?>([\s\S]*)<\/table>)");
QString zTable::createTxtByHtml(const QString& txt){
    QRegularExpression rtable = zStringHelper::getHTMLRegExp(zStringHelper::HTML_TABLE);
    QRegularExpression rtbody = zStringHelper::getHTMLRegExp(zStringHelper::HTML_TBODY);
    QRegularExpression rtr = zStringHelper::getHTMLRegExp(zStringHelper::HTML_TR);
    QRegularExpression rtd = zStringHelper::getHTMLRegExp(zStringHelper::HTML_TD);
    QRegularExpression rp = zStringHelper::getHTMLRegExp(zStringHelper::HTML_P);
    QRegularExpression rspan = zStringHelper::getHTMLRegExp(zStringHelper::HTML_SPAN);

    QStringList name_list{"nev","name","mezo","field","column","oszlop"};
    QStringList type_list{"tipus","type"};
    QStringList comment_list{"comment","megjegyzes","komment","leiras"};

    QString e;
    auto rti = rtable.globalMatch(txt);
    int table_ix=0;
    while(rti.hasNext())
    {
        auto rtm = rti.next();
        QString table_txt=rtm.captured(1);
        QString table_txt2=rtm.captured(0);
        if(!table_txt.isEmpty())
        {
            bool nameOk = false;
            bool typeOk = false;
            //tbody
            auto rtbodym = rtbody.match(table_txt);
            if(rtbodym.hasMatch())
            {
                table_txt = rtbodym.captured(1);//1
            }
            //tr

            auto rtri = rtr.globalMatch(table_txt);
            int row_ix=0;
            while(rtri.hasNext())
            {
                auto rtrm = rtri.next();
                if(row_ix==1)
                {
                    QString tr_txt=rtrm.captured(1);
                    //td
                    auto tdi = rtd.globalMatch(tr_txt);
                    int col_ix = 0;
                    while(tdi.hasNext())
                    {
                        auto tdm = tdi.next();
                        QString td_txt = tdm.captured(1);

                        if(!td_txt.isEmpty())
                        {
                            auto rspani = rspan.globalMatch(td_txt);

                            while(rspani.hasNext())
                            {
                                auto rspanm = rspani.next();

                                auto txt = rspanm.captured(1);
                                QString a = zStringHelper::zNormalize(txt);
                                if(name_list.contains(a)) nameOk = true;
                                if(type_list.contains(a)) typeOk = true;

                            }
                        }
                    }
                }
                row_ix++;
            }
            if(nameOk && typeOk)
            {
                if(!e.isEmpty()) e+=(QStringLiteral("<br/><br/><br/>"));
                e+=table_txt2;
            }
        }
        table_ix++;
    }
    auto str = QStringLiteral(
"<!DOCTYPE html>"
"<html>"
"<head>"
"<title>Page Title</title>"
"</head>"
"<body>"
"%1"
"</body>"
"</html>"
        );
    return str.arg(e);
}

QList<zTable> zTable::createTableByHtml(const QString& txt, const QString &d){
    QList<zTable> e;

    QString sName = (!d.isEmpty())?zStringHelper::zNormalize(d):zStringHelper::Empty;
    QRegularExpression rtable = zStringHelper::getHTMLRegExp(zStringHelper::HTML_TABLE);
    QRegularExpression rtbody = zStringHelper::getHTMLRegExp(zStringHelper::HTML_TBODY);
    QRegularExpression rtr = zStringHelper::getHTMLRegExp(zStringHelper::HTML_TR);
    QRegularExpression rtd = zStringHelper::getHTMLRegExp(zStringHelper::HTML_TD);
    QRegularExpression rp = zStringHelper::getHTMLRegExp(zStringHelper::HTML_P);
    QRegularExpression rspan = zStringHelper::getHTMLRegExp(zStringHelper::HTML_SPAN);

    auto rti = rtable.globalMatch(txt);

    QStringList name_list{"nev","name","mezo","field","column","oszlop"};
    QStringList type_list{"tipus","type"};
    QStringList comment_list{"comment","megjegyzes","komment","leiras"};

    auto pkstr = QStringLiteral("pk");

    int table_ix=0;
    while(rti.hasNext())
    {

        QList<zTablerow> rowlist;
        int row_ix=0;
        QString tablename;
        QString pkname;
        auto rtm = rti.next();
        QString table_txt=rtm.captured(1);


        int ix_name=-1;
        int ix_type=-1;
        int ix_comment=-1;

        if(!table_txt.isEmpty())
        {
//            if(table_txt.contains(QStringLiteral("Törzsadatok")))
//                zTrace();
            //tbody
            auto rtbodym = rtbody.match(table_txt);
            if(rtbodym.hasMatch())
            {
                table_txt = rtbodym.captured(1);
            }
            //tr

            auto rtri = rtr.globalMatch(table_txt);
            while(rtri.hasNext())
            {
                auto rtrm = rtri.next();
                QString tr_txt=rtrm.captured(1);


                QString row_name;
                QString row_type;
                QString row_comment;

                if(!tr_txt.isEmpty())
                {
                    auto tdi = rtd.globalMatch(tr_txt);
                    int col_ix = 0;
                    while(tdi.hasNext())
                    {
                        auto tdm = tdi.next();

                        QString td_txt = tdm.captured(1);

//                        if(table_txt.contains(QStringLiteral("Törzsadatok")))
//                            zTrace();

//                        if(!td_txt.isEmpty())
//                        {
//                            auto rpm = rp.match(td_txt);
//                            if(rpm.hasMatch())
//                            {
//                                td_txt = rpm.captured(1);
//                            }
//                        }

                        if(!td_txt.isEmpty())
                        {
                            auto rspani = rspan.globalMatch(td_txt);
                            QString innerspan;
//                            if(table_txt.contains(QStringLiteral("PARTNER_ARTICLES")))
//                                zTrace();

                            while(rspani.hasNext())
                            {
                                auto rspanm = rspani.next();
                                if(!innerspan.isEmpty())
                                {
                                    innerspan+=" ";
                                }
                                innerspan += rspanm.captured(1);
                            }
                            td_txt = innerspan;
                        }


                        switch (row_ix)
                        {
                            case 0:
                            {
                                if(!td_txt.isEmpty() && !td_txt.contains('<'))
                                {
                                    if(!tablename.isEmpty())
                                    {
                                        tablename+="_";
                                    }
                                    tablename+=td_txt;
                                }
                                //zTrace(QStringLiteral("tablename: %1").arg(td_txt));
                                break;
                            }
                            case 1:
                            {
//                                if(table_txt.contains(QStringLiteral("Törzsadatok")))
//                                    zTrace();

                                QString a = zStringHelper::zNormalize(td_txt);

                                if(name_list.contains(a))
                                {
                                    ix_name=col_ix;
                                    //zTrace(QStringLiteral("ix_name: %1 (%2)").arg(col_ix).arg(td_txt));
                                }
                                if(type_list.contains(a))
                                {
                                    ix_type=col_ix;
                                    //zTrace(QStringLiteral("ix_type: %1 (%2)").arg(col_ix).arg(td_txt));
                                }
                                if(comment_list.contains(a))
                                {
                                    ix_comment=col_ix;
                                    //zTrace(QStringLiteral("comment_list: %1 (%2)").arg(col_ix).arg(td_txt));
                                }                               
                                break;
                            }
                            default:
                            {
                                if(col_ix==ix_name)
                                {
                                    row_name = td_txt;
                                }

                                if(col_ix==ix_type)
                                {
//                                    if(tablename=="fue")
//                                    {
//                                        zTrace("");
//                                    }
                                    QString a = zStringHelper::zNormalize(td_txt);
                                    if(a.endsWith(pkstr))
                                    {
                                        pkname = row_name;

                                        td_txt.chop(pkstr.length());
                                    }
                                    row_type = td_txt;
                                    //zTablerow::
                                }

                                if(col_ix==ix_comment)
                                {
                                    row_comment = td_txt;
                                }
                                break;
                            }
                        }
                        col_ix++;
                    }
                }
                if(row_ix>=2)
                {
                    if(!sName.isEmpty() && zStringHelper::zNormalize(tablename)!=sName)
                    {
                        continue;
                    }
                    //if(table_txt.contains(QStringLiteral("Törzsadatok")))
                    //    zTrace();

                    if(ix_name!=-1 && ix_type!=-1 && !row_type.isEmpty())
                    {
                        QString row_dtype;
                        int dlen = 0;
                        bool isNullable = false;

//                        if(tablename.toLower()=="fue" && row_name.toLower()=="text")
//                        {
//                            zDebug();
//                        }
                        //row_type=int - nem találja a dtype-ot
                        zTable::getClassType(globalSqlMaps, row_type, &row_dtype, &dlen, &isNullable, false, true);
                        //auto gtype = zTable::getClassType(globalClassMaps, propType, &dtype, &dlen, &isNullable, isRequired);

                        auto row = zTablerow(row_name, row_dtype, dlen, isNullable, "");
                        row.comment = row_comment;
                        rowlist.append(row);

                        //zTrace(QStringLiteral("row %1: %2,%3,%4").arg(row_ix).arg(row_name, row_type, row_comment));
                    }
                    else break;
                    //{
                        //zTrace(QStringLiteral("hibás sor %1: %2").arg(row_ix).arg(tr_txt));
                        //zInfo(QStringLiteral("hibás sor %1: %2,%3,%4").arg(row_ix).arg(row_name, row_type, row_comment));

                    //}

                }

                row_ix++;
            }
            //td
            //p?
            //span?
        }
        if(rowlist.isEmpty()) continue;
        auto t = zTable(tablename, pkname, rowlist);

        t.docName = tablename;
        //t.name = "";
        zInfo(QStringLiteral("zTable: %1").arg(t.toString()));
        e.append(t);
        table_ix++;
    }

    return e;
}
