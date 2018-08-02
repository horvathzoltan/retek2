#include "globals.h"
#include "zcaptionmap.h"

Beallitasok beallitasok;
zLog zlog;
//zSQL zsql;
zTokenizer ztokenizer;

QList<zConversionMap> globalCaptionMaps;

QMap<QString, QString> typeMap;
QMap<QString, QString> typeMapR;

QMap<QString, QString> macroMap;
QList<zTable> ztables;

//QString tablanev = "";

zTable *table = nullptr;

//extern const int C_ix_Caption = 0;
//extern const int C_ix_colName = 1;
//extern const int C_ix_colType = 2;
//extern const int C_ix_dlen = 3;
//extern const int C_ix_nullable = 4;
