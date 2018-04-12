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
    auto c = this->widget->textColor();
    this->widget->setTextColor(QColor(Qt::gray));
    this->widget->append(msg);
    this->widget->setTextColor(c);
}

void zLog::log(QString m){
    #ifdef QT_DEBUG

    auto c = this->widget->textColor();

    QColor c2;

    if(m.endsWith("OK"))
        c2 = QColor(Qt::green);
    else if(m.endsWith("ERROR"))
        c2 = QColor(Qt::red);
    else
        c2 = QColor(Qt::black);

    this->widget->append(m);
    this->widget->setTextColor(c);
    #endif
}

void zLog::log(QList<QString>ml){
    zforeach(m, ml){
        this->log(*m);
        }
}
