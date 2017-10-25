#ifndef GLOBALS_H
#define GLOBALS_H


#include "Beallitasok.h"
#include "zlog.h"
#include "zsql.h"

#define zforeach(var, container) for(auto var = (container).begin(); var != (container).end(); ++var)

extern Beallitasok beallitasok;
extern zLog zlog;
extern zSQL zsql;
extern QMap<QString, QString> globalCaptionMap;

#endif // GLOBALS_H
