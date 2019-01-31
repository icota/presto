#include <QDir>
#include <QSettings>

#include "LightningModel.h"

#ifdef Q_OS_ANDROID
#include <QtAndroid>
#include <QAndroidJniObject>
#include <QAndroidJniEnvironment>
#endif

LightningModel *LightningModel::sInstance = nullptr;

PeersModel *LightningModel::peersModel() const
{
    return m_peersModel;
}

PaymentsModel *LightningModel::paymentsModel() const
{
    return m_paymentsModel;
}

WalletModel *LightningModel::walletModel() const
{
    return m_walletModel;
}

InvoicesModel *LightningModel::invoicesModel() const
{
    return m_invoicesModel;
}

QString LightningModel::id() const
{
    return m_id;
}

int LightningModel::port() const
{
    return m_port;
}

QString LightningModel::address() const
{
    return m_address;
}

QString LightningModel::version() const
{
    return m_version;
}

int LightningModel::blockheight() const
{
    return m_blockheight;
}

QString LightningModel::network() const
{
    return m_network;
}

void LightningModel::updateInfo()
{
//    QJsonRpcMessage message = QJsonRpcMessage::createRequest("getinfo", QJsonValue());
//    SEND_MESSAGE_CONNECT_SLOT(message, &LightningModel::updateInfoRequestFinished)
}

void LightningModel::updateInfoRequestFinished()
{
//    GET_MESSAGE_DISCONNECT_SLOT(message, &LightningModel::updateInfoRequestFinished)
//    if (message.type() == QJsonRpcMessage::Error)
//    {
//        //emit errorString(message.toObject().value("error").toObject().value("message").toString());
//    }

//    if (message.type() == QJsonRpcMessage::Response)
//    {
//        QJsonObject resultsObject = message.toObject().value("result").toObject();

//        QJsonArray addressesArray = resultsObject.value("address").toArray();


//        m_address = addressesArray[0].toObject().value("address").toString();
//        m_blockheight = resultsObject.value("blockheight").toInt();
//        setId(resultsObject.value("id").toString());
//        m_network = resultsObject.value("network").toString();
//        m_port = resultsObject.value("port").toInt();
//        m_version = resultsObject.value("version").toString();

//        emit infoChanged();
//    }
}


LightningModel::LightningModel(QString serverName, QObject *parent) {
    {
        Q_UNUSED(parent);

        sInstance = this;

        m_firstStart = true;

        QSettings settings;

        m_address = QString();
        m_blockheight = 0;
        m_id = QString();
        m_network = QString();
        m_port = 0;
        m_version = QString();


        m_peersModel = new PeersModel();
        m_paymentsModel = new PaymentsModel();
        m_walletModel = new WalletModel();
        m_invoicesModel = new InvoicesModel();

        m_nodesModel = new NodesModel();
    }
}

LightningModel *LightningModel::instance()
{
    return sInstance;
}

void LightningModel::setId(const QString &id)
{
    m_id = id;
#ifdef Q_OS_ANDROID
    // Let JNI glue know as well; we might need it to set up an ad-hoc NFC connection

    //send it as a byte array
    QByteArray idByteArray = QByteArray::fromHex(id.toLatin1()).data();
    QAndroidJniEnvironment env;
    jbyteArray javaDataArray = env->NewByteArray(33);
    env->SetByteArrayRegion(javaDataArray, 0, 33, (jbyte*)idByteArray.data());
    QtAndroid::androidContext().callMethod<void>("setId",
                                                 "([B)V",
                                                 javaDataArray);
#endif
}

NodesModel *LightningModel::nodesModel() const
{
    return m_nodesModel;
}

QString LightningModel::manualAddress() const
{
    return m_manualAddress;
}

void LightningModel::setManualAddress(const QString &manualAddress)
{
    m_manualAddress = manualAddress;
}
