#ifndef ZSOURCEHELPER_H
#define ZSOURCEHELPER_H

#include <QString>
#include <QMap>


class zSourceHelper
{
private:
    static const QString p_class;
    static const QString p_const;
    static const QRegularExpression r_const;
public:    
    static void getConstValuesFromFile(QString fileName, QStringList constNameList, QMap<QString, QString>* constValueMap);
};

#endif // ZSOURCEHELPER_H
