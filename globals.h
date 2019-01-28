#ifndef GLOBALS_H
#define GLOBALS_H

#include "Beallitasok.h"
#include "zlog.h"
#include "zsql.h"
#include "ztable.h"
#include "ztokenizer.h"
#include <QDebug>
#include <QException>
#include "zconversionmap.h"
#include "zdownloader.h"
#include "zlogicexception.h"


//QStringLiteral(Q_FUNC_INFO)

extern Beallitasok beallitasok;
extern zLog zlog;
//extern zSQL zsql;
extern zTokenizer ztokenizer;
extern zDownloader downloader;

extern QList<zConversionMap> globalCaptionMaps;
extern QList<zConversionMap> projectCaptionMaps;

// azok a belső osztályok vannak, amik ezekben kulcsként szerepelnek:
// belső -> sql
extern QList<zConversionMap> globalSqlMaps;
// belső->osztály
extern QList<zConversionMap> globalClassMaps;

//valami 6
// elavultak
//extern QMap<QString, QString> typeMap;
//extern QMap<QString, QString> typeMapR;

extern QMap<QString, QString> macroMap;
extern QList<zTable> ztables;

//extern QString tablanev;

extern zTable *table;

static constexpr int C_ix_Caption = 0;
static constexpr int C_ix_colName=1;
static constexpr int C_ix_colType=2;
static constexpr int C_ix_dlen=3;
static constexpr int C_ix_nullable=4;

//namespace z_macro_factory
//{
//    template<int a> QString _nameof(const char* y, std::size_t);
//    QString _zfn(const char* y);
//}

//#define BREAKPOINT __asm ({ int 3; })

#endif // GLOBALS_H
