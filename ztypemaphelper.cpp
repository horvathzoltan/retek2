#include "globals.h"
#include "ztypemaphelper.h"

/*zTypemapHelper::zTypemapHelper()
{

}*/


/*bool zTypemapHelper::containsKey(QMap<QString, QVariant> *map, QString k){
    auto ks = map->keys();
    zforeach(e, ks){
       if(e->toLower()==k.toLower()) return true;
    }
    return false;
}

bool zTypemapHelper::contains(QMap<QString, QVariant> *map, QString v){
    auto vs = map->values();
    zforeach(e, vs){
       auto ee = e->toString();
       if(ee.toLower()==v.toLower()) return true;
    }
    return false;
}

QString zTypemapHelper::getValue(QMap<QString, QVariant> *map, QString v){
    auto vs = map->values();
    zforeach(e, vs){
       if(e->toString().toLower()==v.toLower()) return e->toString();
    }
    return "";
}*/

QString zTypemapHelper::getKey(QMap<QString, QVariant> *map, QString k){
    //auto ks = map->keys();
    QString kl = k.toLower();
    zforeach(e, map->keys()){
       if(e->toLower()==kl || map->value(*e).toString().toLower()==kl) return *e;
    }

    return "";
}
