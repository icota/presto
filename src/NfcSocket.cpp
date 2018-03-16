#include "NfcSocket.h"

static nfcHostCardEmulationCallback_t nfcHceCallback;

void onHceDataReceived(unsigned char *data, unsigned int data_length)
{
    qDebug() << "(HCE)<<<:" << data;
}

void onHostCardEmulationActivated()
{

}

void onHostCardEmulationDeactivated()
{

}

NfcSocket::NfcSocket(QObject *parent) : QObject(parent)
{
    nfcHceCallback.onDataReceived = onHceDataReceived;
    nfcHceCallback.onHostCardEmulationActivated = onHostCardEmulationActivated;
    nfcHceCallback.onHostCardEmulationDeactivated = onHostCardEmulationDeactivated;
    nfcHce_registerHceCallback(&nfcHceCallback);

    int ret = nfcManager_doInitialize();
    qDebug() << "nfc init: " << ret;

    m_socketServer = new QLocalServer(this);
    m_socketServer->setSocketOptions(QLocalServer::WorldAccessOption);
    if (m_socketServer->listen("/home/igor/nfc-socket")) {
        connect(m_socketServer, &QLocalServer::newConnection, this, &NfcSocket::newConnection);
    }
    else {
        qDebug() << "failed!";
    }
}

void NfcSocket::newConnection()
{
    m_socket = m_socketServer->nextPendingConnection();
    connect(m_socket, &QLocalSocket::disconnected, m_socket, &QLocalSocket::deleteLater);
    connect(m_socket, &QLocalSocket::readyRead, this, &NfcSocket::readyRead);
}

void NfcSocket::readyRead()
{
    QByteArray buffer = m_socket->readAll();
    qDebug() << "(SOCKET)<<<: " << buffer; // probably better to read(max)

    // forward to NFC

    int res = nfcHce_sendCommand((unsigned char*)buffer.data(), buffer.length());

    if(res == 0)
    {
        qDebug() << "(HCE)>>>:" << buffer;
    }
    else {
        qDebug() << "HCE FAILURE";
    }
}


