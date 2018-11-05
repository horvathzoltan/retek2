#include <QDir>


#include "globals.h"
#include "ztokenizer.h"
#include "zstringhelper.h"
#include "zstringmaphelper.h"
#include "zfilenamehelper.h"
#include "ztextfilehelper.h"
#include "zfilenamehelper.h"

zTokenizer::zTokenizer() = default;

void zTokenizer::init(QTableWidget *w){
    this->MezoLista = w;

    QString currentProjectPath = zFileNameHelper::getDxMap();
    zStringMapHelper::StringMapFeltolt(currentProjectPath, &dxMap);
    zlog.ok(QStringLiteral("zTokenizer init ok"));
}


/*
    QString currentProjectPath = zFileNameHelper::append(beallitasok.projectPath,beallitasok.currentProjectName);
    zStringMapHelper::StringMapFeltolt(currentProjectPath, &this->dxMap);
    zlog.log("a");
*/
//void zTokenizer::feltoltTmpMap(void){
//    QString tn = "View";
//    QString viewTemplateDirName = beallitasok.getTemplateFilename(tn);


//    auto viewTemplateDir = QDir(viewTemplateDirName);

//    auto filelist = viewTemplateDir.entryList(QDir::NoDotAndDotDot | QDir::System | QDir::Hidden | QDir::AllDirs | QDir::Files, QDir::DirsFirst);//(QDir::Filter::Files,QDir::SortFlag::NoSort)

//    zforeach(f, filelist) {
//        QString k = *f;
//        auto ix = f->indexOf('.');
//        if (ix > -1) k = f->left(ix);

//        QString v = zTextFileHelper::load(viewTemplateDirName + "\\" + *f);
//        tmpMap.insert(k, v);
//    }
//}


/*!
Szmolni kell, hány tokent sikerl feloldani. az utols léps az, amelyikben már nem sikerül
tokent feloldani, tehát marad még token, de ezeket már nem sikerül feloldani.

Ezzel el lehet kerlni, hogy:
1. a templateban logiktlan szerkezetek jelenjenek meg
2. a kd byonyolultsgn lehet cskkenteni.

ha for ciklust hasznlunk, akkor elkerlhet a vgtelen ciklus, s maximalizlhat a template begyazottsga,
for(int i = 10 ... ha elrtk a 10-et, akkor a template too complex
*/

void zTokenizer::tokenize(QString *tmp, QMap<QString, QVariant>*map, int whsp, const QString& dbname) {
    int szint = 0;
    tokenizeR(tmp, 0, &szint, map, whsp, dbname);
}

int zTokenizer::tokenizeR(QString *tmp, int ix, int* szint, QMap<QString, QVariant> *map, int whsp, const QString& dbname) {
    int ix1 = 0;
    //int ix2 = ix;
    int spc = 0;
    for (int i = ix; i < tmp->length(); i++) {
        if (i == -1) continue;

        if((*tmp)[i] == ' ')
        {
            spc++;
        }
        else if ((*tmp)[i] == '\t')
        {
            spc+=8;
        }
        if ((*tmp)[i] == '<' && (*tmp)[i + 1] == '%' && (*tmp)[i + 2] == '=')
        {
            (*szint)++;
            i = tokenizeR(tmp, i + 3, szint, map, whsp+spc, dbname);
        }
        else if ((*tmp)[i] == '%' && (*tmp)[i + 1] == '>')
        {
            ix1 = i;
            (*szint)--;
            break;
        }
        else
        {
            if((*tmp)[i] != ' ')
            {
                spc=0;
            }
        }


        if (*szint > 0) {
        if ((*tmp)[i] == '(' && (*tmp)[i + 1] == '\"') {
            (*szint)++;
            i = tokenizeR(tmp, i + 2, szint, map, whsp+spc, dbname);
        }
        else if ((*tmp)[i] == '\"' && (*tmp)[i + 1] == ')') {
            ix1 = i;
            (*szint)--;
            break;
        }
        }
    }


    if (*szint == 0 && (ix1 != ix) ) {
        int l1 = ix1 - ix;

        QString eredmeny = tmp->mid(ix, ix1 - ix);
        //QString eredmeny2 = eredmeny;
        QString parameter;

        int ix_p1 = eredmeny.indexOf(QStringLiteral("(\""));
        if (ix_p1 > -1) {
            int ix_p2 = eredmeny.indexOf(QStringLiteral("\")"), ix_p1+2);
            if (ix_p2 > -1) {
                parameter = eredmeny.mid(ix_p1 + 2, ix_p2 - ix_p1 -2).trimmed();
                }
            eredmeny = eredmeny.left(ix_p1).trimmed();
            }
        else
        {
            eredmeny = eredmeny.trimmed();
        }

        eredmeny = getToken(eredmeny, parameter, map, whsp, dbname);
        if (eredmeny.isEmpty()) {
            tmp->remove(ix - 3, ix1 - ix + 5);
            ix1 -= l1 + 5;
        }
        else{
            int l2 = eredmeny.length();
            tmp->replace(ix - 3, ix1 - ix + 5, eredmeny);
            ix1 += l2 - l1 - 5;
        }
    }
    return ix1;
}

