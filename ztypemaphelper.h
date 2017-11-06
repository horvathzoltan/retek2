#ifndef ZTYPEMAPHELPER_H
#define ZTYPEMAPHELPER_H

#include <QMap>
#include <QString>
#include <QVariant>

class zTypemapHelper
{
public:
    //zTypemapHelper();

    static bool containsKey(QMap<QString, QVariant>&, QString k);
    static bool contains(QMap<QString, QVariant>&, QString v);


};

#endif // ZTYPEMAPHELPER_H
