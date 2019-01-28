#ifndef ZMACRO_H
#define ZMACRO_H

#include <QRegularExpression>
#include "zlogicexception.h"

#define zforeach(var, container) for(auto (var) = (container).begin(); (var) != (container).end(); ++(var))
#define zforeach_from(var, container, ix) for(auto (var) = (container).begin()+(ix); (var) != (container).end(); ++(var))

#define nameof(x) z_macro_factory::_nameof<0>(#x, sizeof(x))
#define zfn() z_macro_factory::_zfn<0>((const char*)Q_FUNC_INFO)//Q_FUNC_INFO

namespace z_macro_factory {
    template<int a>
    QString _nameof(const char* y, std::size_t)
    {
        QString x(y);
        if(x.endsWith(QStringLiteral("()")))
        {
            QRegularExpression re(QStringLiteral(R"([\w]+(?:::|->|.)([\w]+)\(\))"));
            QRegularExpressionMatch m = re.match(x);

            if (m.hasMatch())
            {
                return m.captured(m.lastCapturedIndex());
            }
        }
        else
        {
            QRegularExpression re(QStringLiteral(R"(^&?([_a-zA-Z]\w*)\s*(->|\.|::)?\s*([_a-zA-Z]\w*)?$)"));
            QRegularExpressionMatch m = re.match(x);

            if (m.hasMatch()) {
                return m.captured(m.lastCapturedIndex());
            }
        }
        //QRegularExpression re("^&?([_a-zA-Z]\\w*(->|\\.|::))*([_a-zA-Z]\\w*)$");

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

#endif // ZMACRO_H
