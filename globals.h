#ifndef GLOBALS_H
#define GLOBALS_H


#include "Beallitasok.h"
#include "zlog.h"
#include "zsql.h"

#define zforeach(var, container) for(auto var = (container).begin(); var != (container).end(); ++var)
#define zforeach_from(var, container, ix) for(auto var = (container).begin()+ix; var != (container).end(); ++var)

extern Beallitasok beallitasok;
extern zLog zlog;
extern zSQL zsql;
extern QMap<QString, QString> globalCaptionMap;
extern QMap<QString, QVariant> typeMap;
extern QMap<QString, QString> macroMap;
#endif // GLOBALS_H
