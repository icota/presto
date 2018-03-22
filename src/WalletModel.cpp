#include "WalletModel.h"
#include "./3rdparty/qjsonrpc/src/qjsonrpcservicereply.h"
#include "macros.h"

WalletModel::WalletModel(QJsonRpcSocket *rpcSocket)
{
    m_rpcSocket = rpcSocket;
    m_funds = QList<FundsTransaction>();
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
    SEND_MESSAGE_CONNECT_SLOT(message, &WalletModel::newAddressRequestFinished)
}

void WalletModel::newAddressRequestFinished()
{
    GET_MESSAGE_DISCONNECT_SLOT(message, &WalletModel::newAddressRequestFinished)
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

void WalletModel::withdrawFunds(QString destinationAddress, QString amountInSatoshi)
{
    QJsonObject paramsObject;
    paramsObject.insert("destination", destinationAddress);
    paramsObject.insert("satoshi", amountInSatoshi);

    QJsonRpcMessage message = QJsonRpcMessage::createRequest("withdraw", paramsObject);
    SEND_MESSAGE_CONNECT_SLOT(message, &WalletModel::withdrawFundsRequestFinished)
}

void WalletModel::withdrawFundsRequestFinished()
{
    GET_MESSAGE_DISCONNECT_SLOT(message, &WalletModel::withdrawFundsRequestFinished)
    if (message.type() == QJsonRpcMessage::Error)
    {
        emit errorString(message.toObject().value("error").toObject().value("message").toString());
    }

    if (message.type() == QJsonRpcMessage::Response)
    {
        QJsonObject jsonObject = message.toObject();

        QString bolt11 = jsonObject.value("result").toObject().value("bolt11").toString();

        if (bolt11.length() > 0)
        {
            //fetchInvoices();
            //emit invoiceAdded(bolt11);
        }

        if (jsonObject.contains("result"))
        {
            QJsonObject resultObject = jsonObject.value("result").toObject();
            if (resultObject.contains("id"))
            {
                // TODO: notify GUI that we've connected
                //fetchInvoices();
            }
        }
    }
}

void WalletModel::updateFunds()
{
    QJsonRpcMessage message = QJsonRpcMessage::createRequest("listfunds", QJsonValue());
    SEND_MESSAGE_CONNECT_SLOT(message, &WalletModel::listFundsRequestFinished)
}

void WalletModel::listFundsRequestFinished()
{
    GET_MESSAGE_DISCONNECT_SLOT(message, &WalletModel::listFundsRequestFinished)
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


void WalletModel::populateFundsFromJson(QJsonArray jsonArray)
{
    m_funds.clear();
    endResetModel();

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
