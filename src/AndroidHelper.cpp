#include "AndroidHelper.h"

#include <QAndroidJniObject>
#include <jni.h>

extern "C" {
    JNIEXPORT void JNICALL Java_com_codexapertus_presto_PrestoActivity_bolt11Received(JNIEnv *env, jobject obj, jstring Param1)
    {
        Q_UNUSED(env);
        Q_UNUSED(obj);
        QAndroidJniObject bolt11String = QAndroidJniObject::fromLocalRef(Param1);
        qDebug() << "BOLT11 received from JNI: " << bolt11String.toString();
        LightningModel::instance()->paymentsModel()->decodePayment(bolt11String.toString());
    }
}

AndroidHelper::AndroidHelper(QObject *parent) : QObject(parent)
{
    m_socketServer = new QLocalServer(this);
    m_socketServer->setSocketOptions(QLocalServer::WorldAccessOption);
    if (m_socketServer->listen(QDir::homePath() + "/nfc-socket")) {
        connect(m_socketServer, &QLocalServer::newConnection, this, &AndroidHelper::newConnection);
        m_socketServerPath = m_socketServer->fullServerName();
        qDebug() << "Socket opened: " << m_socketServerPath;
    }
    else {
        qDebug() << "Failed to open local socket.";
    }
}

