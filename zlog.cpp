//#include "globals.h"
#include "zlog.h"
#include "zshortguid.h"
#include <QMessageBox>
#include <QTextCodec>
#include <csignal>
#ifdef Q_OS_LINUX
#include <execinfo.h>
#include <cxxabi.h>
#endif

bool zLog::isBreakOnError = false;
zLogGUIfn zLog::GUILogger = nullptr;
void* zLog::ui = nullptr;
bool zLog::isVerbose = false;

const QString zLog::OK = QStringLiteral("ok");
const QString zLog::ERROR = QStringLiteral("error");
const QString zLog::WARNING = QStringLiteral("warning");
const QString zLog::FAILED = QStringLiteral("failed");

void zLog::init(zLogGUIfn ez, bool _isBreakOnError, void* uiptr, bool _isVerbose)
{
    GUILogger = ez;
    isBreakOnError = _isBreakOnError;
    ui=uiptr;
    isVerbose = _isVerbose;
}

const QMap<ErrLevels, QString> zLog::ErrLevelNames{
       {ErrLevels::ERROR, nameof(ErrLevels::ERROR)},
       {ErrLevels::WARNING, nameof(ErrLevels::WARNING)},
       {ErrLevels::TRACE, nameof(ErrLevels::TRACE)},
       {ErrLevels::DEBUG, nameof(ErrLevels::DEBUG)},
       {ErrLevels::INFO, nameof(ErrLevels::INFO)},
       {ErrLevels::INFOAPPEND, nameof(ErrLevels::INFOAPPEND)},
       {ErrLevels::INFOCLOSE, nameof(ErrLevels::INFOCLOSE)}
   };

/*log*/


//const QMap<ErrLevels, QString> zLog::ErrLevelNames
//{
//    {ErrLevels::ERROR, nameof(ErrLevels::ERROR)},
//    {ErrLevels::WARNING, nameof(ErrLevels::WARNING)},
//    {ErrLevels::TRACE, nameof(ErrLevels::TRACE)},
//    {ErrLevels::DEBUG, nameof(ErrLevels::DEBUG)},
//    {ErrLevels::INFO, nameof(ErrLevels::INFO)},
//    {ErrLevels::INFOAPPEND, nameof(ErrLevels::INFOAPPEND)},
//    {ErrLevels::INFOCLOSE, nameof(ErrLevels::INFOCLOSE)}
//};


//QString zLog::LevelToString(ErrLevels i)
//{
//    switch(i)
//    {
//        case ErrLevels::ERROR:
//            return QStringLiteral("Error");
//            break;
//        case ErrLevels::WARNING:
//            return QStringLiteral("Warning");
//            break;
//        case ErrLevels::TRACE:
//            return QStringLiteral("Trace");
//            break;
//        case ErrLevels::DEBUG:
//            return QStringLiteral("Debug");
//            break;
//        case ErrLevels::INFO:
//            return QStringLiteral("Info");
//            break;
//        default:
//            return zStringHelper::Empty;
//            break;
//    }

//}


//TODO ha van gui, akkor az külső dependencyként meg kellene jelenjen itt

void zLog::dialog(const QString& str, ErrLevels errlevel) {

    auto h = ErrLevelNames[errlevel];
    QMessageBox messageBox;
    QMessageBox::critical(nullptr, h, str);
    messageBox.setFixedSize(500, 200);
}
/*message*/


void zLog::dialogMessage(const QString& str) {
    dialog(str, ErrLevels::INFO);
}

void zLog::dialogTrace(const QString& str) {
    dialog(str, ErrLevels::TRACE);
}

void zLog::dialogWarning(const QString& str) {
    dialog(str, ErrLevels::WARNING);
}

void zLog::dialogError(const QString& str) {
    dialog(str, ErrLevels::ERROR);
}


//#pragma GCC diagnostic push
//#pragma GCC diagnostic ignored "-Wunused-parameter"
QString zLog::logToString(ErrLevels errlevel, const QString &msg, const QString &loci, const QString &st)
{
    auto level = ErrLevelNames[errlevel];
    QString msg3;

    switch(errlevel)
    {
    case ErrLevels::ERROR:
        msg3= level+": "+msg+"\n"+loci+"\n"+st;
        break;
    case ErrLevels::WARNING:
        msg3= level+": "+msg+"\n"+loci;
        break;
    case ErrLevels::TRACE:
        msg3= level+": "+loci;
        break;
    case ErrLevels::DEBUG:
        msg3= level+": "+msg+"\n"+loci+"\n"+st;
        break;
    case ErrLevels::INFO:
        msg3= level+": "+msg;
        if(isVerbose)
        {
            msg3+="\n"+loci;
        }
        break;
    default:
        break;
    }

    return msg3;
}
//#pragma GCC diagnostic pop



