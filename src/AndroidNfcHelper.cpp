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
// public static native void forwardIncomingSocketData(byte[] data);
JNIEXPORT void JNICALL Java_com_codexapertus_presto_PrestoActivity_forwardIncomingSocketData(JNIEnv *env, jobject obj, jbyteArray array)
{
    Q_UNUSED(obj);
    int len = env->GetArrayLength(array);
    char* buf = new char[len];
    env->GetByteArrayRegion(array, 0, len, reinterpret_cast<jbyte*>(buf));

    AndroidNfcHelper::instance()->forwardDataToSocket(QByteArray::fromRawData(buf, len));
}
}

AndroidNfcHelper *AndroidNfcHelper::sInstance = 0;

AndroidNfcHelper::AndroidNfcHelper(QObject *parent) : QObject(parent)
{
    Q_UNUSED(parent)
    sInstance = this;

    connect(this, &AndroidNfcHelper::bolt11ReceivedThroughNfc,
            LightningModel::instance()->paymentsModel(), &PaymentsModel::decodePayment);

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
    m_socket->write(socketData);
}

void AndroidNfcHelper::newConnection()
{
    qDebug() << "New connection on android socket";
    m_socket = m_socketServer->nextPendingConnection();
    connect(m_socket, &QLocalSocket::disconnected, m_socket, &QLocalSocket::deleteLater);
    connect(m_socket, &QLocalSocket::readyRead, this, &AndroidNfcHelper::readyRead);
}

void AndroidNfcHelper::readyRead()
{
    qDebug() << "Data read on android socket";
    QByteArray dataArray = m_socket->readAll();
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
}

AndroidNfcHelper *AndroidNfcHelper::instance()
{
    return sInstance;
}

