#ifndef ZSOURCEHELPER_H
#define ZSOURCEHELPER_H

#include <QString>
#include <QMap>


class zSourceHelper
{
public:
    static QString p_class;
    static QString p_const;

    static QRegularExpression r_const;

    static void getConstValuesFromFile(QString fileName, QStringList constNameList, QMap<QString, QString>* constValueMap);
};

#endif // ZSOURCEHELPER_H
