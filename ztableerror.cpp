#include "ztableerror.h"


#include <QRegularExpression>


const QString zTableError::colNamePattern = QStringLiteral(R"(([\w]*).([\w]*).([\w]*):[\w]*)");

const QRegularExpression zTableError::colNameRegexp = QRegularExpression(colNamePattern);

const zTableError zTableError::Empty {zStringHelper::Empty,zStringHelper::Empty,zStringHelper::Empty,zStringHelper::Empty, zStringHelper::Empty};

//zTableError zTableError::Parse(const QString& tn)
//{
//    auto m = colNameRegexp.match(tn);

//    if(m.hasMatch())
//    {
//        return { m.captured(1), m.captured(2), m.captured(3),m.captured(4)};
//    }

//    return Empty;
//}

//QString zTableError::toString()
//{
//    return '['+this->tableName+'.'+this->rowName+'.'+this->colName+':'+this->message+']';
//}

QString zTableError::toString() const
{
    QString e = this->description;
    if(!this->params.isEmpty())
    {
        e.append(" ("+this->params.join(',')+')');
    }
    if(!this->source.isEmpty())
    {
        e.prepend(this->source+": ");
    }
    return e;
}

zTableError::zTableError(const QString &tn, const QString &rn, const QString &cn, const QString &msg,  const QString& src, const QString &desc, const QStringList &l)
{
    this->tableName = tn;
    this->rowName = rn;
    this->colName = cn;
    this->message = msg;
    this->description = desc;
    this->params = l;
    this->source = src;
//    if(!l.isEmpty())
//    {
//        auto d = '('+l.join(',')+')';
//        this->description += d;
//        //this->message += d;
//    }
}

bool zTableError::isValid() const
{
    return !(this->rowName.isEmpty()&&this->tableName.isEmpty()&&this->colName.isEmpty());
}

void zTableError::addDescription(const QString &a)
{
    this->description.prepend(a+' ');
}
