#include "globals.h"
#include "zsourcehelper.h"
#include "ztextfilehelper.h"
#include "zfilenamehelper.h"

#include <QString>
#include <QStringList>
#include <QRegularExpression>
#include <signal.h>
#include <QMap>

const QString zSourceHelper::p_class_full = QString(R"((?:\b(?:public|private|protected|internal|protected internal|private protected|)\s+)?(?:\bstatic\s+)?class\s+(%1)\s+(\{(?>[^{}]+|(?2))*\}))");
const QString zSourceHelper::p_class_short = QString(R"(class\s+(%1)\s+(\{(?>[^{}]+|(?2))*\}))");

const QString zSourceHelper::p_word = QString(R"(\w+)");

const QString zSourceHelper::p_const = QString(R"(public\s+const\s+(?:\w+)\s+(\w+)\s+=\s+(.*)\s*;)");

const QString zSourceHelper::p_attr = QString(R"((?:\[[(.\w)]*\]))");

const QRegularExpression zSourceHelper::r_const = QRegularExpression(p_const, QRegularExpression::MultilineOption|QRegularExpression::UseUnicodePropertiesOption);

void zSourceHelper::getConstValuesFromFile(QString fullPath, QStringList constNameList, QMap<QString, QString>* constValueMap){
    auto rootClassName = zFileNameHelper::getfileName(fullPath);
    QString txt = zTextFileHelper::load(fullPath);

    auto rootClass_body = getClass(txt, rootClassName);

    QMap<QString, QString> classMap;

    getChildClasses(rootClass_body, rootClassName, &classMap);

    zforeach(m, classMap){
        //QString cnk = m.key();
        //if(cnk.startsWith('#')){
            //QString cn = cnk.right(cnk.length()-1);
            getConstMembers(m.value(), m.key(), constValueMap);
        //}
    }

    //raise(SIGTRAP);
}

void zSourceHelper::getConstMembers(QString txt, QString parentClassName, QMap<QString, QString> *map){
    auto r_const = QRegularExpression(p_const, QRegularExpression::MultilineOption|QRegularExpression::UseUnicodePropertiesOption);

    auto i_const = r_const.globalMatch(txt);
    while(i_const.hasNext()){
        QRegularExpressionMatch m_const = i_const.next();
        auto k = parentClassName+'.'+m_const.captured(1);
        auto v = m_const.captured(2);
        map->insert(k, v);
    }
}

/*
elkezdjük kiszedni az osztályokat
*/
void zSourceHelper::getChildClasses(QString txt, QString parentClassName, QMap<QString, QString> *map){
   // QMap<QString, QString> e;
    auto r_class = getRegex_r_fullClass();

    auto i_class = r_class.globalMatch(txt);
    while(i_class.hasNext()){
        QRegularExpressionMatch m_class = i_class.next();
        auto k = parentClassName+'.'+m_class.captured(1);
        auto v = m_class.captured(2);
        //map->insert(k, v);
        getChildClasses(v, k, map);
    }
   QString rtxt = txt.remove(r_class);
   map->insert(parentClassName, rtxt);
   //map->insert("#"+parentClassName, rtxt);
   //return rtxt;
}



QString zSourceHelper::getClass(QString str, QString className){
    auto r_class = getRegex_r_fullClass(className);

    QPair<QString, QString> p;

    auto m = r_class.match(str);
    if(m.hasMatch()){
        return m.captured(2);
    }

    return QString::null;
}


QRegularExpression zSourceHelper::getRegex_r_class_or_attr() {
    return  QRegularExpression(p_attr+"|"+p_class_short, QRegularExpression::MultilineOption|QRegularExpression::UseUnicodePropertiesOption);
}

QRegularExpression zSourceHelper::getRegex_r_fullClass(QString className){
    QString p = p_class_full.arg((className.isEmpty())?p_word:className);
    return  QRegularExpression(p, QRegularExpression::MultilineOption|QRegularExpression::UseUnicodePropertiesOption);
}

QRegularExpression zSourceHelper::getRegex_r_class(){
    QString p = p_class_short.arg(p_word);
    return  QRegularExpression(p, QRegularExpression::MultilineOption|QRegularExpression::UseUnicodePropertiesOption);
}

QRegularExpression zSourceHelper::getRegex_r_attr(){
    return QRegularExpression(p_attr, QRegularExpression::MultilineOption|QRegularExpression::UseUnicodePropertiesOption);
}


