#include "applicationclass.h"
#include "appconfig.h"
#include "snmpserver.h"
#include<QDebug>
#include<QSettings>

ApplicationClass::ApplicationClass(QObject *parent):QObject (parent)
{
    app = QCoreApplication::instance();

}

void ApplicationClass::run()
{
    qDebug()<< "SNMP Server Started";
    snmpServer = SNMPServer::instance(this);
    // quit();
}

void ApplicationClass::quit()
{
 //   emit finished();
}

void ApplicationClass::aboutToQuitApp()
{
    qDebug()<< "About to quit";
}

void ApplicationClass::readSNMP()
{


}
