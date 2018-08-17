#include "globals.h"
#include "zfilenamehelper.h"
#include "zlog.h"
#include "zstringhelper.h"
#include "ztextfilehelper.h"

#include <QDir>
#include <QRegularExpression>

/*
zStringHelper::zStringHelper()
{

}
*/
const QChar zStringHelper::SEP = ';';
const QString zStringHelper::Empty = QString();

const QString zStringHelper::Equals = QStringLiteral("Equals");

const QStringList zStringHelper::TrueStr = {QStringLiteral("true"), QStringLiteral("yes"), QStringLiteral("ok"), QStringLiteral("1")};
const QStringList zStringHelper::FalseStr = {QStringLiteral("false"), QStringLiteral("no"), QStringLiteral("nok"), QStringLiteral("0")};


bool zStringHelper::toBool(const QString& ezt){
    if(ezt.isEmpty()) return false;    
    return TrueStr.contains(ezt.toLower());
}

QString zStringHelper::boolToString(bool a)
{
    if(a)
    {
        return TrueStr.first();
    }
    return FalseStr.first();
}

QString zStringHelper::boolToString(bool a, const QString& s)
{
    if(a)
    {
        return s;
    }
    return QStringLiteral("Not")+s;
}

/*
 * ponttal elválasztott tagok esetén
*/
QString zStringHelper::toCamelCase(const QString& s)
{
    auto o = s.split('.');

    for (int i = 0; i < o.length(); i++)
        o[i][0] = o[i][0].toUpper();

    return o.join(Empty);
}

//class_nameCamelCase
QString zStringHelper::getclass_nameCamelCase(const QString& tnev) {
    QString t2 = tnev;//.toLower();
    //QString sep = TXT+'.';

    //if(t2.startsWith(sep))
    //    t2 = t2.remove(0,sep.length());


    //auto o = t2.replace('_', '.');//.split('_');

    /*
    for (int i = 0; i < o.length(); i++)
        o[i][0] = o[i][0].toUpper();

    return o.join("");*/

    return toCamelCase(t2.replace('_', '.'));
}

QStringList zStringHelper::toStringList(const QString &s){
    return s.split(QRegExp(QStringLiteral("(\\r\\n)|(\\n\\r)|\\r|\\n")), QString::SkipEmptyParts);
}

// (?:\".*\")|(?:[\d.]+)
QRegularExpression zStringHelper::r_string_or_number = QRegularExpression(QStringLiteral(R"((?:\".*\")|(?:[^\p{L}][\d.]+))"), QRegularExpression::MultilineOption|QRegularExpression::UseUnicodePropertiesOption);

bool zStringHelper::isclass_name(const QString& str){
    auto m = r_string_or_number.match(str);
    auto i = !m.hasMatch();    // ha nincs egyezés, nem konstans
    return i;
}

//littleM|oUsE littleM|iCe
QString zStringHelper::caseFixer(QString minta, QString ezt){
    int l = (minta.length()<ezt.length())?minta.length():ezt.length();
    for(int i=0;i<l;i++){
        QChar c = minta[i];
        QChar cl= c.toLower();
        if(cl==ezt[i].toLower()){
            if(c.isUpper()){
                ezt[i]=c;
            }
            else{
                ezt[i]=cl;
            }
        }
        else
            break;
    }

    return ezt;
}

const QString zStringHelper::p_filename = QStringLiteral(R"((?:file:\/{2})?((?:[a-zA-Z]\:)?(?:[\\\/][\w._]+)+))");
const QRegularExpression zStringHelper::r_filename = QRegularExpression(p_filename, QRegularExpression::MultilineOption|QRegularExpression::UseUnicodePropertiesOption);
// ha fájlnévvel kezdődik, 
QStringList zStringHelper::getFilePaths(const QString& txt, const QStringList& fileExtFilter){
    auto i_filename = r_filename.globalMatch(txt);
    
    QStringList txtlist;
    QStringList fileNameFilter;
    zforeach(f, fileExtFilter){
        fileNameFilter << "*."+(*f);
    }

    while(i_filename.hasNext()){
        QRegularExpressionMatch m_filename = i_filename.next();

        QString filePath = m_filename.captured(1);

        auto filePathInfo = QFileInfo(filePath);


        if(filePathInfo.isFile()){
           txtlist << filePath;
        }
        else if(filePathInfo.isDir()){
            QStringList files = zFileNameHelper::FindFileNameInDir(filePath, Empty, fileNameFilter);
            if(!files.isEmpty()){
                txtlist << files;
                }
            //auto list2 = getTextFromFiles(m_filename.captured(0), fileNameFilter);
        }
        else{

        }

//        / QStringList files = zFileNameHelper::FindFileNameInDir(path, "Data", class_nameFilter);


    }

    return txtlist;
}

QString zStringHelper::zNormalize(const QString& c){
    return c.normalized(QString::NormalizationForm_D).replace(QRegExp("[^a-zA-Z0-9_\\s]"), Empty).replace(' ', '_').toLower();
}



