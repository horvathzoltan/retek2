#include "globals.h"
#include "zsourcehelper.h"
#include "ztextfilehelper.h"
#include <QString>
#include <QRegularExpression>

QString zSourceHelper::p_class = QString(R"(class\s+(%1)\s+(\{(?>[^{}]+|(?2))*\}))");
QString zSourceHelper::p_const = QString(R"(public\s+const\s+(?:\w+)\s+(\w+)\s+=\s+(.*)\s*;)");

QRegularExpression zSourceHelper::r_const = QRegularExpression(p_const, QRegularExpression::MultilineOption|QRegularExpression::UseUnicodePropertiesOption);

void zSourceHelper::getConstValuesFromFile(QString fileName, QStringList constNameList, QMap<QString, QString>* constValueMap){

    QString txt = zTextFileHelper::load(fileName);

    zforeach(constName, constNameList){
        QRegularExpression r_class = QRegularExpression(p_class.arg(*constName), QRegularExpression::MultilineOption|QRegularExpression::UseUnicodePropertiesOption)

        auto i_class = r_class.globalMatch(txt);
        while(i_class.hasNext()){
            QRegularExpressionMatch m_class = i_class.next();

            QString class_txt = m_class.captured(2);

            auto i_const = r_const.globalMatch(class_txt);
             while(i_const.hasNext()){
                 QRegularExpressionMatch m_const = i_class.next();

             }

        }
    }

}

