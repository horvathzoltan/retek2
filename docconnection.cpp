//#include <utility>
#include "globals.h"
#include "zstringhelper.h"
#include "docconnection.h"
#include <QStringList>
#include "ztextfilehelper.h"

docConnection::docConnection(QString _name, QString _path)
{
    this->Name = std::move(_name);
    this->path = std::move(_path);
}

docConnection docConnection::FromCSV(const QString& i){
    QStringList a = i.split(zStringHelper::SEP);
    if(a.count()==2)
    {
        return docConnection(a[0],a[1]);
    }
    return docConnection(zStringHelper::Empty,zStringHelper::Empty);
}

QString docConnection::toCSV() const
{
    return Name+zStringHelper::SEP+path;
}

QList<docConnection> docConnection::fromCSVFile(const QString& fn)
{
    auto txt = zTextFileHelper::load(fn);

    QList<docConnection> a;
    if(txt.isEmpty()) return a;

    QStringList csvl = zStringHelper::toStringList(txt);

    zforeach(csvr, csvl)
    {
        auto c = docConnection::FromCSV(*csvr);
        if(c.isValid())
        {
            a << c;
        }
    }
    return a;

}

bool docConnection::isValid(){
    return !(
        (Name.isEmpty() || Name.contains(zStringHelper::SEP)) ||
        (path.isEmpty() || path.contains(zStringHelper::SEP))
                    );
}
