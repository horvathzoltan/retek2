#include "globals.h"
#include "ziconhelper.h"

#include <QIcon>
#include <QPainter>

zIconHelper::zIconHelper()
{

}


QIcon zIconHelper::concatenate(QStringList icons)
{    
    QIcon dummyIcon;
    if(icons.isEmpty()) return dummyIcon;

    QPixmap comboPixmap(32*icons.count(), 32);
    comboPixmap.fill();//Qt::red
    QPainter painter(&comboPixmap);

    QPixmap alertPixmap(QStringLiteral(":/alert-triangle.ico"));

    int zix = 0;
    zforeach(i, icons)
    {
        bool xflag;
        QString iconname;
        if(i->endsWith(QStringLiteral("|x")))
        {
            xflag = true;
            iconname = i->left(i->length()-2);
        }
        else
        {
            iconname = *i;
            xflag = false;
        }
        QPixmap p1(iconname);
        painter.drawPixmap(zix, 0, p1);
        if(xflag)
        {
            painter.eraseRect(zix+16, 0, 16, 16);
            painter.drawPixmap(zix+16, 0, alertPixmap);
        }

        zix += p1.width();
    }

    //QPixmap p2(QStringLiteral(":/file-text.ico"));

//    QIcon i1 = QIcon::fromTheme(QStringLiteral(":/database.ico"));
//    QIcon i2 = QIcon::fromTheme(QStringLiteral(":/file-text.ico"));

//    auto p1 = i1.pixmap(i1.availableSizes().first());
//    auto p2 = i2.pixmap(i2.availableSizes().first());


//    painter.drawPixmap(0, 0, p1);
//    painter.drawPixmap(p1.width(), 0, p2);

    dummyIcon.addPixmap(comboPixmap);

   // championbtn->setIcon(dummyIcon);

    return dummyIcon;
}
