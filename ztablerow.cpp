#include "ztablerow.h"
#include "globals.h"
#include "zstringhelper.h"
#include "zxmlhelper.h"

#include <QXmlStreamWriter>

zTablerow::zTablerow(){
     this->isNullable = true;
}

zTablerow::zTablerow(QString colName, QString dtype, int dlen, bool nullable, QString caption){
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


zTablerow* zTablerow::getByName(QList<zTablerow> *rows, QString rn){
    if(rn.isEmpty()) return nullptr;
    zforeach(r,*rows){
        if(!r->colName.isEmpty())
            if(r->colName == rn){
                zTablerow *r2 = r.operator->();
                return r2;
            }
        }
    return nullptr;
}

QList<QString> zTablerow::Validate(zTablerow* rv){
    QList<QString> e;
    e.append(QString("row name: '%1' OK").arg(colName));

    e.append(ValidateCaption(rv->Caption));
    e.append(ValidateColType(rv->colType));
    e.append(ValidateNullable(rv->isNullable));
    e.append(ValidateDLen(rv->dlen));

    return e;
}

QString zTablerow::ValidateCaption(QString rvCaption){
    if(rvCaption.isEmpty())
        return "Caption NOT_TO_VALIDATE";
    else{
        if(Caption.isEmpty())
            return "Caption NOT_EXIST ERROR";
        else{
            if(Caption == rvCaption)
                return "Caption OK";
            else
                return QString("Caption NOT_EQUALS: '%1', '%2' ERROR").arg(Caption).arg(rvCaption);
            }
        }
    }


QString zTablerow::ValidateColType(QString rvcolType){
    if(rvcolType.isEmpty())
        return "Type NOT_TO_VALIDATE";
    else{
        if(colType.isEmpty())
            return "Type NOT_EXIST ERROR";
        else{
            if(!typeMap.contains(colType))
                return QString("Type '%1' is NOT_VALID ERROR").arg(colType);
             else{
                if(colType == rvcolType)
                   return "Type OK";
                else{
                    QString c1 = typeMap.value(colType);
                    QString c2 = typeMap.value(rvcolType);
                    if(c1 == c2)
                        return QString("Type EQUALS: '%1(%3)', '%2(%4)' OK").arg(colType,rvcolType,c1,c2);
                    else
                        return QString("Type NOT_EQUALS: '%1(%3)', '%2(%4)' ERROR").arg(colType,rvcolType,c1,c2);
                    }
                }
            }
        }
    }

QString zTablerow::ValidateNullable(bool rvnullable){
        if(isNullable == rvnullable)
           return "Nullable OK";
        else
            return QString("Nullable NOT_EQUALS: '%1', '%2' ERROR").arg(isNullable).arg(rvnullable);
    }

QString zTablerow::ValidateDLen(int rvdLen){
        if(colType.isEmpty())
            return "Length: Type NOT_EXIST ERROR";
        else{
            if(!typeMap.contains(colType))
                return QString("Length '%1' is NOT_VALID ERROR").arg(colType);
             else{
                if(colType.endsWith("char")){
                    if(this->dlen == rvdLen)
                        return "Length OK";
                    else
                        return QString("Length NOT_EQUALS: '%1', '%2' ERROR").arg(dlen).arg(rvdLen);
                    }
                else{
                    return "Length NOT_TO_VALIDATE";
                }
            }
        }
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



