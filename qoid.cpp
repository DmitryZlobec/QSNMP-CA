#include "qoid.h"
#include <QByteArray>
#include<QString>
#include <QDebug>
QOID::QOID( QByteArray &oid)
{
    b_oid = new QByteArray(oid);
}

QOID::~QOID()
{

    delete b_oid;
}

QOID::QOID( QOID &oid)
{
    b_oid = new QByteArray(oid.b_oid->data());
}


QOID QOID::operator=(QOID oid) {
        if(this != &oid)
        {
            if(this->b_oid != NULL) delete this->b_oid;
            b_oid = new QByteArray(oid.b_oid->data());
        }
        return *this;


}
// http://www.codeproject.com/KB/cs/asn1editor.aspx
QString QOID::getOID()
{
    QString s_oid;

    qint32 x = static_cast<unsigned char>(b_oid->at(2)) / 40;
    qint32 y = static_cast<unsigned char>(b_oid->at(2)) % 40;

    if(x>2)
    {
        y += (x - 2) * 40;
        x = 2;
    }

    s_oid.append(QString::number(x));
    s_oid.append('.');
    s_oid.append(QString::number(y));

     quint64 val=0;

    for (qint32 i = 3; i < b_oid->size(); i++)
        {
            val = ((val << 7) | (static_cast<unsigned char>((b_oid->at(i) & 0x7F))));
            if (!((b_oid->at(i) & 0x80) == 0x80))
            {
                s_oid.append('.');
                s_oid.append(QString::number(val));
                val = 0;
            }
        }
    return s_oid;
}



