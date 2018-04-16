#include "globals.h"
#include "zlog.h"
#include <QMessageBox>

zLog::zLog(){};

zLog::~zLog(){};

void zLog::init(QTextBrowser* b){
    this->widget = b;
}

void zLog::ShowDialog(QString str) {
    QMessageBox messageBox;
    messageBox.critical(0, "Error", str);
    messageBox.setFixedSize(500, 200);
}

void zLog::trace(QString msg){
//    auto c = this->widget->textColor();
//    this->widget->setTextColor(QColor(Qt::gray));
//    this->widget->append(msg);
//    this->widget->setTextColor(c);
    log(msg, TRACE);
}

void zLog::log(QString m){
    #ifdef QT_DEBUG

    if(m.endsWith("OK"))
        log(m.left(m.length()-2), OK);
    else if(m.endsWith("ERROR"))
        log(m.left(m.length()-5), ERROR);    
    else if(m.endsWith("TRACE"))
        log(m.left(m.length()-5), TRACE);
    else
        log(m, -1);
    #endif
}

void zLog::log(QString m, int errlevel){
    #ifdef QT_DEBUG

    auto c = this->widget->textColor();

    switch(errlevel){
    case OK:
        this->widget->setTextColor(QColor(Qt::green));
        break;
    case ERROR:
        this->widget->setTextColor(QColor(Qt::red));
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

void zLog::log(QList<QString>ml){
    zforeach(m, ml){
        this->log(*m);
        }
}
