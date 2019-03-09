#include "snmpgetrequset.h"

SNMPGetRequset::SNMPGetRequset(QString type_,QString community,QOID oid,QByteArray id):SNMPRequest(type_,id),community(community),oid(oid)
{


}
QString SNMPGetRequset::getCommunity()
{
    return this->community;
}

QOID  SNMPGetRequset::getQoid()
{
    return this->oid;
}
SNMPGetRequset::~SNMPGetRequset()
{

}