/*
  A template-ban lévő token megnevezéseket oldja fel a megfelelő tárolt - vagy számított értékek alapján

a zTablesben előre meg kell generálni az értékeket - így azok felülírhatóvá válnak

- vagy egyszerű értéket ad vissza, vagy imlementált funkciót hív

*** sql - adatbázis szintű tokenek
dbname - adatbázisnév / egyadatbázisos project esetén a model névtere
tablename - adattábla neve -

*** MVC tokenek
attrlist - attribútumlista - viewmodell adatannotációs attribútumok

contextname - az adat kontext neve -
newline - új sor

*** osztálydefiníciós tokenek
class_name - az osztály neve - a modellként használatos osztály neve
proplist - egy adott templattel definiált szintaxisú  - propertyk listája
propline - propertyfelsorolás (pl. vesszővel elválasztott)

***entity framework tokenek

entityname - class_name?
entity_attrlist -
prop_attrlist -

nav_proplist -
nav_proptype -
nav_propname -
*/
QString zTokenizer::getToken(const QString& token1, const QString& t2, QMap<QString, QVariant> *map, int whsp, QString dbname) {
    auto t1List = token1.split(' ', QString::SplitBehavior::SkipEmptyParts);
    QString t1 = t1List[0];
    QString t3 = (t1List.length()>1) ? t1List[1] : nullptr;
    QString eredmeny;

    // sql tokenek
    if (t1 == QStringLiteral("dbname"))
    {
        eredmeny = dbname;
    }
    else if (t1 == QStringLiteral("tablename"))
    {
        eredmeny= table->name;
    }
    else if (t1 == QStringLiteral("contextname"))
    {
        eredmeny= dbname;
    }
    // osztály tokenek
    else if (t1 == QStringLiteral("classname"))
    {
        eredmeny= table->class_name;//zStringHelper::getclass_nameCamelCase(table->tablename);
    }
    else if (t1 == QStringLiteral("proplist"))
    {
        eredmeny= getPropList2(t2, t3, whsp, dbname);
    }
    else if (t1 == QStringLiteral("propline"))
    {
        eredmeny= getPropList2(nullptr, t3, whsp, dbname);
    }
    // MVC - viewmodel adatannotációs attribútumok
    else if (t1 == QStringLiteral("attrlist"))
    {
        auto a = getAttrList(map, whsp);
        eredmeny= a;}
    // entitás tokenek
    else if (t1 == QStringLiteral("entity_attrlist")) {
        auto a =  getEntityAttrList(map, whsp);
        eredmeny= a;
    }
    else if (t1 == QStringLiteral("prop_attrlist"))
    {
        eredmeny= getEntityPropAttrList(map, whsp);
    }
    else if (t1 == QStringLiteral("nav_proplist"))
    {
        eredmeny= getEntityNavPropList(t2, t3, whsp, dbname);
    }
    //else if (t1 == "proplist") return getPropListForEntity(t2, t3, whsp, dbname);

    else if (t1 == QStringLiteral("newline"))
    {
        eredmeny= QStringLiteral("\n");
    }


/*
 * entity_attrlist , entityname , prop_attrlist , nav_proplist , nav_proptype , nav_propname
*/

    //else if (t1 == "dispnameforprop") return getDispnameforprop(t2, map);
    //else if (t1 == "dispforprop") return getDispforprop(t2, map);
    //dispnameforprop
    //dispforprop
    else if (map) {
        auto a = map->value(t1).toString();
        if (!a.isEmpty())
        {
            eredmeny= a;
        }
        else
        {
            auto colname = map->value(QStringLiteral("colname")).toString();
            zlog.error(QStringLiteral("A tokenhez nincs érték párosítva: %1 => %2").arg(t1, colname));
            eredmeny= "?1" + t1 + "?";
        }
    } else{
        zlog.error(QStringLiteral("Token nem oldható fel: %1").arg(t1));
        eredmeny= "?2" + t1 + "?";
    }

//    zlog.log(QString("Ismeretlen token: %1").arg(t1));
//    return "?3" + t1 + "?";
    return eredmeny;
}

