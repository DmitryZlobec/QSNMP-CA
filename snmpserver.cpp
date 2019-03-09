#include<QUdpSocket>
#include<QNetworkDatagram>
#include<QDataStream>
#include "snmpserver.h"
#include "snmpgetrequset.h"
#include "snmprequest.h"
#include "qoid.h"
SNMPServer* SNMPServer::m_server = NULL;

SNMPServer::SNMPServer(QObject *parent):QObject(parent)
{
    enum {SNMP_PORT=161};
    udpSocket = new QUdpSocket(this);
    udpSocket->bind(QHostAddress::LocalHost,SNMP_PORT);
    connect(udpSocket,SIGNAL(readyRead()),this,SLOT(readSNMP()));
}
SNMPServer* SNMPServer::instance(QObject* parent)
{
    if(!SNMPServer::m_server)
    {
        SNMPServer::m_server = new SNMPServer(parent);

    }
    return m_server;
}

void SNMPServer::readSNMP(){

          while (udpSocket->hasPendingDatagrams()) {
              QNetworkDatagram datagram = udpSocket->receiveDatagram();
              SNMPRequest* request = processDatagramm(datagram);

              if(QString::compare(request->type, "GET", Qt::CaseInsensitive) == 0)
              {
                 qDebug() << "eceive GET requset";
                 SNMPGetRequset *getRequest = static_cast<SNMPGetRequset*>(request);
                 QByteArray sendData = QByteArray(1,0x30);
                 sendData.resize(sendData.size()+5);
                 int answer_size=4;
                 int len= getRequest->getCommunity().length();
                 sendData[1]=35+len+ answer_size;
                 sendData[2]=0x02;
                 sendData[3]=0x01;
                 sendData[4]=0x00;
                 sendData[5]=0x04;
                 sendData.append(getRequest->getCommunity().length());
                 sendData.append(getRequest->getCommunity().toLocal8Bit());

                 QByteArray varBindList(getRequest->oid.b_oid->data());
                 varBindList.insert(0,0x30);
                 varBindList.insert(1,varBindList.length()+1+answer_size);
                 varBindList.insert(0,0x30);
                 varBindList.insert(1,varBindList.length()+1+answer_size);

                 const char answer[] = {0x02,0x04};
                 QByteArray b;
                 QDataStream b_str(&b,QIODevice::WriteOnly | QIODevice::Append);
                 b_str.setByteOrder(QDataStream::BigEndian);
                 b_str << qint32(65535);
                 varBindList.append(answer,2);
                 varBindList.append(b,answer_size);
                 qDebug() << getRequest->oid.b_oid->toHex();

                 qDebug() <<"varBind:" <<varBindList.toHex();

                 sendData.append(0xa2);
                 sendData.append(varBindList.length()+8+getRequest->id.length());
                 sendData.append(0x02);
                 sendData.append(getRequest->id.length());
                 sendData.append(getRequest->id.data());
                 const char arr1[] = {0x02, 0x01, 0x00};
                 sendData.append(arr1,3);
                 sendData.append(arr1,3);
                 sendData.append(varBindList);
                 //QByteArray responseData(30,0x00);
                 qDebug() << "Send Data:"<<sendData.toHex();
                 udpSocket->writeDatagram(sendData,datagram.senderAddress(),datagram.senderPort());
              return;
              }

           QByteArray sendData = datagram.data();

              sendData[7+1] = static_cast<char>(0xa2);
              udpSocket->writeDatagram(sendData,datagram.senderAddress(),datagram.senderPort());
              delete request;
          }
}

SNMPRequest* SNMPServer::processDatagramm(QNetworkDatagram &datagram)
{
    enum requests_types{GET= 0xa0,RESP = 0xa2};
    QByteArray packetArray(datagram.data());
    QDataStream packetOut(&packetArray,QIODevice::ReadOnly);
    qint8 asn1;
    qint8 len,comLen;
    qint8 v1,v2,v3;
    qint8 version;
    QByteArray communityArray;
    qDebug() << "Packet:" <<packetArray.toHex();
    packetOut >>asn1>>len>>v1>>v2>>version>>v3>>comLen;
    communityArray.resize(comLen);
    QString community  = QString::fromLatin1(packetArray.mid(7,comLen));
    qint8 requestType;

    QDataStream requestTypeStream(packetArray.mid(7+comLen,1));
    requestTypeStream>> requestType;
    qint32 lenght = qint32(packetArray.mid(7+comLen+3,1).at(0));
    QByteArray id_ = packetArray.mid(7+comLen+4,lenght);
    qDebug() << "Id:" << id_.toHex();

    if(requestType == static_cast<qint8>(GET))
    {
        qint8 requestTypeLength;
        QDataStream requestTypeLengthStream(packetArray.mid(7+comLen+1,1));
        requestTypeLengthStream >> requestTypeLength;
        qint8 oidLength;
        QDataStream oidLengthStream(packetArray.mid(7+comLen+17,1));
        oidLengthStream >> oidLength;
        QByteArray b_oid(packetArray.mid(7+comLen+16,oidLength+2));
        QOID qoid(b_oid);
        SNMPGetRequset *snmpGetRequset = new SNMPGetRequset("GET",community, qoid,id_);
        return snmpGetRequset;
    }
    return new SNMPRequest();
}

SNMPServer::~SNMPServer()
{
    if(m_server)
    {
        udpSocket->close();
        delete udpSocket;
        delete m_server;
    }

}