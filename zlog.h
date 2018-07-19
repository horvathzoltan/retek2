#ifndef ZERROR_H
#define ZERROR_H

#include <QString>
#include <QTextBrowser>
#include <QTabWidget>

class zLog
{
private:
    QTextBrowser *widget;
    QTabWidget *tabwidget;
    int tabindex;

public:    
    zLog();
    ~zLog();

    enum ErrLevels:int {OK, ERROR, TRACE };

    void init(QTextBrowser*, QTabWidget*, int);

    static void ShowDialog(const QString& str);
    void trace(const QString&);
    void log(const QString&);

    void log(const QString&, int);

    void log(const QList<QString>&);
};

#endif // ZERROR_H
