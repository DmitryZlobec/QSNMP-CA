#include "appconfig.h"
#include<QDebug>
AppConfig* AppConfig::m_appConfig = nullptr;

AppConfig::AppConfig()
{
        qint16 t_SNMP_PORT= settings->value("system/port").toInt();
        if(t_SNMP_PORT >0)
        {
            qDebug() << "SNMP PORT" << t_SNMP_PORT;
            SNMP_PORT = t_SNMP_PORT;
        }

       settings.reset(new QSettings("qsnmp.ini",QSettings::IniFormat));

    Q_FOREACH (QString group, settings->childGroups()) {
        if (group.startsWith("system"))
            continue;

        settings->beginGroup(group);
        qDebug() << group.toUpper();
        qDebug()<< "class" << settings->value("class").toString();
        qDebug()<< "value" << settings->value("value").toString();
        qDebug()<< "type" << settings->value("type").toString();
        qDebug() << "----";

     settings->endGroup();
    }

}

AppConfig* AppConfig::getConfig()
{
    if(!m_appConfig)
    {
        m_appConfig = new AppConfig();

    }
return m_appConfig;
}
