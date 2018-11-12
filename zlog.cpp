#include "globals.h"
#include "zlog.h"
#include <QMessageBox>
#include <QTextCodec>
#include <csignal>
#include <execinfo.h>
#include <cxxabi.h>


//zLog::zLog() = default;

//zLog::~zLog() = default;

QTextBrowser* zLog::widget2;
QTabWidget* zLog::tabwidget2;
int zLog::tabindex2;
bool zLog::isBreakOnError;

void zLog::init(QTextBrowser* b, QTabWidget* tw, int tabindex, bool isBreak){
//    widget = b;
//    tabwidget = tw;
//    tabindex = tabindex;

    widget2=b;
    tabwidget2 = tw;
    tabindex2 = tabindex;
    isBreakOnError = isBreak;
}

//void zLog::log(const QString& m){
//    #ifdef QT_DEBUG

//    if(m.endsWith("OK")){
//        log(m.left(m.length()-2), OK);
//    }
//    else if(m.endsWith("ERROR")){
//        log(m.left(m.length()-5), ERROR);
//    }
//    else if(m.endsWith("TRACE")){
//        log(m.left(m.length()-5), TRACE);
//    }
//    else{
//        log(m, -1);
//    }
//    #endif
//}

//void zLog::log(const QString& m, int errlevel){
//   // #ifdef QT_DEBUG

//    auto c = widget->textColor();

//    switch(errlevel){

//    case ERROR:
//        widget->setTextColor(QColor(Qt::darkRed));
//        tabwidget->setCurrentIndex(tabindex);
//        break;
//    case WARNING:
//        widget->setTextColor(QColor(Qt::darkYellow));
//        tabwidget->setCurrentIndex(tabindex);
//        break;
//    case TRACE:
//        widget->setTextColor(QColor(Qt::darkGray));
//        break;
//    case DEBUG:
//        widget->setTextColor(QColor(Qt::darkRed));
//        break;
//    case INFO:
//        widget->setTextColor(QColor("steelblue"));
//        break;
//    case OK:
//        widget->setTextColor(QColor(Qt::darkGreen));
//        break;
//    default:
//        widget->setTextColor(QColor(Qt::black));
//        break;
//    }

//    widget->append(m);
//    widget->setTextColor(c);
//    //#endif
//}

//void zLog::log(const QList<QString>& ml){
//    zforeach(m, ml){
//        this->log(*m);
//        }
//}

/*log*/

QString zLog::LevelToString(int i)
{
    switch(i)
    {
        case ERROR:
            return QStringLiteral("Error");
            break;
        case WARNING:
            return QStringLiteral("Warning");
            break;
        case TRACE:
            return QStringLiteral("Trace");
            break;
        case DEBUG:
            return QStringLiteral("Debug");
            break;
        case INFO:
            return QStringLiteral("Info");
            break;
        case OK:
            return QStringLiteral("Info");
            break;
        default:
            return QStringLiteral("");
            break;
    }

}

void zLog::dialog(const QString& str, int errlevel) {
//    QString h;
    //switch(errlevel){

//    case ERROR:
//        h = QStringLiteral("Error");
//        break;
//    case WARNING:
//        h = QStringLiteral("Warning");
//        break;
//    case TRACE:
//        h = QStringLiteral("Trace");
//        break;
//    case DEBUG:
//        h = QStringLiteral("Debug");
//        break;
//    case INFO:
//        h = QStringLiteral("Info");
//        break;
//    case OK:
//        h = QStringLiteral("Info");
//        break;
//    default:
//        h = QStringLiteral("");
//        break;
//    }

    auto h = LevelToString(errlevel);
    QMessageBox messageBox;
    QMessageBox::critical(nullptr, h, str);
    messageBox.setFixedSize(500, 200);
}
/*message*/



//void zLog::message(const QList<QString>& ml){
//    zforeach(m, ml)
//    {
//        message(*m);
//    }
//}


//void zLog::message(const QString& m){
//    log(m, INFO);
//    }

//void zLog::message(const char *m){
//    message(QString::fromLocal8Bit(m));
//    }

/*ok*/



//void zLog::ok(const QList<QString>& ml){
//    zforeach(m, ml){
//        ok(*m);
//        }
//    }

//void zLog::ok(const QString& m){
//    log(m, OK);
//    }

