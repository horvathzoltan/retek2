#include "zstringhelper.h"
#include "zxmlhelper.h"

#include <QDateTime>

bool zXmlHelper::putXmlAttr(const QXmlStreamAttributes& a, const QString& n, QString *ptr){
    bool isA = a.hasAttribute(n);
    if(isA){
        *ptr = (a.value(n)).toString();
    }
    return isA;
}

bool zXmlHelper::putXmlAttr(const QXmlStreamAttributes& a, const QString& n, int *ptr){
    bool isA = a.hasAttribute(n);
    if(isA){
        *ptr = (a.value(n)).toInt();
    }
    return isA;
}

bool zXmlHelper::putXmlAttr(const QXmlStreamAttributes& a, const QString& n, bool *ptr){
    bool isA = a.hasAttribute(n);
    if(isA){
        QString b = (a.value(n)).toString();
        *ptr = zStringHelper::toBool(b);
    }
    return isA;
}

bool zXmlHelper::putXmlAttr(const QXmlStreamAttributes& a, const QString& n, QDateTime *ptr){
    bool isA = a.hasAttribute(n);
    if(isA){
        QString b = (a.value(n)).toString();
        *ptr = QDateTime::fromString(b);
    }
    return isA;
}
