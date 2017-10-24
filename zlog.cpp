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
    zforeach(m, ml){this->log(*m);}
}
