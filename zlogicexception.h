#ifndef ZLOGICEXCEPTION_H
#define ZLOGICEXCEPTION_H

#include <QString>
#include <QException>
#include <QDebug>

class zLogicException: public QException
{
private:
       const QString& msg;
public:
    explicit zLogicException(const QString& _msg);//:msg(_msg);
       /*
       {
            qDebug() << this->msg;
       }*/

    void raise() const override;// { throw *this; }
    zLogicException *clone() const override;// { return new zLogicException(*this); }

};

#endif // ZLOGICEXCEPTION_H
