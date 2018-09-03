#include "ziconhelper.h"

#include <QIcon>
#include <QPainter>

zIconHelper::zIconHelper()
{

}


QIcon zIconHelper::concatenate()
{
    QIcon dummyIcon;
    QPixmap comboPixmap(64, 32);

    comboPixmap.fill();

    QPixmap p1(QStringLiteral(":/database.ico"));
    QPixmap p2(QStringLiteral(":/file-text.ico"));

//    QIcon i1 = QIcon::fromTheme(QStringLiteral(":/database.ico"));
//    QIcon i2 = QIcon::fromTheme(QStringLiteral(":/file-text.ico"));

//    auto p1 = i1.pixmap(i1.availableSizes().first());
//    auto p2 = i2.pixmap(i2.availableSizes().first());

    QPainter painter(&comboPixmap);
    painter.drawPixmap(0, 0, p1);
    painter.drawPixmap(p1.width(), 0, p2);

    dummyIcon.addPixmap(comboPixmap);

   // championbtn->setIcon(dummyIcon);

    return dummyIcon;
}
