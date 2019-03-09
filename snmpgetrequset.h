#ifndef SNMPGETREQUSET_H
#define SNMPGETREQUSET_H
#include"qoid.h"
#include <QString>
#include "snmprequest.h"



class SNMPGetRequset:public SNMPRequest
{
public:
    SNMPGetRequset(QString type, QString community,QOID oid,QByteArray id);
    ~SNMPGetRequset();
    QString community;
    QString getCommunity();
    QOID oid;
    QOID getQoid();
private:
    SNMPGetRequset();
};

#endif // SNMPGETREQUSET_H
