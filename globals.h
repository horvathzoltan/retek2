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

#define zforeach(var, container) for(auto (var) = (container).begin(); (var) != (container).end(); ++(var))
#define zforeach_from(var, container, ix) for(auto (var) = (container).begin()+(ix); (var) != (container).end(); ++(var))

//QStringLiteral(Q_FUNC_INFO)

extern Beallitasok beallitasok;
extern zLog zlog;
//extern zSQL zsql;
extern zTokenizer ztokenizer;
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

#define nameof(x) z_macro_factory::_nameof<0>(#x, sizeof(x))
#define zfn() z_macro_factory::_zfn<0>((const char*)Q_FUNC_INFO)//Q_FUNC_INFO

class zLogicException: public QException
{
private:
    //Q_DISABLE_COPY(zLogicException)
       const QString& msg;

public:
    explicit zLogicException(const QString& _msg)
           :msg(_msg)
       {           
            qDebug() << this->msg;
       }

    void raise() const override { throw *this; }
    zLogicException *clone() const override { return new zLogicException(*this); }

};

namespace z_macro_factory {
    template<int a>
    QString _nameof(const char* y, std::size_t)
    {
        QString x(y);
        //QRegularExpression re("^&?([_a-zA-Z]\\w*(->|\\.|::))*([_a-zA-Z]\\w*)$");
        QRegularExpression re(QStringLiteral(R"(^&?([_a-zA-Z]\w*)\s*(->|\.|::)?\s*([_a-zA-Z]\w*)?$)"));
        QRegularExpressionMatch m = re.match(x);

        if (m.hasMatch()) {
            return m.captured(m.lastCapturedIndex());
        }
        throw zLogicException("A bad expression x in nameof(x). The expression is \"" + x + "\".");
    }

    template<int a> QString _zfn(const char* y)
    {
        QString x(y);

        QRegularExpression re(QStringLiteral(R"(([\w]+::[\w]+))"));
        QRegularExpressionMatch m = re.match(x);

        if (m.hasMatch())
        {
            return m.captured(m.lastCapturedIndex());
        }

        throw zLogicException("A bad expression x in _zfn(x). The expression is \"" + x + "\".");
        return QStringLiteral("unknown");
    }
}// namespace z_macro_factory





//namespace z_macro_factory
//{
//    template<int a> QString _nameof(const char* y, std::size_t);
//    QString _zfn(const char* y);
//}



#endif // GLOBALS_H
