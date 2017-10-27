#include "ztablerow.h"
#include "globals.h"

zTablerow::zTablerow(){
     this->nullable = true;
}

zTablerow::zTablerow(QString colName, QString dtype, int dlen, bool nullable, QString caption){
    this->colName = colName;
    this->colType=dtype;
    this->dlen = dlen;
    this->nullable = nullable;
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
    e.append(ValidateNullable(rv->nullable));
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
                else
                    return QString("Type NOT_EQUALS: '%1', '%2' ERROR").arg(colType).arg(rvcolType);
                }
            }
        }
    }

QString zTablerow::ValidateNullable(bool rvnullable){
        if(nullable == rvnullable)
           return "Nullable OK";
        else
            return QString("Nullable NOT_EQUALS: '%1', '%2' ERROR").arg(nullable).arg(rvnullable);
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
                        return "Type OK";
                    else
                        return QString("Length NOT_EQUALS: '%1', '%2' ERROR").arg(dlen).arg(rvdLen);
                    }
                else{
                    return "Length NOT_TO_VALIDATE";
                }
            }
        }
    }


