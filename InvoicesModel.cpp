#include "InvoicesModel.h"
#include "qjsonrpc/qjsonrpcservicereply.h"

InvoicesModel::InvoicesModel(QJsonRpcSocket *rpcSocket)
{
    m_rpcSocket = rpcSocket;
    fetchInvoices();
}

QHash<int, QByteArray> InvoicesModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[LabelRole] = "label";
    roles[HashRole] = "hash";
    roles[MSatoshiRole] = "msatoshi";
    roles[StatusRole] = "status";
    roles[PayIndexRole] = "payIndex";
    roles[MSatishiReceivedRole] = "msatoshiReceived";
    roles[PaidTimestampRole] = "paidTimestamp";
    roles[PaidAtTimestampRole] = "paidAtTimestamp";
    roles[ExpiryTimeRole] = "expiryTime";
    roles[ExpiresAtRole] = "expiresAt";
    return roles;
}

int InvoicesModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return m_invoices.count();
}

QVariant InvoicesModel::data(const QModelIndex &index, int role) const
{
    if (index.row() < 0 || index.row() >= m_invoices.count())
        return QVariant();

    const Invoice &invoice = m_invoices[index.row()];
    if (role == LabelRole)
        return invoice.label();
    else if (role == HashRole)
        return invoice.hash();
    else if (role == MSatoshiRole)
        return invoice.msatoshi();
    else if (role == StatusRole)
        return invoice.status();
    else if (role == PayIndexRole)
        return invoice.payIndex();
    else if (role == MSatishiReceivedRole)
        return invoice.msatoshiReceived();
    else if (role == PaidTimestampRole)
        return invoice.paidTimestamp();
    else if (role == PaidAtTimestampRole)
        return invoice.paidAtTimestamp();
    else if (role == ExpiryTimeRole)
        return invoice.expiryTime();
    else if (role == ExpiresAtRole)
        return invoice.expiresAtTime();
    return QVariant();
}

void InvoicesModel::fetchInvoices()
{
    QJsonRpcMessage message = QJsonRpcMessage::createRequest("listinvoices", QJsonValue());
    QJsonRpcServiceReply* reply = m_rpcSocket->sendMessage(message);
    QObject::connect(reply, SIGNAL(finished()), this, SLOT(listInvoicesRequestFinished()));
}

void InvoicesModel::listInvoicesRequestFinished()
{
    QJsonRpcServiceReply *reply = static_cast<QJsonRpcServiceReply *>(sender());
    QJsonRpcMessage message = reply->response();

    if (message.type() == QJsonRpcMessage::Response)
    {
        QJsonObject jsonObject = message.toObject();

        if (jsonObject.contains("result"))
        {
            QJsonObject resultObject = jsonObject.value("result").toObject();
            populateInvoicesFromJson(resultObject.value("invoices").toArray());
        }
    }
}

void InvoicesModel::populateInvoicesFromJson(QJsonArray jsonArray)
{
    foreach (const QJsonValue &v, jsonArray)
    {
        beginInsertRows(QModelIndex(), rowCount(), rowCount());

        QJsonObject InvoiceJsonObject = v.toObject();
        Invoice invoice;
        invoice.setLabel(InvoiceJsonObject.value("label").toString());
        invoice.setHash(InvoiceJsonObject.value("payment_hash").toString());
        invoice.setMsatoshi(InvoiceJsonObject.value("msatoshi").toInt());
        invoice.setStatus((Invoice::InvoiceStatus)InvoiceJsonObject.value("status").toInt()); // TODO
        invoice.setPayIndex(InvoiceJsonObject.value("pay_index").toInt());
        invoice.setMsatoshiReceived(InvoiceJsonObject.value("msatoshi_received").toInt());
        invoice.setPaidTimestamp(InvoiceJsonObject.value("paid_timestamp").toInt()); // TODO: Fix this
        invoice.setPaidAtTimestamp(InvoiceJsonObject.value("paid_at").toInt());
        invoice.setExpiryTime(InvoiceJsonObject.value("expiry_time").toInt());
        invoice.setExpiresAtTime(InvoiceJsonObject.value("expires_at").toInt());

        m_invoices.append(invoice);

        endInsertRows();
    }
}

Invoice::Invoice()
{}

QString Invoice::label() const
{
    return m_label;
}

void Invoice::setLabel(const QString &label)
{
    m_label = label;
}

QString Invoice::hash() const
{
    return m_hash;
}

void Invoice::setHash(const QString &hash)
{
    m_hash = hash;
}

int Invoice::msatoshi() const
{
    return m_msatoshi;
}

void Invoice::setMsatoshi(int msatoshi)
{
    m_msatoshi = msatoshi;
}

Invoice::InvoiceStatus Invoice::status() const
{
    return m_status;
}

void Invoice::setStatus(const InvoiceStatus &status)
{
    m_status = status;
}

int Invoice::payIndex() const
{
    return m_payIndex;
}

void Invoice::setPayIndex(int payIndex)
{
    m_payIndex = payIndex;
}

int Invoice::msatoshiReceived() const
{
    return m_msatoshiReceived;
}

void Invoice::setMsatoshiReceived(int msatoshiReceived)
{
    m_msatoshiReceived = msatoshiReceived;
}

int Invoice::paidTimestamp() const
{
    return m_paidTimestamp;
}

void Invoice::setPaidTimestamp(int paidTimestamp)
{
    m_paidTimestamp = paidTimestamp;
}

int Invoice::paidAtTimestamp() const
{
    return m_paidAtTimestamp;
}

void Invoice::setPaidAtTimestamp(int paidAtTimestamp)
{
    m_paidAtTimestamp = paidAtTimestamp;
}

int Invoice::expiryTime() const
{
    return m_expiryTime;
}

void Invoice::setExpiryTime(int expiryTime)
{
    m_expiryTime = expiryTime;
}

int Invoice::expiresAtTime() const
{
    return m_expiresAtTime;
}

void Invoice::setExpiresAtTime(int expiresAtTime)
{
    m_expiresAtTime = expiresAtTime;
}
