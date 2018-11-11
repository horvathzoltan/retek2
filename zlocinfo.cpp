#include "zlocinfo.h"

zLocInfo::zLocInfo(const char *func, const char *file, int line)
{
    this->func=QString(func);
    this->file =QString(file);
    this->line=line;
}

QString zLocInfo::toString() const
{
    auto e = QStringLiteral("%2:%3 %1").arg(func,file).arg(line);
    return e;
}
