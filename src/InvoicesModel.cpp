#include "InvoicesModel.h"
#include "./3rdparty/qjsonrpc/src/qjsonrpcservicereply.h"

InvoicesModel::InvoicesModel(QJsonRpcSocket *rpcSocket)
{
    m_rpcSocket = rpcSocket;
    m_invoices = QList<Invoice>();
}

QHash<int, QByteArray> InvoicesModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[LabelRole] = "invoicelabel";
    roles[HashRole] = "hash";
    roles[MSatoshiRole] = "msatoshi";
    roles[StatusRole] = "status";
    roles[StatusStringRole] = "statusString";
    roles[PayIndexRole] = "payIndex";
    roles[MSatishiReceivedRole] = "msatoshiReceived";
    roles[PaidTimestampRole] = "paidTimestamp";
    roles[PaidAtTimestampRole] = "paidAtTimestamp";
    roles[ExpiryTimeRole] = "expiryTime";
    roles[ExpiresAtRole] = "expiresAt";
    roles[Bolt11Role] = "bolt11";
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
    else if (role == StatusStringRole)
        return invoice.statusString();
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
    else if (role == Bolt11Role)
        return invoice.bolt11();
    return QVariant();
}

void InvoicesModel::updateInvoices()
{
    QJsonRpcMessage message = QJsonRpcMessage::createRequest("listinvoices", QJsonValue());
    QJsonRpcServiceReply* reply = m_rpcSocket->sendMessage(message);
    QObject::connect(reply, &QJsonRpcServiceReply::finished, this, &InvoicesModel::listInvoicesRequestFinished);
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
    m_invoices.clear();
    endResetModel();

    foreach (const QJsonValue &v, jsonArray)
    {
        beginInsertRows(QModelIndex(), rowCount(), rowCount());

        QJsonObject invoiceJsonObject = v.toObject();
        Invoice invoice;
        invoice.setLabel(invoiceJsonObject.value("label").toString());
        invoice.setHash(invoiceJsonObject.value("payment_hash").toString());
        invoice.setMsatoshi(invoiceJsonObject.value("msatoshi").toInt());

        QString status = invoiceJsonObject.value("status").toString();

        if (status.toLower() == "paid") invoice.setStatus(InvoiceTypes::InvoiceStatus::PAID);
        else if (status.toLower() == "unpaid") invoice.setStatus(InvoiceTypes::InvoiceStatus::UNPAID);
        else if (status.toLower() == "expired") invoice.setStatus(InvoiceTypes::InvoiceStatus::EXPIRED);

        invoice.setStatusString(status);

        invoice.setPayIndex(invoiceJsonObject.value("pay_index").toInt());
        invoice.setMsatoshiReceived(invoiceJsonObject.value("msatoshi_received").toInt());
        invoice.setPaidTimestamp(invoiceJsonObject.value("paid_timestamp").toInt()); // TODO: Fix this
        invoice.setPaidAtTimestamp(invoiceJsonObject.value("paid_at").toInt());
        invoice.setExpiryTime(invoiceJsonObject.value("expiry_time").toInt());
        invoice.setExpiresAtTime(invoiceJsonObject.value("expires_at").toInt());
        invoice.setBolt11(invoiceJsonObject.value("bolt11").toString());

        m_invoices.append(invoice);

        endInsertRows();
    }
}

void InvoicesModel::addInvoice(QString label, QString description, QString amountInMsatoshi)
{
    QJsonObject paramsObject;
    paramsObject.insert("label", label);
    paramsObject.insert("description", description);
    paramsObject.insert("msatoshi", amountInMsatoshi);

    QJsonRpcMessage message = QJsonRpcMessage::createRequest("invoice", paramsObject);
    QJsonRpcServiceReply* reply = m_rpcSocket->sendMessage(message);
    QObject::connect(reply, &QJsonRpcServiceReply::finished, this, &InvoicesModel::addInvoiceRequestFinished);
}

void InvoicesModel::addInvoiceRequestFinished()
{
    QJsonRpcServiceReply *reply = static_cast<QJsonRpcServiceReply *>(sender());
    QJsonRpcMessage message = reply->response();

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
            updateInvoices();
            emit invoiceAdded(bolt11);
        }

        if (jsonObject.contains("result"))
        {
            QJsonObject resultObject = jsonObject.value("result").toObject();
            if (resultObject.contains("id"))
            {
                // TODO: notify GUI that we've connected
                updateInvoices();
            }
        }
    }
}

void InvoicesModel::deleteInvoice(QString label, QString status)
{
    QJsonObject paramsObject;
    paramsObject.insert("label", label);
    paramsObject.insert("status", status);

    QJsonRpcMessage message = QJsonRpcMessage::createRequest("delinvoice", paramsObject);
    QJsonRpcServiceReply* reply = m_rpcSocket->sendMessage(message);
    QObject::connect(reply, &QJsonRpcServiceReply::finished, this, &InvoicesModel::deleteInvoiceRequestFinished);
}

void InvoicesModel::deleteInvoiceRequestFinished()
{
    QJsonRpcServiceReply *reply = static_cast<QJsonRpcServiceReply *>(sender());
    QJsonRpcMessage message = reply->response();

    if (message.type() == QJsonRpcMessage::Error)
    {
        emit errorString(message.toObject().value("error").toObject().value("message").toString());
    }


    if (message.type() == QJsonRpcMessage::Response)
    {
        QJsonObject jsonObject = message.toObject();

        if (jsonObject.contains("result"))
        {
            updateInvoices();
        }
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

InvoiceTypes::InvoiceStatus Invoice::status() const
{
    return m_status;
}

void Invoice::setStatus(const InvoiceTypes::InvoiceStatus &status)
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

QString Invoice::bolt11() const
{
    return m_bolt11;
}

void Invoice::setBolt11(const QString &bolt11)
{
    m_bolt11 = bolt11;
}

QString Invoice::statusString() const
{
    return m_statusString;
}

void Invoice::setStatusString(const QString &statusString)
{
    m_statusString = statusString;
}
