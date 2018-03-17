#ifndef ZTEXTFILEHELPER_H
#define ZTEXTFILEHELPER_H

#include <QFile>
#include <QTextStream>
#include <QTextCodec>
#include <QString>

class zTextFileHelper
{
public:
    static QString load(QString filename);
    static void save(QString txt, QString fileName, bool isAppend = false);
    static void append(QString line, QString fileName);
};

#endif // ZTEXTFILEHELPER_H
