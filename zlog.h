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


    void trace(const QString&);       

    [[deprecated]]
    void log(const QString&);

    void log(const QString&, int);

    [[deprecated]]
    void log(const QList<QString>&);

    /**/
    static void errorDialog(const QString& str);
};

#endif // ZERROR_H