/*
tokenelemek feloldsa
*/

//QString zTokenizer::getPropList() {
//    int rows = MezoLista->rowCount();
//    QString proplist = "";

//    for (int i = 0; i < rows; i++) {
//        QTableWidgetItem *item_colName = MezoLista->item(i, C_ix_colName);
//        QTableWidgetItem *item_colType = MezoLista->item(i, C_ix_colType);
//        QTableWidgetItem *item_len = MezoLista->item(i, C_ix_dlen);
//        QTableWidgetItem *item_isnullable = MezoLista->item(i, C_ix_nullable);
//        //QTableWidgetItem *item_Caption = ui.tableWidget_MezoLista->item(i, C_ix_Caption);
//        QString e1;

//        int len = item_len->text().toInt();
//        zStringHelper::toBool(item_isnullable->text());
//        auto pType = getPropType(item_colType->text(), len);
//        auto osztalyNev = getOsztalynevLower(item_colName->text());

//        e1 = QString("public %1 %2 { get; set; }").arg(pType).arg(osztalyNev);

//        proplist += e1 + "// " + item_colName->text() + "\n";
//    }

//    return proplist;
//}

/*
https://www.tutorialspoint.com/asp.net_mvc/asp.net_mvc_data_annotations.htm

System.ComponentModel.DataAnnotations includes the following attributes that impacts the nullability or size of the column.

Key - System.ComponentModel.DataAnnotations
Timestamp
ConcurrencyCheck
Required
MinLength
MaxLength
StringLength

---
System.ComponentModel.DataAnnotations.Schema namespace includes the following attributes that impacts the schema of the database.

Table -  System.ComponentModel.DataAnnotations.Schema
Column
Index
ForeignKey
NotMapped
InverseProperty

*/
QString zTokenizer::getAttrList(QMap<QString, QVariant> *map, int whsp) {
    QString  e;
    QString spacer = QString(whsp, ' ');
    if (!map) return zStringHelper::Empty;

    QString ca = map->value(QStringLiteral("caption")).toString();

    if(ca.isEmpty())
    {
        ca = map->value(QStringLiteral("propname")).toString();
    }


    if(!e.isEmpty()) e+= spacer;

    e += QStringLiteral("[Display(Name = \"%1\")]\n").arg(ca);

    bool isNullable = map->value(QStringLiteral("isnullable")).toBool();

    if(!isNullable && ca.toLower() != QStringLiteral("id")){
        if(!e.isEmpty()) e+= spacer;
        e += QStringLiteral("[Required(ErrorMessage = \"%1 ktk\")]\n").arg(ca);
    }

    QString tyo = map->value(QStringLiteral("proptypeoriginal")).toString();
    QString len = map->value(QStringLiteral("proplen")).toString();

    if (!tyo.isEmpty()) {
        if (tyo == QStringLiteral("datetime")) {
            if(!e.isEmpty()) e+= spacer;
            e += QStringLiteral("[DataType(DataType.DateTime)]\n[DisplayFormat(DataFormatString = \"{0:yyyy.MM.dd}\", ApplyFormatInEditMode = true)]\n");
        }
        else if( tyo.endsWith(QStringLiteral("char")) && len != QStringLiteral("-1")) {
            if(!e.isEmpty()) e+= spacer;
            e += QStringLiteral("[StringLength(%1)]\n").arg(len);
        }
    }

    return  e;
}


