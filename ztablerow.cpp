#include "ztablerow.h"
#include "globals.h"
#include "zstringhelper.h"
#include "zxmlhelper.h"

#include <QXmlStreamWriter>

zTablerow::zTablerow(){
     this->isNullable = true;
}

zTablerow::zTablerow(const QString& colName, const QString& dtype, int dlen, bool nullable, const QString& caption){
    this->colName = colName;
    this->colType=dtype;
    this->dlen = dlen;
    this->isNullable = nullable;
    this->Caption=caption;    
}

QString zTablerow::toString(){
    return this->colName;
}

bool zTablerow::operator==(const zTablerow &o)const {
    return colName==o.colName;
}


zTablerow* zTablerow::getByName(const QList<zTablerow> &rows, const QString& rn)
{
    if(rn.isEmpty()) return nullptr;
    zforeach(r,rows){
        if(!r->colName.isEmpty())
            if(r->colName == rn){
                auto r2 = r.operator->();
                return (zTablerow*)r2;
            }
        }
    return nullptr;
}

//
QList<QString> zTablerow::Validate(zTablerow* rv){
    QList<QString> e;
    e.append(QStringLiteral("row name: '%1' OK").arg(colName));

    e.append(CompareCaption(rv->Caption));
    e.append(CompareColType(rv->colType));
    e.append(ValidateNullable(rv->isNullable));
    e.append(ValidateDLen(rv->dlen));

    return e;
}

/**
A colName kötelező, és egyedi kell legyen / table
A coltype kötelező, és szerepelnie kell az ismert típusok között
*/
bool zTablerow::Validate2(const QStringList& colNames, const QStringList&  knownTypeNames ){
    bool v = true;
    if(colName.isEmpty())
    {
        zlog.error(QStringLiteral("Nincs colName"));
        v = false;
    }
//    if(find(rows, colName))
//    {
//        zlog.error(QStringLiteral("colName nem egyedi: %1").arg(colName));
//        return false;
//    }
    if(colType.isEmpty()){
        zlog.error(QStringLiteral("Nincs típusnév"));
        v= false;
    }
    else
        {
        if(colNames.count(colName)>1)
        {
            zlog.error(QStringLiteral("colName nem egyedi: %1").arg(colName));
            v = false;
        }
    }

    if(!knownTypeNames.contains(colType))
    {
        auto errortxt = QStringLiteral("Ismeretlen típus: %1").arg(colType);
        bool isError = true;
        v= false;
        QStringList tl;
        tl<< zConversionMap::keys(globalClassMaps, colType) << zConversionMap::keys(globalSqlMaps, colType);
        tl.removeDuplicates();
        if(!tl.isEmpty())
        {
            if(tl.count()>1)
            {
                errortxt += QStringLiteral(" javasolt: %1").arg(tl.join(','));
            }
            else
            {
                errortxt += QStringLiteral(" cserélve: %1").arg(tl.first());
                colType = tl.first();
                isError = false;
            }

        }
        if(isError)
        {
            zlog.error(errortxt);
        }
        else
        {
            zlog.trace(errortxt);
        }

    }

    return v;
}

const zTablerow* zTablerow::find(const QList<zTablerow>& rows, const QString& rn){
    if(rn.isEmpty()) return nullptr;
    zforeach(r, rows){
        if(rn == r->colName)
        {
            return &(*r);//.operator->();
        }
    }
    return nullptr;
}

/*
Ha nem egyezik a megadottal, hiba
*/
QString zTablerow::CompareCaption(const QString& rvCaption){
    QString eredmeny;

    if(rvCaption.isEmpty())
    {
        eredmeny= QStringLiteral("Caption NOT_TO_VALIDATE");
    }
    else
    {
        if(Caption.isEmpty())
        {
            eredmeny= QStringLiteral("Caption NOT_EXIST ERROR");
        }
        else
        {
            if(Caption == rvCaption)
            {
                eredmeny= QStringLiteral("Caption OK");
            }
            else
            {
                eredmeny= QStringLiteral("Caption NOT_EQUALS: '%1', '%2' ERROR").arg(Caption, rvCaption);
            }
        }
    }
    return eredmeny;
}

