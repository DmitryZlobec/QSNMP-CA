#include "snmprequest.h"
#include <qstring.h>


SNMPRequest::SNMPRequest(QString type_,QByteArray id_):type(type_),id(id_.data())
{

}

SNMPRequest::~SNMPRequest(){}