/*
property listát ad vissza - a használt típus a globalClassMaps lesz
sql irány -> fieldlist
//mindíg c típusok kellenek
*/
QString zTokenizer::getPropList2(const QString& tmp, const QString& param, int whsp, const QString& dbname) {
    //if(MezoLista)
    int rows = MezoLista->rowCount();

    QString proplist = zStringHelper::Empty;
    QStringList exPropsBynameList;
    if (!param.isEmpty() && param.startsWith('-')) {
        exPropsBynameList = param.right(param.length() - 1).toLower().split(',', QString::SplitBehavior::SkipEmptyParts);
    }

    int j = 0;
    for (int i = 0; i < rows; i++) {

        QTableWidgetItem *item_colName = MezoLista->item(i, C_ix_colName);
        QTableWidgetItem *item_colType = MezoLista->item(i, C_ix_colType);
        QTableWidgetItem *item_len = MezoLista->item(i, C_ix_dlen);
        QTableWidgetItem *item_isnullable = MezoLista->item(i, C_ix_nullable);
        QTableWidgetItem *item_Caption = MezoLista->item(i, C_ix_Caption);

        QString colName = item_colName->text();
        if (exPropsBynameList.contains(colName.toLower())) continue;
        auto propName = colName;//getOsztalynevLower(colName);

        if (!tmp.isEmpty()) {
            QString e1;

            int len = item_len->text().toInt();
            bool isnullable = zStringHelper::toBool(item_isnullable->text());
            auto colType = item_colType->text();            
            QString propType;
            propType = getPropClassType(colType, len); //mindíg c típusok kellenek

//            if(table->sourcetype==zTableSourceTypes::SQL) {
//                propType = getPropType(colType, len);
//            }else{
//                propType = colType;
//            }
            //auto propName = getOsztalynev(colName);
            QString caption = (item_Caption) ? item_Caption->text() : zStringHelper::Empty;
            QString coltypeName = item_colType->text();

            QMap<QString, QVariant> map;
            map.insert(QStringLiteral("proptype"), propType);
            map.insert(QStringLiteral("propname_"), propName.replace('.','_'));
            map.insert(QStringLiteral("propname"), propName);
            map.insert(QStringLiteral("isnullable"), isnullable);
            map.insert(QStringLiteral("proplen"), len);
            map.insert(QStringLiteral("colname"), colName);

//            if (dxMap.contains(colType)) {
//                map.insert("dxWidget", dxMap.value(colType) );
//            }

            //if (!caption.isEmpty())
            map.insert(QStringLiteral("caption"), caption);
            map.insert(QStringLiteral("proptypeoriginal"), coltypeName);

            /*
            ha a tmp !-al kezddik, akkor fájlból jön a template

            !bytype,edit.cshtml : a edit_cshtml sztrbl jn fieldtypeName alapjn
            !bytype,create.cshtml : a create_cshtml sztrbl jn fieldtypeName alapjn
            !bytype,details.cshtml : a details_cshtml sztrbl jn fieldtypeName alapjn

            -- egymsba gyazott mapok kellenek
            */
//            map.insert("controlid", colName+'_'+QString::number(i));

//            if (tmp.startsWith('!')) {
//                auto templatekeyList = tmp.right(tmp.length() - 1).split(',');

//                if (templatekeyList.length() > 2 && templatekeyList.indexOf(colName) > -1)
//                    templatekeyList[0] = "byname";

//                QString tmpkRoot = templatekeyList[0]+ '_'+ templatekeyList[1];
//                QString tmpkDefault = tmpkRoot + "_default";
//                QString tmpk = tmpkRoot;

//                if (templatekeyList[0] == "bytype") {
//                    tmpk += "_" + coltypeName;
//                }
//                else if (templatekeyList[0] == "byname")
//                    tmpk += "_" + colName;

//                if(tmpMap.contains(tmpk))
//                    e1 = tmpMap.value(tmpk);
//                else
//                    e1 = tmpMap.value(tmpkDefault, "?"+ tmpk +"?");
//            }
//            else
//                e1 = tmp;

            e1=tmp;

            tokenize(&e1, &map, whsp, dbname);

            proplist += QString(j++==0?0:whsp, ' ') + e1;// +"// " + item_colName->text();
            if (i < rows - 1)
            {
                proplist += QStringLiteral("\n");
            }
        }
        else {
            //auto propName = getOsztalynev(colName);
            if (!proplist.isEmpty())
            {
                proplist += ',';
            }
            proplist += propName;
        }
    }

    return proplist;
}

//Table,NotMapped
QString zTokenizer::getEntityAttrList(QMap<QString, QVariant> * /*map*/, int w) {
    QStringList attrList;
    if(table->name.isEmpty())
    {
        attrList<< QStringLiteral("[NotMapped]");
    }
    if(table->name!=table->class_name)
    {
        attrList<< QStringLiteral("[Table(\"%1\")]").arg(table->name);
    }
    if(!table->comment.isEmpty())
    {
        attrList<<QStringLiteral("[Description(\"%1\")]").arg(table->name);
    }
    return AttrListJoin(attrList, w);
}

