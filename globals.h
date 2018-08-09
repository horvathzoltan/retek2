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
#define zfunc QStringLiteral(__FUNCTION__)

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

extern QMap<QString, QString> typeMap;
extern QMap<QString, QString> typeMapR;

extern QMap<QString, QString> macroMap;
extern QList<zTable> ztables;

//extern QString tablanev;

extern zTable *table;

static constexpr int C_ix_Caption = 0;
static constexpr int C_ix_colName=1;
static constexpr int C_ix_colType=2;
static constexpr int C_ix_dlen=3;
static constexpr int C_ix_nullable=4;

#endif // GLOBALS_H

#define nameof(x) bravikov::_nameof<0>(#x, sizeof(x))

class zLogicException: public QException
{
public:
    explicit zLogicException(QString const& _msg){ qDebug() << _msg; this->msg = _msg;}

    void raise() const override { throw *this; }
    zLogicException *clone() const override { return new zLogicException(*this); }
private:
    QString msg;
};

namespace bravikov {
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
}  // namespace bravikov
