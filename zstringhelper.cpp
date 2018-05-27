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
    if(ezt.compare("0", Qt::CaseInsensitive)) return true;
    return false;
}

/*
 * ponttal elválasztott tagok esetén
*/
QString zStringHelper::toCamelCase(QString s)
{
    auto o = s.split('.');

    for (int i = 0; i < o.length(); i++)
        o[i][0] = o[i][0].toUpper();

    return o.join("");
}

//ClassnameCamelCase
QString zStringHelper::getClassNameCamelCase(QString tnev) {
    QString t2 = tnev.toLower();
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

QStringList zStringHelper::toStringList(QString s){
    return s.split(QRegExp("(\\r\\n)|(\\n\\r)|\\r|\\n"), QString::SkipEmptyParts);
}

//QString zStringHelper::singularize(QString s)
//{
//    if(s.endsWith('s')){
//        return s.left(s.length()-1);
//    }
//    else{
//        return s;
//    }
//}

//QString zStringHelper::pluralize(QString s)
//{
//    if(s.endsWith('s')){
//        return s+"es";
//    }
//    else{
//        return s+"s";
//    }
//}


//QString zStringHelper::singularizeAll(QString s)
//{
//    auto o = s.split('.');

//    for (int i = 0; i < o.length(); i++)
//        o[i] = singularize(o[i]);

//    return o.join(".");
//}

