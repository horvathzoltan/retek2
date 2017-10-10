#include "Beallitasok.h"


Beallitasok::Beallitasok()
{
}


Beallitasok::~Beallitasok()
{
}


QString Beallitasok::getConnStr(){
    return  this->connectionTemplate.arg(this->server).arg(this->adatbazisNev);
}
