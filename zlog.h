#ifndef ZERROR_H
#define ZERROR_H

#include <QString>
#include <QTextBrowser>
#include <QTabWidget>
#include <zstringhelper.h>
#include "zlocinfo.h"

#if defined(Q_OS_LINUX)
#define getLocInfo zLocInfo(static_cast<const char*>(__PRETTY_FUNCTION__),__FILE__,__LINE__)
#elif defined(Q_OS_WIN)
#define getLocInfo zLocInfo(__FUNCSIG__ ,__FILE__,__LINE__)
#endif


#define zError(msg) zLog::error2((msg), getLocInfo)

#define zWarning(msg) zLog::warning2((msg), getLocInfo);
#define zInfo(msg) zLog::info2((msg), getLocInfo);
#define zDebug() zLog::debug2(getLocInfo);
#define zTrace() zLog::trace2(getLocInfo);

// 1. log egy messaget
// 2. szerez loc infot
// 3. szerez debug infot (stack)

typedef void (*zLogGUIfn)(int errlevel, const QString &msg, const QString &loci, const QString &st, void *ui);

//QT_MESSAGELOG_FILE, QT_MESSAGELOG_LINE, QT_MESSAGELOG_FUNC

class zLog
{
private:    
    static zLogGUIfn GUILogger;
//    QTextBrowser *widget;
//    QTabWidget *tabwidget;
//    int tabindex;
    static void dialog(const QString&, int);
//    void log(const QString&, int);

//    static QTextBrowser *widget2;
//    static QTabWidget *tabwidget2;
//    static int tabindex2;
    static bool isBreakOnError;
    static bool isVerbose;

    static void *ui;

    static QString logToString(int, const QString&, const QString&, const QString&);


    //static QString zGetLocInfo(const char *func, const char *file, int line);
    static QString zStackTrace();

public:        
    enum ErrLevels:int {ERROR, WARNING, TRACE, DEBUG, INFO, INFOAPPEND, INFOCLOSE};
    //zLog();
    //~zLog();
    static QString LevelToString(int loglevel);

    //void init(QTextBrowser*, QTabWidget*, int,bool);
    static void init(zLogGUIfn ez, bool isBreak, void* ui, bool isVerbose);


//    [[deprecated]]
//    void log(const QString&);
//    void log(const QString&, int);

//    [[deprecated]]
//    void log(const QList<QString>&);

    /*message*/
    static void dialogMessage(const QString& str);
//    //void message(const char*);
//    void message(const QString&);
//    void message(const QList<QString>&);

//    /*ok*/
    static void dialogOk(const QString& str);
//    //void ok(const char*);
//    void ok(const QString&);
//    void ok(const QList<QString>&);

//    /*warning*/
    static void dialogWarning(const QString& str);
//    //void warning(const char*);
//    void warning(const QString&);
//    void warning(const QList<QString>&);

//    /*error*/
    static void dialogError(const QString& str);
//    //void error(const char*);
//    void error(const QString&);
//    void error(const QString&, const QString&);
//    void error(const QList<QString>&);

//    /*trace*/
    static void dialogTrace(const QString& str);
//    //void trace(const char*);
//    void trace(const QString&);
//   // void trace(const char*, const QString&);
//    void trace(const QString&, const QString&);
//    void trace(const QList<QString>&);

    static void error2(const QString& msg, const zLocInfo& l);
    static void warning2(const QString& msg, const zLocInfo& l);
    static void info2(const QString& msg, const zLocInfo& l);
    static void info2(const QStringList& msg, const zLocInfo& l);
    static void debug2(const zLocInfo& l);
    static void trace2(const zLocInfo& l);

    static QString openInfo(const QString& txt);
    static void appendInfo(const QString& key, const QString& txt);
    static void closeInfo(const QString& key);
};

#endif // ZERROR_H
