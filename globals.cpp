#include "globals.h"

Beallitasok beallitasok;
zLog zlog;
zSQL zsql;
zTokenizer ztokenizer;

QMap<QString, QString> globalCaptionMap;
//QMap<QString, QVariant> typeMap;

QMap<QString, QString> typeMap;
QMap<QString, QString> typeMapR;

QMap<QString, QString> macroMap;
QList<zTable> ztables;

QStringList pks;

QString tablanev = "";

const int C_ix_Caption = 0;
const int C_ix_colName = 1;
const int C_ix_colType = 2;
const int C_ix_dlen = 3;
const int C_ix_nullable = 4;
