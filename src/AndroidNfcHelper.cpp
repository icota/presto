#include "AndroidNfcHelper.h"

#include <QAndroidJniObject>
#include <jni.h>

extern "C" {
JNIEXPORT void JNICALL Java_com_codexapertus_presto_PrestoActivity_bolt11Received(JNIEnv *env, jobject obj, jstring Param1)
{
    Q_UNUSED(env);
    Q_UNUSED(obj);
    QAndroidJniObject bolt11String = QAndroidJniObject::fromLocalRef(Param1);
    AndroidNfcHelper::instance()->bolt11FromJni(bolt11String.toString());
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
    emit bolt11ReceivedThroughNfc(bolt11);
}

AndroidNfcHelper *AndroidNfcHelper::instance()
{
    return sInstance;
}

