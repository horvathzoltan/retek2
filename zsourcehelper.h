#ifndef ZSOURCEHELPER_H
#define ZSOURCEHELPER_H

#include <QString>
#include <QMap>


class zSourceHelper
{
private:
    static const QString p_word;
    static const QString p_class_full;
    static const QString p_class_short;
    static const QString p_attr;
    static const QString p_const;
    static const QRegularExpression r_const;
public:    
    static void getConstValuesFromFile(QString fileName, QStringList constNameList, QMap<QString, QString>* constValueMap);
    static QRegularExpression getRegex_r_class_or_attr();
    static QRegularExpression getRegex_r_fullClass(QString = QString::null );
    static QRegularExpression getRegex_r_class();
    static QRegularExpression getRegex_r_attr();

    static QString getClass(QString, QString);
    //static QPair<QString,QString> getClass(QString, QString);

    static void getChildClasses(QString txt, QString, QMap<QString, QString>*);
    static void getConstMembers(QString txt, QString parentClassName, QMap<QString, QString> *map);
};

#endif // ZSOURCEHELPER_H