QString zTokenizer::getEntityPropAttrList(QMap<QString, QVariant> *map, int w) {
    QStringList attrList;

    QString colname = map->value(QStringLiteral("colname")).toString();

    auto r = zTablerow::getByName(table->rows, colname);
    //if(!r->comment.isEmpty())

    if(!r->isNullable)
    {
       attrList<< QStringLiteral("[Reqiured]");
    }
    if(table->pkname() == r->colName)
    {
        attrList<< QStringLiteral("[Key]");
    }
    if(r->dlen>0){
        if(r->colType.contains(QStringLiteral("char")))
        {
            attrList<< QStringLiteral("[StringLength(%0)]").arg(r->dlen);
        }else{
            attrList<<QStringLiteral("[MaxLength(%0)]").arg(r->dlen);
        }
    }   

    auto al = AttrListJoin(attrList, w);
    return al;
}

//void zTokenizer::AttrListAdd(QStringList& e, const QString& str, int whsp){
//    e.append(str);
//}

QString zTokenizer::AttrListJoin(const QStringList& e, int w){
    QString eredmeny;

    if(e.length()>1) {
        QString s = e[0];
        for(int i=1;i<e.length();i++){
            s+=("\n"+QString(w, ' ')+e[i]);
        }
        eredmeny =  s+"\n"+QString(w, ' ');
    }
    else if(e.length()==1)
    {
        eredmeny =  e[0]+"\n"+QString(w, ' ');//+QString(w, ' ');
    }
    else
    {
        eredmeny = zStringHelper::Empty;
    }

    return eredmeny;
}

// Ha a zTablesnek van fk-ja
// ha van rfk-ja
QString zTokenizer::getEntityNavPropList(const QString& tmp, const QString&  /*param*/, int whsp, const QString& dbname){
    if(tmp.isEmpty()) return zStringHelper::Empty;

    auto fkList = table->getFKclass_name();

    QStringList propList;
    zforeach(fk, fkList){
        QString propName = *fk;
        QString propType = *fk;

        propList << getProp(propType, propName, tmp, whsp, dbname);
    }

    auto rfkList = table->getRFKclass_namePlural();
    zforeach(rfk, rfkList){
        auto r = (*rfk).split(';');
        QString propType = QStringLiteral("ICollection<%1>").arg(r[0]);
        QString propName = r[1];

        propList << getProp(propType, propName, tmp, whsp, dbname);
    }

    auto al = AttrListJoin(propList, whsp);
    return al;
}

QString zTokenizer::getProp(const QString& propType, const QString& propName, const QString& tmp, int whsp, const QString& dbname){
    QMap<QString, QVariant> map;
    map.insert(QStringLiteral("propname"), propName);
    map.insert(QStringLiteral("proptype"), propType);

    QString e1 = tmp;

    tokenize(&e1, &map, whsp, dbname);

    return e1;
}

// ha a ztable sql típusú, de ha txt típusú, nem kell keresni, mert ugyanaz
//
//QString zTokenizer::getPropType(const QString& tipusnev, bool isnullable) {
//    if(!typeMap.contains(tipusnev)){
//        zlog.error(QStringLiteral("Nem található típus: %1").arg(tipusnev));
//        return zStringHelper::Empty;
//    }

//    QString pt = typeMap.value(tipusnev);//.toString();
//    if (isnullable && !tipusnev.endsWith(QStringLiteral("char"))) pt += '?';
//    return pt;
//}

// belső típus -> osztálytípus a gobális tábla alapján key->value irányban
// ha nullable, akkor a típusnév ?-re fog végződni (lehetne Nullable<típusnév> is)
QString zTokenizer::getPropClassType(const QString& tipusnev, bool isnullable) {
    QString cn = zConversionMap::external(globalClassMaps, tipusnev);
    if(cn.isEmpty())
    {
        zlog.error(QStringLiteral("Nem található classmap value a keyhez: %1").arg(tipusnev));
        return zStringHelper::Empty;
    }
    if (isnullable && !tipusnev.startsWith(QStringLiteral("string"))) cn += '?';
    return cn;
}

QString zTokenizer::getePropType(const QString& tipusnev, int length) {
    QString pt = tipusnev;
    if (length > 0 && tipusnev.endsWith(QStringLiteral("char"))) pt += QStringLiteral("(%1)").arg(length);
    return pt;
}


//const QString zTokenizer::TXT = "txt";



//QString zTokenizer::getOsztalynevLower(QString tnev) {
//    return tnev;
//}

//QString zTokenizer::getContextNev(void) {
//    return data
//}

//QString zTokenizer::get_liNev(QString O) {
//    auto o = O.toLower();

//    if (o.endsWith("_id"))
//        return o.left(o.length() - 3);
//    else if (o.endsWith("id"))
//        return o.left(o.length() - 2);
//    return o;
//}
