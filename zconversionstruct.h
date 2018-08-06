#ifndef ZCONVERSIONSTRUCT_H
#define ZCONVERSIONSTRUCT_H

#include <QString>

class zConversionStruct
{
public:
    zConversionStruct(const QString& in, const QString& out,  const QString &dir);

    QString in_value;
    QString out_value;
    QString direction;
};

#endif // ZCONVERSIONSTRUCT_H
