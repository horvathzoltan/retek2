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
    this->widget->append(msg);
}

void zLog::log(QString msg){
    #ifdef QT_DEBUG
    this->widget->append(msg);
    #endif
}

void zLog::log(QList<QString>ml){
    zforeach(m, ml){
        if(m->endsWith("OK"))
            this->log(QString("<font color=green>%1</font>").arg(*m));
        else if(m->endsWith("ERROR"))
            this->log(QString("<font color=red>%1</font>").arg(*m));
        else
            this->log(QString("<font color=black>%1</font>").arg(*m));
    }
}
