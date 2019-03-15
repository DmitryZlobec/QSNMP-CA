#include <QCoreApplication>
#include<QObject>
#include<QTimer>
#include<QCommandLineParser>
#include <QFileInfo>
#include <QtGlobal>
#include <QTextStream>
#include <QDateTime>
#include"applicationclass.h"
#include"snmpserver.h"

QString configFileName = "qsnmp.ini";
QString logFileName = "qsnmp.log";

void snmpDebugMessageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    QByteArray localMsg = msg.toLocal8Bit();
    const char *file = context.file ? context.file : "";
    const char *function = context.function ? context.function : "";
    QString message=QString("%2 in %3 file %4").arg(localMsg.data()).arg(function).arg(file);
    QString txt;
    switch(type){
        case QtDebugMsg:
                txt = QString("%1 Debug: %2 ").arg(QDateTime::currentDateTime().toString()).arg(message);
                break;
        case QtWarningMsg:
                txt = QString("%1 Warning: %2 ").arg(QDateTime::currentDateTime().toString()).arg(message);
                break;
        case QtCriticalMsg:
                txt = QString("%1 Critical: %2 ").arg(QDateTime::currentDateTime().toString()).arg(message);
                break;
        case QtFatalMsg:
                txt = QString("%1 Fatal: %2 ").arg(QDateTime::currentDateTime().toString()).arg(message);
                abort();
        default:
                txt = QString("%1 Other: %2").arg(QDateTime::currentDateTime().toString()).arg(message);
    }
    QFile logFile(logFileName);
    logFile.open(QIODevice::WriteOnly| QIODevice::Append);
    QTextStream logStream(&logFile);
    logStream << txt << "\n\r" << endl;
}


int main(int argc, char *argv[])
{
    qInstallMessageHandler(snmpDebugMessageHandler);
    QCoreApplication a(argc, argv);
    QCoreApplication::setApplicationName("QSNMP-CA");
    QCoreApplication::setApplicationVersion("0.1");
    QCommandLineParser parser;
    parser.setApplicationDescription("QSNMP-CA helper");
    parser.addHelpOption();
    parser.addVersionOption();
    parser.addPositionalArgument("config", QCoreApplication::translate("config", "Config file."));
    parser.process(a);

    const QStringList args = parser.positionalArguments();
    if(args.size()>0)
    {
        QFileInfo configFile(args.at(0));
        if(configFile.exists())
        {
                configFileName = args.at(0);
        }
    }

    ApplicationClass snmpApp;
    snmpApp.configFile = configFileName;
    QObject::connect(&snmpApp,SIGNAL(finished()),&a,SLOT(quit()));
    QObject::connect(&a,SIGNAL(aboutToQuit()),&snmpApp,SLOT(aboutToQuitApp()));
    QTimer::singleShot(10,&snmpApp,SLOT(run()));
    return a.exec();
}
