#ifndef ZICONHELPER_H
#define ZICONHELPER_H

#include <QGraphicsPixmapItem>



class zIconHelper
{
public:
    zIconHelper();

    static QIcon concatenate(QStringList iconPaths);
};

#endif // ZICONHELPER_H
