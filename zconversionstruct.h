#ifndef ZCONVERSIONSTRUCT_H
#define ZCONVERSIONSTRUCT_H

#include <QString>

class zConversionStruct
{
public:
    QString internal;
    QString external;
    QString direction;

//    zConversionStruct();
//    ~zConversionStruct();

    zConversionStruct(const QString& internal, const QString& external,  const QString &direction);
};

#endif // ZCONVERSIONSTRUCT_H
