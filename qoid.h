#ifndef QOID_H
#define QOID_H
#include<QString>
#include<QByteArray>
#include<QStringList>

class QOID
{

public:
    QByteArray *b_oid = NULL;
    QOID(QByteArray&);
    QOID(QOID&);
    ~QOID();
    QString getOID();
    QOID operator=(QOID);
};

#endif // QOID_H
