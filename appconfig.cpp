#include "appconfig.h"
#include "param.h"
#include <QSharedPointer>
#include<QDebug>
AppConfig* AppConfig::m_appConfig = nullptr;

AppConfig::AppConfig()
{
        settings.reset(new QSettings("qsnmp.ini",QSettings::IniFormat));
        params = new QList<QSharedPointer<Param>>();
        qint16 t_SNMP_PORT= settings->value("system/port").toInt();
        if(t_SNMP_PORT >0)
        {
            qDebug() << "SNMP PORT" << t_SNMP_PORT;
            SNMP_PORT = t_SNMP_PORT;
        }
    Q_FOREACH (QString group, settings->childGroups()) {
        if (group.startsWith("system"))
            continue;

        settings->beginGroup(group);
        QString _oid = group.toUpper();
        QString _class_type = settings->value("class").toString();
        QString _value = settings->value("value").toString();
        QString _type = settings->value("type").toString();
        QSharedPointer<Param> p = QSharedPointer<Param>(new Param(_oid, _class_type,_type,_value));
        params->append(p);

        qDebug() << "----";

     settings->endGroup();
    }

}

AppConfig* AppConfig::getConfig()
{
    if(m_appConfig == nullptr)
    {
        m_appConfig = new AppConfig();

    }
return m_appConfig;
}


AppConfig::~AppConfig()
{
    params->clear();
    delete params;
}
