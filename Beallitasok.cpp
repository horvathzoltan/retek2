#include "Beallitasok.h"
#include "zfilenamehelper.h"
#include "globals.h"
#include <QDir>

Beallitasok::Beallitasok(){};

Beallitasok::~Beallitasok(){};

void Beallitasok::init(QLineEdit* wu, QLineEdit* wp, QLineEdit* wserver, QLineEdit* wcatalog)
{
    this->widget_user = wu;
    this->widget_password = wp;
    this->widget_server = wserver;
    this->widget_adatbazisNev = wcatalog;
}

void Beallitasok::getUI()
{
    user = widget_user->text();
    password = widget_password->text();
    server = widget_server->text();
    adatbazisNev = widget_adatbazisNev->text();
}

void Beallitasok::setUI()
{
    widget_user->setText(this->user);
    widget_password->setText(this->password);
    widget_server->setText(this->server);
    widget_adatbazisNev->setText(this->adatbazisNev);
}


QString Beallitasok::getCaptionFileName(QString tablanev){
    QString fn = zFileNameHelper::getCClassFilename(munkadir, adatbazisNev, "caption_"+tablanev+".txt");
    return fn;
}

QString Beallitasok::getModelFilename(QString tfname, QString dirname) {
    auto e = QString(munkadir+R"(\%2\%1)").arg(dirname).arg(adatbazisNev);
    QDir d(e);if(!d.exists()){d.mkpath(d.absolutePath());}

    e += "\\"+tfname;
    zlog.trace(e);
    return e;
}

QString Beallitasok::getTemplateFilename(QString tfname) {
    auto fn = QString(beallitasok.tmpDir+R"(\%1\%2)").arg(beallitasok.adatbazisNev).arg(tfname);
    if(QFileInfo(fn).exists())
        return fn;
    else{
        zlog.log("nincs project templatefile:"+ fn);
        fn = QString(beallitasok.tmpDir+R"(\%1)").arg(tfname);
        zlog.log("nincs defaule template:" +fn);
        if(QFileInfo(fn).exists())
            return fn;
        else{
            zlog.log("nincs default templatefile:"+ fn);
            }
        }
    return NULL;
}

