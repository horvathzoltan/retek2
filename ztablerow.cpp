#include "ztablerow.h"
#include "globals.h"
#include "zstringhelper.h"
#include "zxmlhelper.h"
#include "ztableerror.h"

#include <QXmlStreamWriter>




zTablerow::zTablerow(){
    this->isNullable = true;
    this->dlen = 0;
}

zTablerow::zTablerow(const QString& colName, const QString& dtype, int dlen, bool nullable, const QString& caption){
    this->colName = colName;
    this->colType=dtype;
    this->dlen = dlen;
    this->isNullable = nullable;
    this->Caption=caption;    
}

QString zTablerow::toString() const
{
    return this->colName;
}

bool zTablerow::operator==(const zTablerow &o)const {
    return colName==o.colName;
}


zTablerow* zTablerow::getByName(const QList<zTablerow> &rows, const QString& rn)
{
    if(rn.isEmpty()) return nullptr;
    zforeach(r,rows)
    {
        if(!r->colName.isEmpty())
        {
            if(r->colName == rn)
            {
                auto r2 = r.operator->();
                return (zTablerow*)r2;
            }
        }
    }
    return nullptr;
}

//enum class zTablerow::ErrCode:int{noteq, unknown, notex, nullable};

const QMap<zTablerow::ErrCode, QString> zTablerow::ErrCodeNames
{
    {ErrCode::noteq, QStringLiteral("noteq")},
        {ErrCode::unknown, QStringLiteral("unknown")},
        {ErrCode::notex, QStringLiteral("notex")},
        {ErrCode::nullable, QStringLiteral("nullable")}
};

const QMap<zTablerow::ErrCode, QString> zTablerow::ErrCodeDescriptions
{
    {ErrCode::noteq, QStringLiteral("Not Equals")},
        {ErrCode::unknown, QStringLiteral("Unknown")},
        {ErrCode::notex, QStringLiteral("Not Exist")},
        {ErrCode::nullable, QStringLiteral("is Nullable")}
};

/*
sorokat hasonlít össze - metaadataik alapján
*/

bool zTablerow::Compare(const zTablerow& rv, QList<zTableError>& e, const QString& source){

    if(rv.colName=="UnitTypeId"){//UnitTypeId
        zTrace();
    }
    bool v  = true;

    if(this->colName!=rv.colName)
    {
        auto err = GetFullError("colName", ErrCode::noteq, {this->colName, rv.colName}, source);
        e.append(err);//QStringLiteral("colName Not Equals: (%1,%2)").arg(colName, rv.colName));
        v = false;
    }

//    bool ev=true; // lehet validálni, mert van
//    if(this->colType.isEmpty()){
//        auto err = GetFullError("colType", ErrCode::notex, {}, "tableValidate");
//        e.append(err);
//        v = false;
//        ev=false;
//    }

//    if(rv.colType.isEmpty()){
//        auto err = GetFullError("colType", ErrCode::notex, {}, source);
//        e.append(err);
//        v = false;
//        ev=false;
//    }
//!(this->colType.isEmpty() || rv.colType.isEmpty()) &&
    if(this->colType!=rv.colType)
    {
        auto err = GetFullError("colType", ErrCode::noteq, {this->colType, rv.colType}, source);
        e.append(err);//QStringLiteral("colType Not Equals: %1(%2,%3)").arg(colName,colType,rv.colType ));
        v = false;
    }

    if(this->dlen!=rv.dlen)
    {
        auto err = GetFullError("dlen", ErrCode::noteq, { QString::number(this->dlen), QString::number(rv.dlen)}, source);
        e.append(err);//QStringLiteral("dlen not equals: %1(%2,%3)").arg(colName).arg(this->dlen).arg(rv.dlen));
        v = false;
    }

    if(this->isNullable!=rv.isNullable)
    {
        auto err = GetFullError("isNullable", ErrCode::noteq, {zStringHelper::boolToString(this->isNullable), zStringHelper::boolToString(rv.isNullable)}, source);
        e.append(err);
        v = false;
    }

    return v;
}

