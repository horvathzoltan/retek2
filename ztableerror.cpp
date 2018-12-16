#include "ztableerror.h"

#include <QRegularExpression>


const QString zTableError::colNamePattern = QStringLiteral(R"(([\w]*).([\w]*).([\w]*):[\w]*)");

const QRegularExpression zTableError::colNameRegexp = QRegularExpression(colNamePattern);

zTableError zTableError::Parse(const QString& tn)
{
    auto m = colNameRegexp.match(tn);

    if(m.hasMatch())
    {
        return { m.captured(1), m.captured(2), m.captured(3),m.captured(4)};
    }

    return {"","","",""};
}

QString zTableError::toString()
{
    return '['+this->tableName+'.'+this->rowName+'.'+this->colName+':'+this->message+']';
}

bool zTableError::isValid()
{
    return !(this->rowName.isEmpty()&&this->tableName.isEmpty()&&this->colName.isEmpty());
}
