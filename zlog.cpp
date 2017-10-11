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
