#include "zconversionstruct.h"

//zConversionStruct::zConversionStruct() = default;
//zConversionStruct::~zConversionStruct() = default;

zConversionStruct::zConversionStruct(const QString& key, const QString& value, const QString& direction)
{
    this->internal = key;
    this->external = value;
    this->direction = direction;
}
