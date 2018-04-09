#include "zlog.h"
#include "zstringhelper.h"

/*
zStringHelper::zStringHelper()
{

}
*/

bool zStringHelper::toBool(QString ezt){
    if(ezt.isEmpty()) return false;
    ezt = ezt.toLower();
    if(!ezt.compare("yes", Qt::CaseInsensitive)) return true;
    if(!ezt.compare("true", Qt::CaseInsensitive)) return true;
    if(!ezt.compare("ok", Qt::CaseInsensitive)) return true;
    return false;
}

QString zStringHelper::toCamelCase(QString s)
{
    auto o = s.split('.');

    for (int i = 0; i < o.length(); i++)
        o[i][0] = o[i][0].toUpper();

    return o.join("");
}

QStringList zStringHelper::toStringList(QString s){
    return s.split(QRegExp("(\\r\\n)|(\\n\\r)|\\r|\\n"), QString::SkipEmptyParts);
}

QString zStringHelper::singularize(QString s)
{
    if(s.endsWith('s')){
        return s.left(s.length()-1);
    }
    else{
        return s;
    }
}

QString zStringHelper::singularizeAll(QString s)
{
    auto o = s.split('.');

    for (int i = 0; i < o.length(); i++)
        o[i] = singularize(o[i]);

    return o.join(".");
}