#ifdef Q_OS_LINUX
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
            e << QStringLiteral("%1: %2 + %3").arg(symbollist[i],ret,begin_offset);
            //e << QStringLiteral("%1").arg(funcname);
        }
        else
        {
            // demangling failed. Output function name as a C function with
            // no arguments.
            e << QStringLiteral("%1: %2 + %3").arg(symbollist[i],begin_name,begin_offset);
            //e << QStringLiteral("%1").arg(begin_name);
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

#endif

#ifdef Q_OS_WIN
QString zLog::zStackTrace(){
    QStringList e;

   // unsigned int max_frames = 64;

    e << QStringLiteral("stack trace:");

    // WIN implementációt adni
    e << QStringLiteral("?");

    auto a =  e.join('\n');

    return a;
}
#endif


void zLog::error2(const QString& msg, const zLocInfo& locinfo){
    auto li = locinfo.toString();
    auto st = zLog::zStackTrace();

    if(GUILogger!=nullptr)
    {
        GUILogger(ErrLevels::ERROR, msg, li, st, ui);
    }
    auto msg2 = logToString(ErrLevels::ERROR, msg, li, st);

#ifdef QT_DEBUG
#ifdef Q_OS_WIN
    auto a = __FUNCTION__;
#elif defined(Q_OS_LINUX)
    auto a = static_cast<const char*>(__PRETTY_FUNCTION__);
#endif
    QMessageLogger(QT_MESSAGELOG_FILE, QT_MESSAGELOG_LINE, a).critical().noquote()<<msg2;
#ifdef Q_OS_LINUX
    if(isBreakOnError) std::raise(SIGTRAP);
#endif
#endif
    }

void zLog::warning2(const QString& msg, const zLocInfo& locinfo){
    auto li = locinfo.toString();
    if(GUILogger!=nullptr)
    {
        GUILogger(ErrLevels::WARNING, msg, li, nullptr, ui);
    }
    auto msg2 = logToString(ErrLevels::WARNING, msg, li, nullptr);
#ifdef QT_DEBUG
#ifdef Q_OS_WIN
    auto a = __FUNCTION__;
#elif defined(Q_OS_LINUX)
    auto a = static_cast<const char*>(__PRETTY_FUNCTION__);
#endif
    QMessageLogger(QT_MESSAGELOG_FILE, QT_MESSAGELOG_LINE, a).warning().noquote()<<msg2;
#endif
    }

void zLog::info2(const QString& msg, const zLocInfo& locinfo)
{
    QString li;
    if(isVerbose)
    {
        li = locinfo.toString();
    }
    if(GUILogger!=nullptr)
    {
        GUILogger(ErrLevels::INFO, msg, nullptr, nullptr, ui);
    }    

    auto msg2 = logToString(ErrLevels::INFO, msg, li, nullptr);
#ifdef QT_DEBUG
#ifdef Q_OS_WIN
    auto a = __FUNCTION__;
#elif defined(Q_OS_LINUX)
    auto a = static_cast<const char*>(__PRETTY_FUNCTION__);
#endif
    QMessageLogger(QT_MESSAGELOG_FILE, QT_MESSAGELOG_LINE, a).info().noquote()<<msg2;
#endif
}

void zLog::info2(const QStringList& msgl, const zLocInfo& locinfo)
{
    QString li;
    if(isVerbose)
    {
        li = locinfo.toString();
    }

    zforeach(msg,msgl)
    {
        if(GUILogger!=nullptr)
        {
            GUILogger(ErrLevels::INFO, *msg, nullptr, nullptr, ui);
        }
        auto msg2 = logToString(ErrLevels::INFO, *msg, nullptr, nullptr);
#ifdef QT_DEBUG
#ifdef Q_OS_WIN
    auto a = __FUNCTION__;
#elif defined(Q_OS_LINUX)
        auto a = static_cast<const char*>(__PRETTY_FUNCTION__);
#endif
        QMessageLogger(QT_MESSAGELOG_FILE, QT_MESSAGELOG_LINE, a).info().noquote()<<msg2;
#endif
    }
}

void zLog::debug2(const zLocInfo& locinfo){
    auto li = locinfo.toString();
    auto st = zLog::zStackTrace();
    if(GUILogger!=nullptr)
    {
        GUILogger(ErrLevels::DEBUG, nullptr, li, st, ui);
    }
    auto msg2 = logToString(ErrLevels::DEBUG, nullptr, li, st);
#ifdef QT_DEBUG
#ifdef Q_OS_WIN
    auto a = __FUNCTION__;
#elif defined(Q_OS_LINUX)
    auto a = static_cast<const char*>(__PRETTY_FUNCTION__);
#endif
    QMessageLogger(QT_MESSAGELOG_FILE, QT_MESSAGELOG_LINE, a).debug().noquote()<<msg2;
#ifdef Q_OS_LINUX
    if(isBreakOnError) std::raise(SIGTRAP);
#endif
#endif
    }

void zLog::trace2(const zLocInfo& locinfo){
    auto li = locinfo.toString();
    if(GUILogger!=nullptr)
    {
        GUILogger(ErrLevels::TRACE, nullptr, li, nullptr, ui);
    }
    auto msg2 = logToString(ErrLevels::TRACE, nullptr, li, nullptr);
#ifdef QT_DEBUG  
#ifdef Q_OS_WIN
    auto a = __FUNCTION__;
#elif defined(Q_OS_LINUX)
    auto a = static_cast<const char*>(__PRETTY_FUNCTION__);
#endif
   QMessageLogger(QT_MESSAGELOG_FILE, QT_MESSAGELOG_LINE, a).debug().noquote()<<msg2;
#endif
    }

QString zLog::openInfo(const QString& msg)
    {

        QString e= zShortGuid::createNew().value;

        if(GUILogger!=nullptr)
        {
            GUILogger(ErrLevels::INFO, e+' '+msg, nullptr, nullptr, ui);
        }
      return e;
    }

void zLog::appendInfo(const QString& key, const QString& msg)
    {

    if(GUILogger!=nullptr)
    {
        GUILogger(ErrLevels::INFOAPPEND, msg, key, nullptr, ui);
    }
    }

void zLog::closeInfo(const QString& key)
    {

        if(GUILogger!=nullptr)
        {
            GUILogger(ErrLevels::INFOCLOSE, nullptr, key, nullptr, ui);
        }
    }
