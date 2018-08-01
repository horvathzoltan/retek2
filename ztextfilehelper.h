#ifndef ZTEXTFILEHELPER_H
#define ZTEXTFILEHELPER_H


#include <QFile>
#include <QString>
#include <QTextCodec>
#include <QTextStream>


class zTextFileHelper
{
public:
    static QString load(const QString& filename);
    static void save(const QString& txt, const QString& fileName, bool isAppend = false);
    static void append(QString line, QString fileName);       
};

#endif // ZTEXTFILEHELPER_H