/*
ez azt is vizsgálja, hogy csereszabatosak-e a colTypeok - de ezt nem kellene, ha minden egy darab belső típusra fordulna ,
mert akkor nem lehetne keresztbe megadni a típusokat, illetve nem lenne függő attól, hogy honnan származik a row - mindíg az egyetemes típus lenne az

a típustáblában szerepel-e
*/
QString zTablerow::CompareColType(const QString& rvcolType){
    QString e;

    if(rvcolType.isEmpty())
    {
        e= QStringLiteral("Type NOT_TO_VALIDATE");
    }
    else
    {
        if(colType.isEmpty())
        {
            e= QStringLiteral("Type NOT_EXIST ERROR");
        }
        else{
            if(!typeMap.contains(colType))
            {
                e= QStringLiteral("Type '%1' is NOT_VALID ERROR").arg(colType);
            }
            else
            {
                if(colType == rvcolType)
                {
                   e= QStringLiteral("Type OK");
                }
                else
                {
                    QString c1 = typeMap.value(colType);
                    QString c2 = typeMap.value(rvcolType);
                    if(c1 == c2)
                    {
                        e= QStringLiteral("Type EQUALS: '%1(%3)', '%2(%4)' OK").arg(colType,rvcolType,c1,c2);
                    }
                    else
                    {
                        e= QStringLiteral("Type NOT_EQUALS: '%1(%3)', '%2(%4)' ERROR").arg(colType,rvcolType,c1,c2);
                    }
                }
            }
        }
    }
    return e;
}

QString zTablerow::ValidateNullable(bool rvnullable){
    QString e;

    if(isNullable == rvnullable)
    {
      e = QStringLiteral("Nullable OK");
    }
    else
    {
        e= QStringLiteral("Nullable NOT_EQUALS: '%1', '%2' ERROR").arg(isNullable).arg(rvnullable);
    }
    return e;
}

QString zTablerow::ValidateDLen(int rvdLen){
    QString e;
    if(colType.isEmpty())
    {
        e= QStringLiteral("Length: Type NOT_EXIST ERROR");
    }
    else
    {
        if(!typeMap.contains(colType))
        {
            e= QStringLiteral("Length '%1' is NOT_VALID ERROR").arg(colType);
        }
        else
        {
            if(colType.endsWith(QStringLiteral("char")))
            {
                if(this->dlen == rvdLen)
                {
                    e= QStringLiteral("Length OK");
                }
                else
                {
                    e= QStringLiteral("Length NOT_EQUALS: '%1', '%2' ERROR").arg(dlen, rvdLen);
                }
            }
            else
            {
                e= QStringLiteral("Length NOT_TO_VALIDATE");
            }
        }
    }
    return e;
}


///
/// \brief zTablerow::toXML
/// \param s
///
/// s as stream
///
void zTablerow::toXML(QXmlStreamWriter *s)
{    
    s->writeStartElement(nameof(zTablerow));
    s->writeAttribute(nameof(this->Caption), this->Caption);
    s->writeAttribute(nameof(this->colName), this->colName);
    s->writeAttribute(nameof(this->colType), this->colType);

    s->writeAttribute(nameof(this->dlen), QString::number(this->dlen));
    s->writeAttribute(nameof(this->isNullable), QString::number(this->isNullable));
    s->writeAttribute(nameof(this->comment), this->comment);

    s->writeEndElement();
}

///
/// \brief zTablerow::fromXML
/// \param xml
/// \return
///
/// r as row
/// s as stream
/// a as attribute
///
zTablerow zTablerow::fromXML(QXmlStreamReader* s){
    zTablerow r;

    auto a = s->attributes();

    zXmlHelper::putXmlAttr(a, nameof(Caption), &(r.Caption));
    zXmlHelper::putXmlAttr(a, nameof(colName), &(r.colName));
    zXmlHelper::putXmlAttr(a, nameof(colType), &(r.colType));
    zXmlHelper::putXmlAttr(a, nameof(dlen), &(r.dlen));
    zXmlHelper::putXmlAttr(a, nameof(isNullable), &(r.isNullable));
    zXmlHelper::putXmlAttr(a, nameof(comment), &(r.comment));

    return r;
}

QStringList zTablerow::colNames(const QList<zTablerow> &rows){
    QStringList colNames;
    zforeach(r,rows){
        colNames << r->colName;
    }
    return colNames;
}



