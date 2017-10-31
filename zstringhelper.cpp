#include "zstringhelper.h"

/*
zStringHelper::zStringHelper()
{

}
*/

bool zStringHelper::toBool(QString ezt){
    if(!ezt.compare("yes", Qt::CaseInsensitive)) return true;
    if(!ezt.compare("true", Qt::CaseInsensitive)) return true;
    if(!ezt.compare("ok", Qt::CaseInsensitive)) return true;
    return false;
}
