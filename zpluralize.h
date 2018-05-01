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
    static QMap<QString, QString> _irregularPluralsDictionary;
    static QMap<QString, QString> _assimilatedClassicalInflectionDictionary;

    bool IsNoOpWord(QString word);
    bool IsAlphabets(QString word);
    QString GetSuffixWord(QString word, QString *prefixWord);
    bool IsUninflective(QString word);
    bool DoesWordContainSuffix(QString word, QStringList suffixes);
public:
    zPluralizer();
    //QString ToLowerInvariant(QString word);
    QString InternalPluralize(QString word);
    bool IsPlural(QString word);
    QString InternalSingularize(QString );
    bool TryInflectOnSuffixInWord(QString word, QStringList suffixes,  int l, QString newWord, QString *outWord);
};

#endif // ZPLURALIZE_H
