#include<QUdpSocket>
#include<QNetworkDatagram>
#include<QDataStream>
#include <QSharedPointer>

#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/json.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/exception/exception.hpp>

#include "snmpserver.h"
#include "snmpgetrequset.h"
#include "snmprequest.h"
#include"appconfig.h"
#include "applicationclass.h"
#include "qoid.h"

SNMPServer* SNMPServer::m_server = nullptr;

SNMPServer::SNMPServer(QObject *parent):QObject(parent)
{
    ApplicationClass *app = qobject_cast<ApplicationClass*>(parent);
    AppConfig *appConfig =  AppConfig::getConfig();
    qint16 SNMP_PORT =161;

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
                 const qint16 NULL_ANSWER_SIZE=2;
                 const char rawAnswer[] = {'\x05','\x00'};
                 QByteArray answer = QByteArray::fromRawData(rawAnswer,sizeof(rawAnswer));
                 int answer_size=NULL_ANSWER_SIZE;

                 qDebug() << "Receive GET requset";
                 SNMPGetRequset *getRequest = static_cast<SNMPGetRequset*>(request);
                 AppConfig *appConfig =  AppConfig::getConfig();

                 QString s_oid = getRequest->oid.getOID();

                 if(appConfig->params->contains(s_oid))
                 {
                     QSharedPointer<Param> p = appConfig->params->operator[](s_oid);
                     qDebug() << "Value:" << p->value;

                     if (p->type == "Integer")
                     {
                       answer.clear();
                       answer.append('\x02'); //Integer
                       answer.append(sizeof(qint32));
                       answer_size+=sizeof(qint32);
                       QByteArray answerData;
                       QDataStream answerDataStream(&answerData,QIODevice::ReadWrite);
                       answerDataStream << p->getParamInt();
                       //TODO: BUG Here
                       qDebug() << "answerData.data() " <<answerData.toHex();
                       answer.append(QByteArray::fromRawData(answerData.data(),sizeof(qint32)));
                       qDebug() << "answer " <<answer.toHex();
                     }
                        if(p->type == "String")
                        {
                            int stringSize = p->getParamString().toLatin1().length();
                            answer.clear();
                            answer.append('\x04'); //Octet String
                            answer.append(static_cast<char>(stringSize));
                            answer.append(p->getParamString().toLatin1());
                            answer_size+=stringSize;
                        }
                 }

                 if(appConfig->params->contains("mongo"))
                 {
                    mongocxx::instance inst{};
                    mongocxx::client conn{mongocxx::uri{}};

                    bsoncxx::builder::stream::document document{};

                    if(appConfig->params->contains("mongo/db") && appConfig->params->contains("mongo/collection"))
                    {
                        auto db_name =appConfig->params->value("mongo/db")->getParamString().toStdString();
                        auto collection_name =appConfig->params->value("mongo/collection")->getParamString().toStdString();

                        try {
                            auto collection = conn[db_name][collection_name];
                            auto session = conn.start_session();
                            document << "oid" << s_oid.toStdString() << bsoncxx::builder::stream::finalize;
                            bsoncxx::stdx::optional<bsoncxx::document::value> maybe_result = collection.find_one(session,document.view());
                            if(maybe_result) {
                              // Do something with *maybe_result;
                                auto v = (*maybe_result).view();
                                bsoncxx::document::element k_oid {v["oid"]};
                                if(!k_oid || k_oid.type() == bsoncxx::type::k_int32)
                                {
                                    int32_t oid_value = k_oid.get_int32();
                                    answer.clear();
                                    answer.append('\x02'); //Integer
                                    answer.append(sizeof(qint32));
                                    answer_size+=sizeof(qint32);
                                    QByteArray answerData;
                                    QDataStream answerDataStream(&answerData,QIODevice::ReadWrite);
                                    answerDataStream << oid_value;
                                    answer.append(QByteArray::fromRawData(answerData.data(),sizeof(qint32)));
                                }
                                else if (!k_oid || k_oid.type() == bsoncxx::type::k_utf8)
                                {
                                    bsoncxx::stdx::string_view view = k_oid.get_utf8().value;
                                    std::string oid_value = view.to_string();
                                    int32_t stringSize = oid_value.length();
                                    answer.clear();
                                    answer.append('\x04'); //Octet String
                                    answer.append(static_cast<char>(stringSize));
                                    answer.append(oid_value.data());
                                    answer_size+=stringSize;
                                }

                            }
                            else {
                                qDebug() << "Couldn't find an object";
                            }

                        } catch (mongocxx::exception e) {
                            qDebug() << "Retrieval from MongoDB failed (and exception thrown)";
                        }
                    }
                    else {
                        qDebug() << "no DB and Collection params";
                    }
                 }

                 QByteArray sendData = QByteArray(1,'\x30');

                 //Append lenght of packet
                 sendData.append(static_cast<char>('\xff'));

                 //Append version
                 const char version[] = {0x02,0x01,0x00};
                 sendData.append(version,3);

                 //Append community
                 sendData.append(static_cast<char>(0x04));
                 sendData.append(static_cast<char>(getRequest->getCommunity().length()));
                 sendData.append(getRequest->getCommunity().toLocal8Bit());

                 //Append var
                 QByteArray varBindList(getRequest->oid.b_oid->data());
                 // Object ID
                 varBindList.insert(0,static_cast<char>(0x06));
                 varBindList.insert(1,static_cast<char>(getRequest->oid.b_oid->length()));

                 varBindList.insert(0,0x30);
                 varBindList.insert(1,static_cast<char>(varBindList.length()+answer_size-1));
                 varBindList.insert(0,0x30);
                 varBindList.insert(1,static_cast<char>(varBindList.length()+answer_size-1));

                 qDebug() << "varBindList.append(answer.data(),answer_size)" << answer.toHex();
                  varBindList.append(QByteArray::fromRawData(answer,answer_size));

                 QByteArray b;
                 QDataStream b_str(&b,QIODevice::WriteOnly | QIODevice::Append);
                 b_str.setByteOrder(QDataStream::BigEndian);
                 b_str << qint16(0x0000);


                 //if(answer_size>NULL_ANSWER_SIZE)
                 //  varBindList.append(b,answer_size-NULL_ANSWER_SIZE);


                 qDebug() << getRequest->oid.b_oid->toHex();
                 qDebug() <<"varBind:" <<varBindList.toHex();

                 //SNMP RESPONSE:
                 sendData.append(static_cast<char>(0xa2));
                 sendData.append(static_cast<char>(varBindList.length()+8+getRequest->id.length()));
                 sendData.append(0x02);
                 sendData.append(static_cast<char>(getRequest->id.length()));
                 sendData.append(getRequest->id.data());

                 const char error_status[] = {0x02, 0x01, 0x00};
                 sendData.append(error_status,3);
                 const char error_index[] = {0x02, 0x01, 0x00};
                 sendData.append(error_index,3);
                 sendData.append(varBindList);

                 sendData[1]=static_cast<char>(sendData.length()-2);

                 qDebug() << "Send Data:"<<sendData.toHex();
                 udpSocket->writeDatagram(sendData,datagram.senderAddress(),datagram.senderPort());
              return;
              }

          //    QByteArray sendData = datagram.data();
          //    sendData[7+1] = static_cast<char>(0xa2);
          //    udpSocket->writeDatagram(sendData,datagram.senderAddress(),datagram.senderPort());
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
        QDataStream oidLengthStream(packetArray.mid(7+comLen+15+lenght,1));
        qDebug() << "packetArray.mid(7+comLen+17,1)" << packetArray.mid(7+comLen+15+lenght,1).toHex();
        oidLengthStream >> oidLength;
        qDebug() << "oidLength" <<oidLength;
        QByteArray b_oid(packetArray.mid(7+comLen+15+lenght+1,oidLength));
        qDebug()<< "Hex OID" << b_oid.toHex();
        QOID qoid(b_oid);
        QString s_oid = qoid.getOID();

        qDebug()<< "String OID" << qoid.getOID();

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
