#ifndef SNMPSERVER_H
#define SNMPSERVER_H
#include<QObject>
#include<QUdpSocket>
#include<QSettings>
#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>

#include"snmprequest.h"
#include"snmpgetrequset.h"
class SNMPServer:public QObject
{
  Q_OBJECT
  QUdpSocket* udpSocket;
  SNMPRequest* processDatagramm(QNetworkDatagram &datagram);
  static  SNMPServer *m_server;
  SNMPServer(QObject* parent=0);
  ~SNMPServer();
  QSharedPointer<QSettings> settings;
  mongocxx::instance inst{};
  mongocxx::client conn{mongocxx::uri{}};
public:
    static   SNMPServer* instance(QObject* parent);

public slots:
    void readSNMP();
};

#endif // SNMPSERVER_H
