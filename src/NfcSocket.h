#ifndef NFCSOCKET_H
#define NFCSOCKET_H

#include <QObject>
#include <QLocalServer>
#include <QLocalSocket>

#include "3rdparty/linux_libnfc-nci/src/include/linux_nfc_api.h"
//#include "3rdparty/linux_libnfc-nci/src/service/interface/nativeNfcManager.h"

class NfcSocket : public QObject
{
    Q_OBJECT
public:
    explicit NfcSocket(QObject *parent = nullptr);

private:
    QLocalServer* m_socketServer;
    QLocalSocket* m_socket;

private slots:
    void newConnection();
    void readyRead();

signals:

public slots:
};

#endif // NFCSOCKET_H
