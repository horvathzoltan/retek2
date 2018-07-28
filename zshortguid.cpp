#include "zshortguid.h"


const zShortGuid zShortGuid::Empty = zShortGuid(QUuid());

/*
http://www.singular.co.nz/2007/12/shortguid-a-shorter-and-url-friendly-guid-in-c-sharp/
 */

zShortGuid::zShortGuid(QUuid guid){
 value = Encode(guid);
 uuid = guid;
}

zShortGuid zShortGuid::createNew(){
    auto g = QUuid::createUuid();
    return zShortGuid(g);
}

QString zShortGuid::Encode(QUuid g){
    auto b = g.toRfc4122().toBase64(QByteArray::Base64UrlEncoding);
    QString e(b);
    e = e.replace('/', '_').replace('+', '-').left(22);// - "=="
    return e;
}

QUuid zShortGuid::Decode(QString s){
      s = s.replace('_', '/').replace('-', '+') + "==";
      auto o = QByteArray::fromBase64(s.toLocal8Bit());
      QUuid a = QUuid::fromRfc4122(o);
      return a;
    }


