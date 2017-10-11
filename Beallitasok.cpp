#include "Beallitasok.h"

Beallitasok::Beallitasok(){};

Beallitasok::~Beallitasok(){};

void Beallitasok::init(QLineEdit* wu, QLineEdit* wp, QLineEdit* wserver, QLineEdit* wcatalog)
{
    this->widget_user = wu;
    this->widget_password = wp;
    this->widget_server = wserver;
    this->widget_adatbazisNev = wcatalog;
}


QString Beallitasok::getConnStr(){
    return  this->connectionTemplate.arg(this->server).arg(this->adatbazisNev);
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
