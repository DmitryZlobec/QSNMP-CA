#ifndef SNMPREQUEST_H
#define SNMPREQUEST_H
#include<qstring.h>


class SNMPRequest
{
public:
    SNMPRequest(QString type_="None",QByteArray id="0");
    const QString type;
    QByteArray id;
    virtual ~SNMPRequest();

};

#endif // SNMPREQUEST_H
