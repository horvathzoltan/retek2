#ifndef GLOBALS_H
#define GLOBALS_H


#include "Beallitasok.h"
#include "zlog.h"
#include "zsql.h"
#include "ztable.h"
#include "ztokenizer.h"

#define zforeach(var, container) for(auto var = (container).begin(); var != (container).end(); ++var)
#define zforeach_from(var, container, ix) for(auto var = (container).begin()+ix; var != (container).end(); ++var)

extern Beallitasok beallitasok;
extern zLog zlog;
extern zSQL zsql;
extern zTokenizer ztokenizer;
extern QMap<QString, QString> globalCaptionMap;
//extern QMap<QString, QVariant> typeMap;
//extern QMap<QString, QVariant> typeMapR;

extern QMap<QString, QString> typeMap;
extern QMap<QString, QString> typeMapR;

extern QMap<QString, QString> macroMap;
extern QList<zTable> ztables;

//extern QString tablanev;

extern zTable *table;

extern const int C_ix_Caption;
extern const int C_ix_colName;
extern const int C_ix_colType;
extern const int C_ix_dlen;
extern const int C_ix_nullable;
#endif // GLOBALS_H
