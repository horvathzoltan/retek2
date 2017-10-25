#include "ztablerow.h"

zTablerow::zTablerow(){}

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
