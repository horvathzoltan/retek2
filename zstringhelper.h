#ifndef ZSTRINGHELPER_H
#define ZSTRINGHELPER_H

#include <QString>
#include <QRegularExpression>



class zStringHelper
{    
public:
    static const QChar SEP;
    static const QString Empty;
    static const QString NewLine;
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

    static const QChar _htmlEntityEndingChars[];

    static const int UNICODE_PLANE00_END;// = 0x00FFFF;
    static const int UNICODE_PLANE01_START;// = 0x10000;
    static const int UNICODE_PLANE16_END;// = 0x10FFFF;

    static const QChar HIGH_SURROGATE_START; //= '\uD800';
    static const QChar LOW_SURROGATE_START;// = '\uDC00';
    static const QChar LOW_SURROGATE_END;// = '\uDFFF';

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
//    static QString encodeEntities(const QString &src, const QString &force);
//    static QString decodeEntities(const QString &src);
    static QString HtmlDecode(const QString& value);
private:
    static const int UnicodeReplacementChar;// = '\uFFFD';
    static const QSet<QChar> htmlEntityEndings;

    enum UnicodeDecodingConformance { Auto, Strict, Compat, Loose};

    static UnicodeDecodingConformance getHtmlDecodeConformance();
    static bool StringRequiresHtmlDecoding(const QString& s);
    static QString HtmlDecode2(const QString &value);
    static void ConvertSmpToUtf16(uint smpChar, QChar leadingSurrogate, QChar trailingSurrogate);
    static QChar HtmlEntityLookup(const QString& e);
    static const QMap<QString,quint16> HtmlNamedEntitiesLatin1;
    static const QMap<QString,quint16> HtmlNamedEntitiesSymbolic;
    static const QMap<QString,quint16> HtmlNamedEntitiesSpecial;

};

#endif // ZSTRINGHELPER_H
