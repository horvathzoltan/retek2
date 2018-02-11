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
    QString fn = zFileNameHelper::append(QDir::homePath(), munkadir, adatbazisNev, "caption_"+tablanev+".txt");
    return fn;
}

QString Beallitasok::getModelFilename(QString tfname, QString dirname) {
    //auto e = QString(munkadir+R"(\%2\%1)").arg(dirname).arg(adatbazisNev);
    QString  e = zFileNameHelper::append(QDir::homePath(),munkadir, dirname, adatbazisNev);
    QDir d(e);if(!d.exists()){d.mkpath(d.absolutePath());}

    e += QDir::separator()+tfname;
    zlog.trace(e);
    return e;
}

/*
 * a beállítások alapján a template névhez tartozó fájl nevét adja
*/
QString Beallitasok::getTemplateFilename(QString tfname) {
    bool isVal = true;
    if(beallitasok.tmpDir.isEmpty())
        {zlog.log("A template könyvtár a beállításokban nincs megadva");isVal=false;}
    if(tfname.isEmpty())
        {zlog.log("A template fájlnév nincs megadva");isVal=false;}
    if(beallitasok.adatbazisNev.isEmpty())
        {zlog.log("Az adatbázisnév nincs megadva");isVal=false;}
    if(isVal == false)
        {zLog::ShowDialog("A template fájlnév nem meghatározható");return NULL;}

    auto fn = zFileNameHelper::append(QDir::homePath(),beallitasok.tmpDir, beallitasok.adatbazisNev, tfname);


    if(QFileInfo(fn).exists())
        return fn;
    else{
        zlog.log("nincs project templatefile:"+ fn);
        fn = QString(beallitasok.tmpDir+R"(\%1)").arg(tfname);
        zlog.log("nincs default template:" +fn);
        if(QFileInfo(fn).exists())
            return fn;
        else{
            zlog.log("nincs default templatefile:"+ fn);
            }
        }


    return NULL;
}

