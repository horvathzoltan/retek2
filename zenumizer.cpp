#include "zenumizer.h"
#include "globals.h"

//QString cn, QString ft, QMap<int, QString> ms
QString zEnumizer::GenerateEnum(EnumSource es)
{
    zlog.trace("GenerateEnum");
    if(es.cn.isEmpty()||es.ft.isEmpty()) {zlog.trace("no enum name or type");return "";}
    QString vl = "";
    zforeach(m, es.ms){
        if(!vl.isEmpty()) vl+=",\n";
        //auto en = m.value().normalized(QString::NormalizationForm_D).replace(QRegExp("[^a-zA-Z0-9_\\s]"), "").replace(' ', '_').toLower();
        auto en = zStringHelper::zNormalize(m.value());
        vl += QString("\t%1 = %2").arg(en).arg(m.key());
    }

    return QString("enum %1 : %2\n{\n%3\n};").arg(es.cn).arg(es.ft, vl);
}
