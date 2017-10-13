#ifndef ZERROR_H
#define ZERROR_H

#include <QString>
#include <QTextBrowser>

class zLog
{
private:
    QTextBrowser *widget;

public:
    zLog();
    ~zLog();

    void init(QTextBrowser*);

    static void ShowDialog(QString str);
    void trace(QString);
    void log(QString);

    void log(QList<QString>);
};

#endif // ZERROR_H
