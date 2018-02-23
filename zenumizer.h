#ifndef ZENUMIZER_H
#define ZENUMIZER_H

#include <QString>
#include <QMap>

class zEnumizer
{
public:
    struct EnumSource{
        QString cn;
        QString ft;
        QMap<int, QString> ms;
    };

    static QString GenerateEnum(EnumSource);
};

#endif // ZENUMIZER_H
