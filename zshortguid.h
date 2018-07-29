#ifndef ZSHORTGUID_H
#define ZSHORTGUID_H

#include <QUuid>

class zShortGuid
{
public:
    zShortGuid();
    //zShortGuid(QString value);
    static const zShortGuid Empty;

    zShortGuid(QUuid guid);

    QUuid uuid;
    QString value;

    static zShortGuid createNew();

    static QString Encode(QUuid);
    static QUuid Decode(QString);
};

#endif // ZSHORTGUID_H
