#ifndef ZXMLHELPER_H
#define ZXMLHELPER_H

#include <QXmlStreamWriter>



class zXmlHelper
{
public:
    static bool putXmlAttr(const QXmlStreamAttributes&, const QString& n, QString *ptr);
    static bool putXmlAttr(const QXmlStreamAttributes&, const QString& n, int *ptr);
    static bool putXmlAttr(const QXmlStreamAttributes&, const QString& n, bool *ptr);
    static bool putXmlAttr(const QXmlStreamAttributes&, const QString& n, QDateTime *ptr);
};

#endif // ZXMLHELPER_H
