#include "NfcSocket.h"
#include "NfcController.h"
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


NfcSocket::NfcSocket(QObject *parent) : QObject(parent)
{
    m_askForSocketData = false;
    m_nfcTagPresentLastState = false;

    bolt11 = "lntb15u1pd276zcpp5mnkhsg09uraqjyvw636emw8aga6yg7smmr6d6u26yg4t9dxkdyvqdq4"
             "xysyymr0vd4kzcmrd9hx7cqp2p6nuyvw0yg96dx6l9zg4mffdtl53tval406lqwe8hu85rrle"
             "dmspwnmgw73emnccg4skel56me6wy40mhf0024qzrwpxhmsv4sfvtqspe3mgsy";

    initializeNfc();

    m_tagStatusCheckTimer = new QTimer();
    m_tagStatusCheckTimer->setSingleShot(false);
    m_tagStatusCheckTimer->setInterval(100);
    connect(m_tagStatusCheckTimer, &QTimer::timeout, this, &NfcSocket::nfcTagStatusCheck);
    m_tagStatusCheckTimer->start();

    m_socketServer = new QLocalServer(this);
    m_socketServer->setSocketOptions(QLocalServer::WorldAccessOption);
    if (m_socketServer->listen(QDir::tempPath() + "/nfc-socket")) {
        connect(m_socketServer, &QLocalServer::newConnection, this, &NfcSocket::newConnection);
        m_socketServerPath = m_socketServer->fullServerName();
        qDebug() << "Socket opened: " << m_socketServerPath;
    }
    else {
        qDebug() << "Failed to open local socket.";
    }
}

void NfcSocket::onNfcTagArrival()
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

void NfcSocket::onNfcTagDeparture()
{

}

void NfcSocket::sendBolt11ToHceDevice()
{
    QByteArray bolt11Bytes = QByteArray(bolt11);

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

        unsigned char response[100];
        int res = nfcTag_transceive(currentTagInfo.handle, command, sizeof(command), response, sizeof(response), 2000);

        if (res == 0) {
            qDebug() << "NFC transcieve failure!";
        }
        else {
            qDebug() << "NFC received: " << response[0];
            if (response[0] == BOLT11_RECEIVED_NO_SOCKET) {
                qDebug() << "BOLT11 received, no socket pls";
            }
        }
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

void NfcSocket::nfcTagStatusCheck()
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


