#include "NfcHelper.h"
#include "NfcController.h"
#include "LightningModel.h"
#include <QDir>

unsigned char SELECT_LIGHTNING[] = {0x00,0xA4,0x04,0x00,0x0A,0xF0, // Select custom AID
                                    0x4C, // L
                                    0x49, // I
                                    0x47, // G
                                    0x48, // H
                                    0x54, // T
                                    0x4E, // N
                                    0x49, // I
                                    0x4E, // N
                                    0x47};// G

unsigned char SELECT_OK[] = {0x90, 0x00};
unsigned char BOLT11_COMMAND = 0x01; // Followed by BOLT11 string
unsigned char NFC_SOCKET_COMMAND = 0x02; // Followed by peer ID
unsigned char BOLT11_RECEIVED_NO_SOCKET = 0x03;
unsigned char NFC_SOCKET_STREAM = 0x04;
unsigned char NFC_SOCKET_STREAM_NO_DATA = 0x05;

static const int packetSize = 128; // Need to keep this below 189 for some reason


NfcHelper::NfcHelper(QObject *parent) : QObject(parent)
{
    m_askForSocketData = false;
    m_nfcTagPresentLastState = false;

    m_bolt11 = "BROKEN";

    initializeNfc();

    m_tagStatusCheckTimer = new QTimer();
    m_tagStatusCheckTimer->setSingleShot(false);
    m_tagStatusCheckTimer->setInterval(100);
    connect(m_tagStatusCheckTimer, &QTimer::timeout, this, &NfcHelper::nfcTagStatusCheck);
    m_tagStatusCheckTimer->start();

    connect(LightningModel::instance()->peersModel(), &PeersModel::connectedToPeer,
            this, &NfcHelper::connectedToPeer);

    m_socketServer = new QLocalServer(this);
    m_socketServer->setSocketOptions(QLocalServer::WorldAccessOption);
    if (m_socketServer->listen(QDir::tempPath() + "/nfc-socket")) {
        connect(m_socketServer, &QLocalServer::newConnection, this, &NfcHelper::newConnection);
        m_socketServerPath = m_socketServer->fullServerName();
        qDebug() << "Socket opened: " << m_socketServerPath;
    }
    else {
        qDebug() << "Failed to open local socket.";
    }
}

void NfcHelper::setBolt11(const QString &bolt11)
{
    m_bolt11 = bolt11;
}

void NfcHelper::connectedToPeer(QString peerId)
{
    if (peerId == m_socketPeerId) {
        qDebug() << "Connected to NFC peer!";
    }
}

void NfcHelper::onNfcTagArrival()
{
    unsigned char response[2];
    int res = nfcTag_transceive(currentTagInfo.handle, SELECT_LIGHTNING, sizeof(SELECT_LIGHTNING), response, sizeof(response), 2000);
    if (res == 0) {
        qDebug() << "NFC transcieve failure!";
    }
    else {
        qDebug() << "NFC received: " << response[0] << response[1];
        if (memcmp(response, SELECT_OK, sizeof(res))){
            qDebug() << "NFC device has lightning support, sending BOLT11";
            sendBolt11ToHceDevice();
        }
    }
}

void NfcHelper::onNfcTagDeparture()
{

}

void NfcHelper::sendBolt11ToHceDevice()
{
    QByteArray bolt11Bytes = m_bolt11.toUtf8();

    int payloadSize = packetSize - 3;
    int numberOfPackets = (bolt11Bytes.size() / payloadSize) + 1;

    for (int i = 0; i < numberOfPackets; i++)
    {
        QByteArray bolt11Chunk = bolt11Bytes.left(payloadSize);
        bolt11Bytes = bolt11Bytes.mid(payloadSize);

        unsigned char command [3 + bolt11Chunk.length()];
        command[0] = BOLT11_COMMAND;
        command[1] = i;
        command[2] = numberOfPackets;
        memcpy(&command[3], (unsigned char*)bolt11Chunk.data(), bolt11Chunk.length());

        unsigned char response[34];
        int res = nfcTag_transceive(currentTagInfo.handle, command, sizeof(command), response, sizeof(response), 2000);

        if (res == 0) {
            qDebug() << "NFC transcieve failure!";
        }
        else {
            qDebug() << "NFC received: " << response[0];
            if (response[0] == BOLT11_RECEIVED_NO_SOCKET) {
                qDebug() << "BOLT11 received, no socket pls";
            }
            else if (response[0] == NFC_SOCKET_COMMAND) {
                qDebug() << "Device wants a socket connection";
                QByteArray socketPeerId = QByteArray::fromRawData((const char*)&response[1], 33);
                m_socketPeerId = QString(socketPeerId.toHex());
                LightningModel::instance()->peersModel()->connectToPeer(m_socketPeerId, m_socketServerPath);
            }
        }
    }
}

void NfcHelper::newConnection()
{
    m_socket = m_socketServer->nextPendingConnection();
    connect(m_socket, &QLocalSocket::disconnected, m_socket, &QLocalSocket::deleteLater);
    connect(m_socket, &QLocalSocket::readyRead, this, &NfcHelper::readyRead);
}

void NfcHelper::readyRead()
{
    QByteArray buffer = m_socket->read(packetSize - 1);

    unsigned char command [1 + buffer.length()];
    command[0] = NFC_SOCKET_STREAM;
    memcpy(&command[1], (unsigned char*)buffer.data(), buffer.length());

    unsigned char response[512];
    int res = nfcTag_transceive(currentTagInfo.handle, command, sizeof(command), response, sizeof(response), 2000);

    if (res == 0) {
        qDebug() << "NFC socket transcieve failure!";
    }
    else {
        qDebug() << "NFC socket received response: " << response[0];
        if (response[0] == NFC_SOCKET_STREAM) {
            qDebug() << "Socket data received, forwarding";
            m_socket->write((const char*)response[1], res - 1);
        }
        else if (response[0] == NFC_SOCKET_STREAM_NO_DATA) {
            // ask for data in another 100 ms
            m_askForSocketData = true;
        }
    }
}

void NfcHelper::nfcTagStatusCheck()
{
    if(tagPresent != m_nfcTagPresentLastState) {
        m_nfcTagPresentLastState = tagPresent;
        if (m_nfcTagPresentLastState == true) {
            onNfcTagArrival();
        }
        else {
            onNfcTagDeparture();
        }
    }

    if (m_askForSocketData) {
        readyRead();
    }
}

void NfcHelper::forwardDataToSocket(QByteArray socketData)
{
    // not using this for now
    qDebug() << "forwardDataToSocket:" << socketData;
    m_socket->write(socketData);
}


