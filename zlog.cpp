#include "globals.h"
#include "zlog.h"
#include <QMessageBox>

//zLog::zLog() = default;

//zLog::~zLog() = default;

void zLog::init(QTextBrowser* b, QTabWidget* tw, int tabindex){
    this->widget = b;
    this->tabwidget = tw;
    this->tabindex = tabindex;
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

void zLog::log(const QString& m, int errlevel){
   // #ifdef QT_DEBUG

    auto c = this->widget->textColor();

    switch(errlevel){
    case OK:
        this->widget->setTextColor(QColor(Qt::darkGreen));
        break;
    case ERROR:
        this->widget->setTextColor(QColor(Qt::darkRed));
        tabwidget->setCurrentIndex(tabindex);
        break;
    case WARNING:
        this->widget->setTextColor(QColor(Qt::darkYellow));
        tabwidget->setCurrentIndex(tabindex);
        break;
    case TRACE:
        this->widget->setTextColor(QColor(Qt::darkGray));
        break;
    case MESSAGE:
        this->widget->setTextColor(QColor(Qt::black));
        break;
    default:
        this->widget->setTextColor(QColor(Qt::black));
        break;
    }

    this->widget->append(m);
    this->widget->setTextColor(c);
    //#endif
}

//void zLog::log(const QList<QString>& ml){
//    zforeach(m, ml){
//        this->log(*m);
//        }
//}

/*log*/

void zLog::dialog(const QString& str, int errlevel) {
    QString h;
    switch(errlevel){
    case OK:
        h = QStringLiteral("OK");
        break;
    case ERROR:
        h = QStringLiteral("Error");
        break;
    case WARNING:
        h = QStringLiteral("Warning");
        break;
    case TRACE:
        h = QStringLiteral("Trace");
        break;
    case MESSAGE:
        h = QStringLiteral("Message");
        break;
    default:
        h = QStringLiteral("Info");
        break;
    }

    QMessageBox messageBox;
    QMessageBox::critical(nullptr, h, str);
    messageBox.setFixedSize(500, 200);
}
/*message*/

void zLog::dialogMessage(const QString& str) {
    dialog(str, MESSAGE);
}

void zLog::message(const QList<QString>& ml){
    zforeach(m, ml)
    {
        message(*m);
    }
}

void zLog::message(const QString& m){
    log(m, MESSAGE);
    }

void zLog::message(const char *m){
    message(QString(m));
    }

/*ok*/

void zLog::dialogOk(const QString& str) {
    dialog(str, OK);
}

void zLog::ok(const QList<QString>& ml){
    zforeach(m, ml){
        ok(*m);
        }
    }

void zLog::ok(const QString& m){
    log(m, OK);
    }

void zLog::ok(const char *m){
    ok(QString(m));
    }



/*trace*/
void zLog::dialogTrace(const QString& str) {
    dialog(str, TRACE);
}

void zLog::trace(const QString& msg){
    log(msg, TRACE);
}

void zLog::trace(const QList<QString>& ml){
    zforeach(m, ml){
        trace(*m);
        }
    }

void zLog::trace(const char *m){
    trace(QString(m));
    }

/*warning*/

void zLog::dialogWarning(const QString& str) {
    dialog(str, WARNING);
}

void zLog::warning(const QString& m){
    log(m, WARNING);
    }

void zLog::warning(const QList<QString>& ml){
    zforeach(m, ml){
        warning(*m);
        }
    }

void zLog::warning(const char *m){
    warning(QString(m));
    }


/*error*/

void zLog::dialogError(const QString& str) {
    dialog(str, ERROR);
}

void zLog::error(const QString& m){
    log(m, ERROR);
    }

void zLog::error(const QList<QString>& ml){
    zforeach(m, ml){
        error(*m);
        }
    }

void zLog::error(const char *m){
    error(QString(m));
    }
