//#include <utility>
#include "globals.h"
#include "zstringhelper.h"
#include "srcconnection.h"
#include <QStringList>
#include "ztextfilehelper.h"

srcConnection::srcConnection(QString _name, QString _path)
{
    this->Name = std::move(_name);
    this->path = std::move(_path);
}

srcConnection srcConnection::FromCSV(const QString& i){
    QStringList a = i.split(zStringHelper::SEP);
    if(a.count()==2)
    {
        return srcConnection(a[0],a[1]);
    }
    return srcConnection(zStringHelper::Empty,zStringHelper::Empty);
}

QString srcConnection::toCSV() const
{
    return Name+zStringHelper::SEP+path;
}

QList<srcConnection> srcConnection::fromCSVFile(const QString& fn)
{
    auto txt = zTextFileHelper::load(fn);

    QList<srcConnection> a;
    if(txt.isEmpty()) return a;

    QStringList csvl = zStringHelper::toStringList(txt);

    zforeach(csvr, csvl)
    {
        auto c = srcConnection::FromCSV(*csvr);
        if(c.isValid())
        {
            a << c;
        }
    }

}

bool srcConnection::isValid(){
    return !(
            (Name.isEmpty() || Name.contains(zStringHelper::SEP)) ||
            (path.isEmpty() || path.contains(zStringHelper::SEP))
        );
}
