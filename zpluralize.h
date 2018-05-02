#ifndef ZPLURALIZE_H
#define ZPLURALIZE_H

#include <QString>
#include <QMap>

class zPluralizer
{
private:
    static QMap<QString, QString> _userDictionary;
    static QRegularExpression IsAlphabetsRegexp;
    static QStringList _pronounList;
    static QStringList _uninflectiveWordList;
    static QStringList _uninflectiveSuffixList;
    static QStringList _knownPluralWords;
    static QStringList _knownSingluarWords;
    static QMap<QString, QString> _irregularPluralsDictionary;
    static QMap<QString, QString> _assimilatedClassicalInflectionDictionary;
    static QMap<QString, QString> _wordsEndingWithInxAnxYnxPluralizationDictionary;
    static QMap<QString, QString> _oSuffixPluralizationDictionary;

    static QMap<QString, QString> _classicalInflectionDictionary;
    static QMap<QString, QString> _irregularVerbList;
    static QMap<QString, QString> _wordsEndingWithSisDictionary;
    static QMap<QString, QString> _wordsEndingWithSeDictionary;
    static QMap<QString, QString> _wordsEndingWithSusDictionary;

    static bool IsNoOpWord(QString word);
    static bool IsAlphabets(QString word);
    static QString GetSuffixWord(QString word, QString *prefixWord);
    static bool IsUninflective(QString word);
    static bool DoesWordContainSuffix(QString word, QStringList suffixes);
    static bool TryInflectOnSuffixInWord(QString word, QStringList suffixes,  int l, QString newWord, QString *outWord);
public:
    zPluralizer();

    static QString Pluralize(QString word);
    static QString Singularize(QString );
    static bool IsPlural(QString word);
    static bool IsSingular(QString word);
};

#endif // ZPLURALIZE_H
