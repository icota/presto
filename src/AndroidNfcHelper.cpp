#include "AndroidNfcHelper.h"

#include <QAndroidJniObject>
#include <QtAndroid>
#include <QAndroidJniEnvironment>
#include <jni.h>

extern "C" {
JNIEXPORT void JNICALL Java_com_codexapertus_presto_PrestoActivity_bolt11Received(JNIEnv *env, jobject obj, jstring Param1)
{
    Q_UNUSED(env);
    Q_UNUSED(obj);
    QAndroidJniObject bolt11String = QAndroidJniObject::fromLocalRef(Param1);
    AndroidNfcHelper::instance()->bolt11FromJni(bolt11String.toString());
}

JNIEXPORT void JNICALL Java_com_codexapertus_presto_PrestoActivity_forwardIncomingSocketData(JNIEnv *env, jobject obj, jbyteArray array)
{
    Q_UNUSED(obj);
    qDebug() << "forwardIncomingSocketData";
    int len = env->GetArrayLength(array);
    qDebug() << "len: " << len;
    char* buf = new char[len];
    env->GetByteArrayRegion(array, 0, len, reinterpret_cast<jbyte*>(buf));

    // Needs to be a different thread
    QMetaObject::invokeMethod(AndroidNfcHelper::instance(),
                              "forwardDataToSocket",
                              Qt::QueuedConnection,
                              Q_ARG(QByteArray, QByteArray::fromRawData(buf, len)));
    //AndroidNfcHelper::instance()->forwardDataToSocket(QByteArray::fromRawData(buf, len));
}

JNIEXPORT void JNICALL Java_com_codexapertus_presto_PrestoActivity_linkDeactived(JNIEnv *env, jobject obj, jint reason)
{
    Q_UNUSED(obj);
    qDebug() << "linkDeactivated";

    AndroidNfcHelper::instance()->linkDeactivated(reason);
}
}

AndroidNfcHelper *AndroidNfcHelper::sInstance = 0;

AndroidNfcHelper::AndroidNfcHelper(QObject *parent) : QObject(parent)
{
    Q_UNUSED(parent)
    sInstance = this;

    m_socketPeerId = QString();

    connect(this, &AndroidNfcHelper::bolt11ReceivedThroughNfc,
            LightningModel::instance()->paymentsModel(), &PaymentsModel::decodePayment);

    connect(LightningModel::instance()->peersModel(), &PeersModel::connectedToPeer,
            this, &AndroidNfcHelper::connectedToPeer);

    m_socketServer = new QLocalServer(this);
    m_socketServer->setSocketOptions(QLocalServer::WorldAccessOption);
    if (m_socketServer->listen(QDir::homePath() + "/nfc-socket")) {
        connect(m_socketServer, &QLocalServer::newConnection, this, &AndroidNfcHelper::newConnection);
        m_socketServerPath = m_socketServer->fullServerName();
        qDebug() << "Socket opened: " << m_socketServerPath;
    }
    else {
        qDebug() << "Failed to open local socket.";
    }
}

void AndroidNfcHelper::bolt11FromJni(QString bolt11)
{
    // We have to emit a signal because JNI and QSocket can't be on the same thread
    qDebug() << "BOLT11 received from JNI: " << bolt11;
    connect(LightningModel::instance()->paymentsModel(), &PaymentsModel::paymentDecoded,
            this, &AndroidNfcHelper::paymentDecoded);

    emit bolt11ReceivedThroughNfc(bolt11);
}

void AndroidNfcHelper::forwardDataToSocket(QByteArray socketData)
{
    qDebug() << "forwardDataToSocket: " << socketData;
    if (!socketData.isEmpty() && !m_socketPeerId.isEmpty() && m_socket && m_socket->isOpen()) {
        qDebug() << "Writing to socket!";
        m_socket->write(socketData);
    }
    else {
        qDebug() << "c-lightning not connected to socket";
    }
}

void AndroidNfcHelper::linkDeactivated(int reason)
{
    qDebug() << "Link deactivated reason: " << reason;
    m_socketPeerId.clear();
//    if (m_socket) {
//        m_socket->disconnectFromServer();
//    }
}

void AndroidNfcHelper::newConnection()
{
    qDebug() << "New connection on android socket";
    m_socket = m_socketServer->nextPendingConnection();
    connect(m_socket, &QLocalSocket::disconnected, this, &AndroidNfcHelper::socketDisconnected);
    connect(m_socket, &QLocalSocket::readyRead, this, &AndroidNfcHelper::readyRead);
}

void AndroidNfcHelper::socketDisconnected()
{
    qDebug() << "Android socket disconnected";
    //m_socket = NULL;
}

void AndroidNfcHelper::readyRead()
{
    QByteArray dataArray = m_socket->readAll();
    qDebug() << "Data read on android socket: " << dataArray.length();
    qDebug() << dataArray.toHex();
    QAndroidJniEnvironment env;
    jbyteArray javaDataArray = env->NewByteArray(dataArray.length());
    env->SetByteArrayRegion(javaDataArray, 0, dataArray.length(), (jbyte*)dataArray.data());
    QtAndroid::androidContext().callMethod<void>("forwardSocket",
                                                 "([B)V",
                                                 javaDataArray);
}

void AndroidNfcHelper::paymentDecoded(int createdAt, QString currency, QString description, int expiry, int minFinalCltvExpiry, int msatoshi, QString payee, QString paymentHash, QString signature, int timestamp, QString bolt11)
{
    Q_UNUSED(createdAt)
    Q_UNUSED(currency)
    Q_UNUSED(description)
    Q_UNUSED(expiry)
    Q_UNUSED(minFinalCltvExpiry)
    Q_UNUSED(msatoshi)
    Q_UNUSED(paymentHash)
    Q_UNUSED(signature)
    Q_UNUSED(timestamp)
    Q_UNUSED(bolt11)

    disconnect(LightningModel::instance()->paymentsModel(), &PaymentsModel::paymentDecoded,
               this, &AndroidNfcHelper::paymentDecoded);

    m_socketPeerId = payee;
    qDebug() << "m_socketPeerId:" << m_socketPeerId;

    LightningModel::instance()->peersModel()->connectToPeer(m_socketPeerId, m_socketServerPath);
    // Only ask for socket after connecting?
}

void AndroidNfcHelper::connectedToPeer(QString peerId)
{
    if (peerId == m_socketPeerId) {
        qDebug() << "Connected to NFC peer!";
    }
}

AndroidNfcHelper *AndroidNfcHelper::instance()
{
    return sInstance;
}

