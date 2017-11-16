#include <QSqlQuery>
//#include <QVector>
//#include <QMap>
#include <QVariant>

#include "globals.h"
#include "ztablerow.h"
#include "ztable.h"
#include "zstringmaphelper.h"

#include <QRegularExpression>

zTable::zTable(){};

zTable::~zTable(){};

zTable::zTable(QString n, QString pkn, QList<zTablerow> tr, QList<zTablerow> pl){
    this->tablename = n;
    this->rows = tr;
    this->props = pl;
    this->pkname = pkn;
}


//zTable zTable::LoadFromSQL(QString tablanev, QMap<QString, QString> globalCaptionMap, QString fn)
//{

//    QString commandTextTemplate = "Select "
//        "C.COLUMN_NAME, "
//        "C.DATA_TYPE, "
//        "C.CHARACTER_MAXIMUM_LENGTH, "
//        "C.NUMERIC_PRECISION, "
//        "C.NUMERIC_SCALE, "
//        "C.IS_NULLABLE, "
//        "Case When Z.CONSTRAINT_NAME Is Null Then 0 Else 1 End As IsPartOfPrimaryKey From INFORMATION_SCHEMA.COLUMNS "
//        "As C Outer Apply("
//        "Select CCU.CONSTRAINT_NAME From INFORMATION_SCHEMA.TABLE_CONSTRAINTS As TC "
//        "Join INFORMATION_SCHEMA.CONSTRAINT_COLUMN_USAGE As CCU "
//        "On CCU.CONSTRAINT_NAME = TC.CONSTRAINT_NAME "
//        "Where TC.TABLE_SCHEMA = C.TABLE_SCHEMA And TC.TABLE_NAME = C.TABLE_NAME And TC.CONSTRAINT_TYPE = 'PRIMARY KEY' And CCU.COLUMN_NAME = C.COLUMN_NAME) As Z "
//        "Where C.TABLE_NAME = '%1'";


//    QString commandText = commandTextTemplate.arg(tablanev);



//    QSqlQuery query(commandText);

//    QList<zTablerow> tr;

//    if(query.next()){
//        QMap<QString, QString> tablaCaptionMap;
//        zStringMapHelper::StringMapFeltolt(fn, &tablaCaptionMap);

//        while (query.next()) {
//            QString colName = query.value("COLUMN_NAME").toString();
//            QString dtype = query.value("DATA_TYPE").toString();
//            int dlen = query.value("CHARACTER_MAXIMUM_LENGTH").toInt();
//            bool nullable = query.value("IS_NULLABLE").toBool();
//            QString cn = colName.toLower();

//            QString caption = tablaCaptionMap.contains(cn)?tablaCaptionMap[cn]:globalCaptionMap.contains(cn)?globalCaptionMap[cn]:cn;

//            auto r = zTablerow(colName, dtype, dlen, nullable, caption);

//            tr.append(r);
//        }

//        auto e = zTable(tablanev, tr);
//        return e;
//    }
//    else{
//        //QList<zTablerow> trl;
//        return zTable("", QList<zTablerow>());
//    }
//}

//zTable zTable::LoadFromMySQL(QString tablanev, QMap<QString, QString> globalCaptionMap, QString fn)
//{
//    QString commandTextTemplate = "SELECT "
//                "COLUMN_NAME,"
//                "DATA_TYPE,"
//                "CHARACTER_MAXIMUM_LENGTH,"
//                "NUMERIC_PRECISION,"
//                "NUMERIC_SCALE,"
//                "IS_NULLABLE"
//            "FROM INFORMATION_SCHEMA.COLUMNS "
//            "where table_schema = '%1' and table_name = '%2'";

//    QString commandText = commandTextTemplate.arg("wiki1").arg(tablanev);



//    QSqlQuery query(commandText);

//    QList<zTablerow> tr;

//    if(query.next()){
//        QMap<QString, QString> tablaCaptionMap;
//        zStringMapHelper::StringMapFeltolt(fn, &tablaCaptionMap);

