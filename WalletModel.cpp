#include "WalletModel.h"
#include "qjsonrpc/qjsonrpcservicereply.h"

WalletModel::WalletModel(QJsonRpcSocket *rpcSocket)
{
    m_rpcSocket = rpcSocket;
    fetchFunds();
}

QHash<int, QByteArray> WalletModel::roleNames() const {
    QHash<int, QByteArray> roles;
    roles[TxidRole] = "txid";
    roles[OutputRole] = "output";
    roles[SatoshiRole] = "satoshi";
    return roles;
}

int WalletModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return m_funds.count();
}

QVariant WalletModel::data(const QModelIndex &index, int role) const
{
    if (index.row() < 0 || index.row() >= m_funds.count())
        return QVariant();

    const FundsTransaction &fundsTransaction = m_funds[index.row()];
    if (role == TxidRole)
        return fundsTransaction.txId();
    else if (role == OutputRole)
        return fundsTransaction.outputs();
    else if (role == SatoshiRole)
        return fundsTransaction.amountSatoshi();
    return QVariant();
}

int WalletModel::totalAvailableFunds()
{
    int sumOfAvailableFunds = 0;
    foreach (FundsTransaction transaction, m_funds) {
        sumOfAvailableFunds += transaction.amountSatoshi();
    }
    return sumOfAvailableFunds;
}

void WalletModel::requestNewAddress()
{
    QJsonRpcMessage message = QJsonRpcMessage::createRequest("newaddr", QJsonValue());
    QJsonRpcServiceReply* reply = m_rpcSocket->sendMessage(message);
    QObject::connect(reply, SIGNAL(finished()), this, SLOT(newAddressRequestFinished()));
}

void WalletModel::fetchFunds()
{
    QJsonRpcMessage message = QJsonRpcMessage::createRequest("listfunds", QJsonValue());
    QJsonRpcServiceReply* reply = m_rpcSocket->sendMessage(message);
    QObject::connect(reply, SIGNAL(finished()), this, SLOT(listFundsRequestFinished()));
}

void WalletModel::listFundsRequestFinished()
{
    QJsonRpcServiceReply *reply = static_cast<QJsonRpcServiceReply *>(sender());
    QJsonRpcMessage message = reply->response();

    if (message.type() == QJsonRpcMessage::Response)
    {
        QJsonObject jsonObject = message.toObject();

        if (jsonObject.contains("result"))
        {
            QJsonObject resultObject = jsonObject.value("result").toObject();
            populateFundsFromJson(resultObject.value("outputs").toArray());
        }
    }
}

void WalletModel::newAddressRequestFinished()
{
    QJsonRpcServiceReply *reply = static_cast<QJsonRpcServiceReply *>(sender());
    QJsonRpcMessage message = reply->response();

    if (message.type() == QJsonRpcMessage::Response)
    {
        QJsonObject jsonObject = message.toObject();

        if (jsonObject.contains("result"))
        {
            QJsonObject resultObject = jsonObject.value("result").toObject();
            emit newAddress(resultObject.value("address").toString());
        }
    }
}

void WalletModel::populateFundsFromJson(QJsonArray jsonArray)
{
    foreach (const QJsonValue &v, jsonArray)
    {
        beginInsertRows(QModelIndex(), rowCount(), rowCount());

        QJsonObject OutputsJsonObject = v.toObject();

        FundsTransaction fundsTransaction;
        fundsTransaction.setTxId(OutputsJsonObject.value("output").toString());
        fundsTransaction.setOutputs(OutputsJsonObject.value("txid").toBool());
        fundsTransaction.setAmountSatoshi(OutputsJsonObject.value("value").toInt());

        m_funds.append(fundsTransaction);

        endInsertRows();
    }

    emit totalAvailableFundsChanged();
}

QString FundsTransaction::txId() const
{
    return m_txId;
}

void FundsTransaction::setTxId(const QString &txId)
{
    m_txId = txId;
}

int FundsTransaction::outputs() const
{
    return m_outputs;
}

void FundsTransaction::setOutputs(int outputs)
{
    m_outputs = outputs;
}

int FundsTransaction::amountSatoshi() const
{
    return m_amountSatoshi;
}

void FundsTransaction::setAmountSatoshi(int amountSatoshi)
{
    m_amountSatoshi = amountSatoshi;
}
