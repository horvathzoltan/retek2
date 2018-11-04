#ifndef ZSTRINGHELPER_H
#define ZSTRINGHELPER_H

#include <QString>
#include <QRegularExpression>



class zStringHelper
{    
public:
    static const QChar SEP;
    static const QString Empty;
    static const QString Equals;

    static const QStringList TrueStr;
    static const QStringList FalseStr;

    static const QString HTML_TAGPATTERN;
    static const QString HTML_TABLE;
    static const QString HTML_TBODY;
    static const QString HTML_TR;
    static const QString HTML_TD;
    static const QString HTML_P;
    static const QString HTML_SPAN;

    zStringHelper();
    static bool toBool(const QString&);
    static QString boolToString(bool a);
    static QString boolToString(bool a, const QString&);

    static QString toCamelCase(const QString& );
    static QStringList toStringList(const QString& s);

//    static QString singularize(QString s);
//    static QString pluralize(QString s);

//    static QString singularizeAll(QString s);

    static QString getclass_nameCamelCase(const QString& tablanev);
    static bool isclass_name(const QString &str);
    static QRegularExpression r_string_or_number;
    static QString caseFixer(QString, QString);
    static const QString p_filename;
    static const QRegularExpression r_filename;
    static QStringList getFilePaths(const QString &, const QStringList &);
    static QString zNormalize(const QString &c);

    static QRegularExpression getHTMLRegExp(const QString &s);
};

#endif // ZSTRINGHELPER_H