//        while (query.next()) {
//            QString colName = query.value("COLUMN_NAME").toString();
//            QString dtype = query.value("DATA_TYPE").toString();
//            int dlen = query.value("CHARACTER_MAXIMUM_LENGTH").toInt();
//            bool nullable = query.value("IS_NULLABLE").toBool();
//            QString cn = colName.toLower();

//            QString caption = tablaCaptionMap.contains(cn)?tablaCaptionMap[cn]:globalCaptionMap.contains(cn)?globalCaptionMap[cn]:cn;

//            auto r = zTablerow(colName, dtype, dlen, nullable, caption);

//            tr.append(r);
//        }

//        auto e = zTable(tablanev, tr);
//        return e;
//    }
//    else{
//        //QList<zTablerow> trl;
//        return zTable("", QList<zTablerow>());
//    }
//}

QString zTable::toString(){
    QString rs;
    QString ps;
    zforeach(r, this->rows){
        if(!rs.isEmpty()) rs+=",";
        if(r->colName==this->pkname) rs+="PK:";
        rs+=r->toString();
    }

    zforeach(p, this->props){
        if(!rs.isEmpty()) ps+=",";
        //if(p->colName==this->pkname) ps+="PK:";
        ps+=p->toString();
    }

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

               auto fns = fn->split(',', QString::SkipEmptyParts);
               QString fname = fns[0].trimmed();

               if(fns.length()>1){
                   zforeach_from(fn2, fns, 1){
                       auto fn3s= fn2->split(' ', QString::SkipEmptyParts);
                       bool isDtype = false;
                       zforeach(fn3, fn3s){
                           QString ezt1;
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
                           if(zStringMapHelper::contains(&typeMap, ezt1)){
                               QString k = zStringMapHelper::getKey(&typeMap, ezt1);
                               dtype =  typeMap.value(k);;
                               isDtype = true;
                           }
                           else if(zStringMapHelper::contains(&typeMapR, ezt1)){
                               QString k = zStringMapHelper::getKey(&typeMapR, ezt1);
                               dtype =  typeMapR.value(k);;
                               isDtype = true;
                           }
                           else{
                               auto i2 = re_dlen1.match(ezt1);
                               if(i2.hasMatch()){
                                   bool isOK;
                                   int n = i2.captured(1).toInt(&isOK);
                                   if(isOK) dlen = n;
                                   }
                               else{
                                    i2 = re_dlen2.match(ezt1);
                                    if(i2.hasMatch()){
                                        bool isOK;
                                        int n = i2.captured(1).toInt(&isOK);
                                        if(isOK) dlen = n;
                                        }
                                    }
                                }
                            }
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
                                        }
                                    }
                                }
                            }
                        }
                    }
               if(dtype.isEmpty()){
                   auto p = zTablerow(fname, "property", dlen, isNullable, caption);
                   pl.append(p);
                   }
               else{
                   auto r = zTablerow(fname, dtype, dlen, isNullable, caption);
                   rl.append(r);
                   }
            }
            auto t = zTable(tn, pkn, rl, pl);
            tl.append(t);
            zlog.log("GenerateByText: "+t.toString());
        }
    }
    return tl;
}

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
            QList<zTablerow> pl;

            zforeach(fn, fns){ // sorok
               if(fn->isEmpty()) continue;
               if(*fn=="_") continue;

               QString dtype="";
               int dlen = 0;
               bool isNullable = true;
               QString caption = "";

               auto fns = fn->split(' ', QString::SkipEmptyParts);
               QString fname = fns[0].trimmed();

               zlog.trace("sor:"+*fn);

               if(fns.length()>1){ //szavak (mezők)
                    zforeach(fn2, fns){
                        zlog.trace("szó:"+*fn2);
                    }
               }

               if(dtype.isEmpty()){
                   auto p = zTablerow(fname, "property", dlen, isNullable, caption);
                   pl.append(p);
                   }
               else{
                   auto r = zTablerow(fname, dtype, dlen, isNullable, caption);
                   rl.append(r);
                   }
            }
            auto t = zTable(tn, pkn, rl, pl);
            tl.append(t);
            zlog.log("GenerateByText2: "+t.toString());
        }
    }

    return tl;
}

