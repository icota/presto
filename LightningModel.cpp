#include "LightningModel.h"

// TODO: Add all the strings to a namespaces
//namespace CLightningJson {
//static QString
//}

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

LightningModel::LightningModel(QObject *parent) {
    {
        Q_UNUSED(parent);
        m_unixSocket = new QLocalSocket();
        m_rpcSocket = new QJsonRpcSocket(m_unixSocket);

        QObject::connect(m_unixSocket, &QLocalSocket::connected,
                         this, &LightningModel::rpcConnected);

        QObject::connect(m_unixSocket, SIGNAL(error(QLocalSocket::LocalSocketError)),
                         this, SLOT(unixSocketError(QLocalSocket::LocalSocketError)));

        QObject::connect(m_rpcSocket, SIGNAL(messageReceived(QJsonRpcMessage)), this, SLOT(rpcMessageReceived(QJsonRpcMessage)));

        m_unixSocket->connectToServer("/home/igor/.lightning/lightning-rpc");
    }
}

void LightningModel::rpcConnected()
{
    m_peersModel = new PeersModel(m_rpcSocket);
    m_paymentsModel = new PaymentsModel(m_rpcSocket);
    m_walletModel = new WalletModel(m_rpcSocket);
    m_invoicesModel = new InvoicesModel(m_rpcSocket);
}

void LightningModel::rpcMessageReceived(QJsonRpcMessage message)
{
    qDebug() << message.toJson();
}

void LightningModel::unixSocketError(QLocalSocket::LocalSocketError socketError)
{
    qDebug() << socketError;
}