//void zLog::ok(const char *m){
//    ok(QString::fromLocal8Bit(m));
//    }





void zLog::dialogMessage(const QString& str) {
    dialog(str, INFO);
}

void zLog::dialogTrace(const QString& str) {
    dialog(str, TRACE);
}

void zLog::dialogWarning(const QString& str) {
    dialog(str, WARNING);
}

void zLog::dialogError(const QString& str) {
    dialog(str, ERROR);
}

void zLog::dialogOk(const QString& str) {
    dialog(str, OK);
}

//void zLog::trace(const char *m){
//    QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));
//    trace(QString::fromLocal8Bit(m));
//    }

//void zLog::trace(const QString& msg){
//    log(msg, TRACE);
//}

//void zLog::trace(const QList<QString>& ml){
//    zforeach(m, ml){
//        trace(*m);
//        }
//    }

//void zLog::trace(const char *m, const QString& msg2){
//    trace(QString::fromLocal8Bit(m) + ", " + msg2);
//    }

/*void zLog::trace(const QString& msg, const QString& msg2){
    trace(msg + ": " + msg2);
}

/*warning*/



//void zLog::warning(const QString& m){
//    log(m, WARNING);
//    }

//void zLog::warning(const QList<QString>& ml){
//    zforeach(m, ml){
//        warning(*m);
//        }
//    }*/

//void zLog::warning(const char *m){
//    warning(QString::fromLocal8Bit(m));
//    }


/*error*/



//void zLog::error(const QString& m){
//    log(m, ERROR);
//    }

//void zLog::error(const QString& msg, const QString& msg2){
//    error(msg + ": " + msg2);
//}

//void zLog::error(const QList<QString>& ml){
//    zforeach(m, ml){
//        error(*m);
//        }
//    }


/*
log to gui -> QString
*/
QString zLog::logToGUI(int errlevel, const QString &msg, const QString &loci, const QString &st)
{
    auto level = LevelToString(errlevel);
    auto c = widget2->textColor();
    QString msg3;

    switch(errlevel){
    case ERROR:
        widget2->setTextColor(QColor(Qt::darkRed));
        tabwidget2->setCurrentIndex(tabindex2);
        widget2->append(level+": "+msg);
        widget2->append(loci);
        widget2->append(st);
        msg3= level+": "+msg+"\n"+loci;
        break;
    case WARNING:
        widget2->setTextColor(QColor(Qt::darkYellow));
        tabwidget2->setCurrentIndex(tabindex2);
        widget2->append(level+": "+msg);
        widget2->append(loci);
        msg3= level+": "+msg+"\n"+loci;
        break;
    case TRACE:
        widget2->setTextColor(QColor(Qt::darkGray));
        widget2->append(level+": "+loci);
        msg3= level+": "+loci;
        break;
    case DEBUG:
        widget2->setTextColor(QColor(Qt::darkRed));
        tabwidget2->setCurrentIndex(tabindex2);
        widget2->append(level);
        widget2->append(loci);
        widget2->append(st);
        msg3= level+": "+msg+"\n"+loci;
        break;
    case INFO:
        widget2->setTextColor(QColor("steelblue"));
        widget2->append(msg);
        msg3= level+": "+msg;
        break;
    case OK:
        widget2->setTextColor(QColor(Qt::darkGreen));
        break;
    default:
        widget2->setTextColor(QColor(Qt::black));
        break;
    }

    widget2->setTextColor(c);
    return msg3;
}

//QString zLog::zGetLocInfo(const char *func, const char *file, int line)
//{
//    auto e = QStringLiteral("%2:%3 %1").arg(func,file).arg(line);
//    return e;
//}