bool zTablerow::Validate(QStringList& e){
    bool isOK  = true;
    //e.append(QStringLiteral("row name: '%1' OK").arg(colName));
    e.append(CompareCaption(this->Caption));
    e.append(isKnownTypeName(this->colType)?QStringLiteral("type ok: %1").arg(this->colType):QStringLiteral("Valid type: %1").arg(this->colType));
    e.append(ValidateNullable(this->isNullable));
    e.append(ValidateDLen(this->dlen));

    return isOK;
}

/**
A colName kötelező, és egyedi kell legyen / table
A coltype kötelező, és szerepelnie kell az ismert típusok között
*/
bool zTablerow::Validate2(const QStringList& colNames, QList<zTableError>& e, const QString& source, QStringList& errlist){
    bool v = true;
    if(colName.isEmpty())
    {
        errlist<<QStringLiteral("Nincs colName error");
        v = false;
    }       
//    if(find(rows, colName))
//    {
//        zError(QStringLiteral("colName nem egyedi: %1").arg(colName));
//        return false;
//    }
    if(colType.isEmpty()){
        errlist<<QStringLiteral("Nincs típusnév error");
        auto err = GetFullError("colType", ErrCode::notex, {}, source);
        e.append(err);
        v= false;
    }
    else
    {
        if(colNames.count(colName)>1)
        {
            errlist<<QStringLiteral("colName nem egyedi: %1 error").arg(colName);
            v = false;
        }
    //}

        auto isKnownType = isKnownTypeName(colType);
        if(!isKnownType)
        {
            auto err = GetFullError(QStringLiteral("colType"), ErrCode::unknown, {colType}, source);
            e.append(err);
            auto errortxt = QStringLiteral("Ismeretlen típus: %1").arg(colType);
            bool isError = true;
            v= false;
            QStringList tl;
            tl<< zConversionMap::internals(globalClassMaps, colType) << zConversionMap::internals(globalSqlMaps, colType);
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
            errlist<<errortxt;
//            if(isError)
//            {
//                errlist<<(errortxt + " error");
//            }
//            else
//            {
//                errlist<<errortxt;
//            }

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

int zTablerow::findIx(const QList<zTablerow> &rows, const QString &rn)
{
    if(rn.isEmpty()) return -1;
    for(int ix=0;ix<rows.count();ix++)
    {
        if(rn == rows[ix].colName)
        {
            return ix;
        }
    }
    return  -1;
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
az ismert típusok közt szerepel-e
*/
bool zTablerow::isKnownTypeName(const QString& tn)
{
    QStringList knownTypeNames;
    knownTypeNames << zConversionMap::internals(globalClassMaps) << zConversionMap::internals(globalSqlMaps);

    auto v= knownTypeNames.contains(tn);
    return v;
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
//        if(!typeMap.contains(colType))
//        {
//            e= QStringLiteral("Length '%1' is NOT_VALID ERROR").arg(colType);
//        }
//        else
//        {
            //if(colType.endsWith(QStringLiteral("char")))
            if(colType.startsWith(QStringLiteral("string")))
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
   // }
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


//QString zTablerow::GetErrorMessage(const QString& tn, const QString& cn, ErrCode code)
//{
//    auto c = ErrCodeNames[code];
//    auto l = QStringLiteral("[%1.%2.%3:%4]").arg(tn, this->colName, cn, c);
//    return l;
//}

zTableError zTablerow::GetFullError(const QString& cn, ErrCode code, const QStringList& p,const QString& source) const
{
    auto err = zTableError(zStringHelper::Empty, this->colName, cn, ErrCodeNames[code], source, ErrCodeDescriptions[code], p);
//    auto l4 = err.toString();
//    auto l2 = ErrCodeDescriptions[code];
//    auto l3 = '('+p.join(',')+')';
//    QString msg = l2+' '+l3 +' '+ l4;

    return err;
}



const zTablerow::ErrCode* zTablerow::GetErrCode(const QString& a){
    zforeach(v, ErrCodeNames)
    {
        if(v.value()==a)
        {
            return &(v.key());
        }
    }
    return nullptr;
    //auto k = ErrCodeNames.key(a, QString());
}



