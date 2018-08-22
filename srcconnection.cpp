#include <utility>

#include <utility>

#include "zstringhelper.h"
#include "srcconnection.h"
#include <QStringList>


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


bool srcConnection::isValid(){
    return !(
            (Name.isEmpty() || Name.contains(zStringHelper::SEP)) ||
            (path.isEmpty() || path.contains(zStringHelper::SEP))
        );
}
