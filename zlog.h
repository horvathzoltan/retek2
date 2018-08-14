#ifndef ZERROR_H
#define ZERROR_H

#include <QString>
#include <QTextBrowser>
#include <QTabWidget>
#include <zstringhelper.h>

class zLog
{
private:
    QTextBrowser *widget;
    QTabWidget *tabwidget;
    int tabindex;
    static void dialog(const QString&, int);
    void log(const QString&, int);
    enum ErrLevels:int {OK, ERROR, WARNING, TRACE, MESSAGE};

public:    
    //zLog();
    //~zLog();

    void init(QTextBrowser*, QTabWidget*, int);


//    [[deprecated]]
//    void log(const QString&);
//    void log(const QString&, int);

//    [[deprecated]]
//    void log(const QList<QString>&);

    /*message*/
    static void dialogMessage(const QString& str);
    //void message(const char*);
    void message(const QString&);
    void message(const QList<QString>&);

    /*ok*/
    static void dialogOk(const QString& str);
    //void ok(const char*);
    void ok(const QString&);
    void ok(const QList<QString>&);

    /*warning*/
    static void dialogWarning(const QString& str);
    //void warning(const char*);
    void warning(const QString&);
    void warning(const QList<QString>&);

    /*error*/
    static void dialogError(const QString& str);
    //void error(const char*);
    void error(const QString&);
    void error(const QString&, const QString&);
    void error(const QList<QString>&);

    /*trace*/
    static void dialogTrace(const QString& str);
    //void trace(const char*);
    void trace(const QString&);
   // void trace(const char*, const QString&);
    void trace(const QString&, const QString&);
    void trace(const QList<QString>&);
};

#endif // ZERROR_H
