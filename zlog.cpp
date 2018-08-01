#include "globals.h"
#include "zlog.h"
#include <QMessageBox>

zLog::zLog(){};

zLog::~zLog(){};

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
    case TRACE:
        this->widget->setTextColor(QColor(Qt::darkGray));
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
    case TRACE:
        h = QStringLiteral("Trace");
        break;
    default:
        h = QStringLiteral("Info");
        break;
    }

    QMessageBox messageBox;
    QMessageBox::critical(nullptr, h, str);
    messageBox.setFixedSize(500, 200);
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
