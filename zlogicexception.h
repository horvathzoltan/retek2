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
    explicit zLogicException(const QString& _msg);

    void raise() const override;
    zLogicException *clone() const override;
};

#endif // ZLOGICEXCEPTION_H