QString zLog::zStackTrace()
{
    QStringList e;

    unsigned int max_frames = 64;

    e << QStringLiteral("stack trace:");

    // storage array for stack trace address data
    void* addrlist[max_frames+1];

    // retrieve current stack addresses
    int addrlen = backtrace(static_cast<void**>(addrlist), sizeof(addrlist) / sizeof(void*));

    if (addrlen == 0)
    {
        e << QStringLiteral("<empty, possibly corrupt>");
        return e.join('\n');
    }

    // resolve addresses into strings containing "filename(function+address)", this array must be free()-ed
    auto symbollist = backtrace_symbols(static_cast<void**>(addrlist), addrlen);

    // allocate string which will be filled with the demangled function name
    size_t funcnamesize = 256;
    auto funcname = static_cast<char*>(malloc(funcnamesize));

    // iterate over the returned symbol lines. skip the first, it is the address of this function.
    for (int i = 1; i < addrlen; i++)
    {
    char *begin_name = nullptr, *begin_offset = nullptr, *end_offset = nullptr;

    for (char *p = symbollist[i]; *p; ++p)
    {
        if (*p == '(')
        {
            begin_name = p;
        }
        else if (*p == '+')
        {
            begin_offset = p;
        }
        else if (*p == ')' && begin_offset)
        {
            end_offset = p;
            break;
        }
    }

    if (begin_name && begin_offset && end_offset
        && begin_name < begin_offset)
    {
        *begin_name++ = '\0';
        *begin_offset++ = '\0';
        *end_offset = '\0';

        int status;
        char* ret = abi::__cxa_demangle(begin_name,funcname, &funcnamesize, &status);
        if (status == 0)
        {
            funcname = ret; // use possibly realloc()-ed string
            //e << QStringLiteral("%1: %2 + %3").arg(symbollist[i],funcname,begin_offset);
            e << QStringLiteral("%1").arg(funcname);
        }
        else
        {
            // demangling failed. Output function name as a C function with
            // no arguments.
            //e << QStringLiteral("%1: %2 + %3").arg(symbollist[i],begin_name,begin_offset);
            e << QStringLiteral("%1").arg(begin_name);
        }
    }
    else
    {
        // couldn't parse the line? print the whole line.
        e << QString(symbollist[i]);
    }
    }

    free(funcname);
    free(symbollist);

    auto a =  e.join('\n');
    return a;
}
//QString zLog::zStackTrace()
//{
//    QStringList e;
//    void *array[10];
//     size_t size;
//     char **strings;
//     size_t i;

//     size = backtrace (array, 10);
//     strings = backtrace_symbols (array, size);

//     e << QStringLiteral("Obtained %1 stack frames./n").arg(size);

//     for (i = 0; i < size; i++)
//       e<< QStringLiteral("%1\n").arg(strings[i]);

//     free (strings);
//    auto a = e.join("\n");
//    return QString(a);
//}

void zLog::error2(const QString& msg, const zLocInfo& locinfo){
    auto li = locinfo.toString();
    auto st = zLog::zStackTrace();
    //logtogui -> to widget, logtostring
    auto msg2 = logToGUI(ERROR, msg, li, st);
    // logtosql, csv...
    // logto messagelogger <- logtostring
    // msg2 = logToString(ERROR, msg, li, st);
#ifdef QT_DEBUG
    qCritical().noquote() << msg2;
    if(isBreakOnError) std::raise(SIGTRAP);
#endif
    }

void zLog::warning2(const QString& msg, const zLocInfo& locinfo){
    auto li = locinfo.toString();
    auto msg2 = logToGUI(WARNING, msg, li, nullptr);
#ifdef QT_DEBUG
    qWarning().noquote() << msg2;
#endif
    }

void zLog::info2(const QString& msg)
{
    auto msg2 = logToGUI(INFO, msg, nullptr, nullptr);
#ifdef QT_DEBUG
    qInfo().noquote() << msg2;
#endif
}

void zLog::info2(const QStringList& msgl)
{
    zforeach(msg,msgl)
    {
        auto msg2 = logToGUI(INFO, *msg, nullptr, nullptr);
#ifdef QT_DEBUG
        qInfo();//.noquote() << msg2;
#endif
    }
}

void zLog::debug2(const zLocInfo& locinfo){
    auto li = locinfo.toString();
    auto st = zLog::zStackTrace();
    auto msg2 = logToGUI(DEBUG, nullptr, li, st);
#ifdef QT_DEBUG
    qDebug().noquote() << msg2;
    if(isBreakOnError) std::raise(SIGTRAP);
#endif
    }

void zLog::trace2(const zLocInfo& locinfo){
    auto li = locinfo.toString();
    auto msg2 = logToGUI(TRACE, nullptr, li, nullptr);
#ifdef QT_DEBUG
    qDebug().noquote() << msg2;
#endif
    }
