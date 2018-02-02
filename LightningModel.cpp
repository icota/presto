#include "LightningModel.h"

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

        QObject::connect(m_rpcSocket, &QJsonRpcAbstractSocket::messageReceived, this, &LightningModel::rpcMessageReceived);

        m_unixSocket->connectToServer("/home/igor/.lightning/lightning-rpc");
    }
}

void LightningModel::rpcConnected()
{
    m_peersModel = new PeersModel(m_rpcSocket);
    m_paymentsModel = new PaymentsModel(m_rpcSocket);
    m_walletModel = new WalletModel(m_rpcSocket);
    m_invoicesModel = new InvoicesModel(m_rpcSocket);

    m_updatesTimer = new QTimer();

    // Update the models every 15 secs
    m_updatesTimer->setInterval(15000);
    m_updatesTimer->setSingleShot(false);
    QObject::connect(m_updatesTimer, &QTimer::timeout, this, &LightningModel::updateModels);
    m_updatesTimer->start();
}

void LightningModel::rpcMessageReceived(QJsonRpcMessage message)
{
    qDebug() << message.toJson();
}

void LightningModel::unixSocketError(QLocalSocket::LocalSocketError socketError)
{
    qDebug() << socketError;
}

void LightningModel::updateModels()
{
    m_peersModel->updatePeers();
    m_paymentsModel->updatePayments();
    m_walletModel->updateFunds();
    m_invoicesModel->updateInvoices();
}
