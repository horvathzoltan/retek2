#ifndef GLOBALS_H
#define GLOBALS_H

#include "Beallitasok.h"
#include "zlog.h"
#include "zsql.h"
#include "ztable.h"
#include "ztokenizer.h"
#include <QException>
#include <QDebug>

#define zforeach(var, container) for(auto var = (container).begin(); var != (container).end(); ++var)
#define zforeach_from(var, container, ix) for(auto var = (container).begin()+ix; var != (container).end(); ++var)

extern Beallitasok beallitasok;
extern zLog zlog;
//extern zSQL zsql;
extern zTokenizer ztokenizer;
extern QMap<QString, QString> globalCaptionMap;
//extern QMap<QString, QVariant> typeMap;
//extern QMap<QString, QVariant> typeMapR;
//valami 6

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

#define nameof(x) bravikov::_nameof<0>(#x, sizeof(x))

class zLogicException: public QException
{
public:
    zLogicException(QString const& _msg){ qDebug() << _msg; this->msg = _msg;}

    void raise() const { throw *this; }
    zLogicException *clone() const { return new zLogicException(*this); }
private:
    QString msg;
};

namespace bravikov {
    template<int a>
    QString _nameof(const QString x, std::size_t)
    {
        //QRegularExpression re("^&?([_a-zA-Z]\\w*(->|\\.|::))*([_a-zA-Z]\\w*)$");
        QRegularExpression re(R"(^&?([_a-zA-Z]\w*)\s*(->|\.|::)?\s*([_a-zA-Z]\w*)?$)");
        QRegularExpressionMatch m = re.match(x);

        if (m.hasMatch()) {
            return m.captured(m.lastCapturedIndex());
        }
        throw zLogicException("A bad expression x in nameof(x). The expression is \"" + x + "\".");
    }
}
