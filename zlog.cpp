#include "globals.h"
#include "zlog.h"
#include <QMessageBox>

zLog::zLog(){};

zLog::~zLog(){};

void zLog::init(QTextBrowser* b, QTabWidget* tw, int tix){
    this->widget = b;
    this->tabwidget = tw;
    this->tabindex = tix;
}

void zLog::trace(const QString& msg){
//    auto c = this->widget->textColor();
//    this->widget->setTextColor(QColor(Qt::gray));
//    this->widget->append(msg);
//    this->widget->setTextColor(c);
    log(msg, TRACE);
}


void zLog::log(const QString& m){
    #ifdef QT_DEBUG

    if(m.endsWith("OK")){
        log(m.left(m.length()-2), OK);
    }
    else if(m.endsWith("ERROR")){
        log(m.left(m.length()-5), ERROR);    
    }
    else if(m.endsWith("TRACE")){
        log(m.left(m.length()-5), TRACE);
    }
    else{
        log(m, -1);
    }
    #endif
}

void zLog::log(const QString& m, int errlevel){
    #ifdef QT_DEBUG

    auto c = this->widget->textColor();

    switch(errlevel){
    case OK:
        this->widget->setTextColor(QColor(Qt::green));
        break;
    case ERROR:
        this->widget->setTextColor(QColor(Qt::red));        
        tabwidget->setCurrentIndex(tabindex);
        break;
    case TRACE:
        this->widget->setTextColor(QColor(Qt::gray));
        break;
    default:
        this->widget->setTextColor(QColor(Qt::black));
        break;
    }

    this->widget->append(m);
    this->widget->setTextColor(c);
    #endif
}

void zLog::log(const QList<QString>& ml){
    zforeach(m, ml){
        this->log(*m);
        }
}

/**/

void zLog::errorDialog(const QString& str) {
    QMessageBox messageBox;
    QMessageBox::critical(nullptr, QStringLiteral("Error"), str);
    messageBox.setFixedSize(500, 200);
}


