#include "PaymentsModel.h"

QHash<int, QByteArray> PaymentsModel::roleNames() const {
    QHash<int, QByteArray> roles;
    roles[HashRole] = "hash";
    roles[IncomingRole] = "incoming";
    roles[MSatoshiRole] = "msatoshi";
    roles[TimestampRole] = "timestamp";
    roles[DestinationRole] = "destination";
    roles[PaymentIdRole] = "paymentid";
    roles[PaymentStatusRole] = "paymentstate";
    return roles;
}

PaymentsModel::PaymentsModel(QJsonRpcSocket *rpcSocket)
{
    m_rpcSocket = rpcSocket;
    fetchPayments();
}

void PaymentsModel::fetchPayments()
{
    QJsonRpcMessage message = QJsonRpcMessage::createRequest("getPayments", QJsonValue());
    /*QJsonRpcServiceReply* reply = */m_rpcSocket->sendMessage(message);
}

int PaymentsModel::rowCount(const QModelIndex & parent) const
{
    Q_UNUSED(parent);
    return m_payments.count();
}

QVariant PaymentsModel::data(const QModelIndex &index, int role) const
{
    if (index.row() < 0 || index.row() >= m_payments.count())
        return QVariant();

    const Payment &Payment = m_payments[index.row()];
    if (role == HashRole)
        return Payment.hash();
    else if (role == IncomingRole)
        return Payment.incoming();
    else if (role == MSatoshiRole)
        return Payment.msatoshi();
    else if (role == TimestampRole)
        return Payment.timestamp();
    else if (role == DestinationRole)
        return Payment.destination();
    else if (role == PaymentIdRole)
        return Payment.id();
    else if (role == PaymentStatusRole)
        return Payment.status();
    return QVariant();
}

void PaymentsModel::populatePaymentsFromJson(QJsonObject jsonObject)
{
    foreach (const QJsonValue &v, jsonObject)
    {
        beginInsertRows(QModelIndex(), rowCount(), rowCount());

        QJsonObject PaymentJsonObject = v.toArray()[0].toObject(); // TODO: Fix
        Payment Payment;
        Payment.setId(PaymentJsonObject.value("channel").toString());
        Payment.setIncoming(PaymentJsonObject.value("connected").toBool());
        Payment.setMsatoshi(PaymentJsonObject.value("msatoshi_to_us").toInt());
        Payment.setTimestamp(PaymentJsonObject.value("msatoshi_total").toInt());
        Payment.setDestination(PaymentJsonObject.value("netaddr").toString()); // TODO: Figure out why addresses are in an array
        Payment.setHash(PaymentJsonObject.value("Paymentid").toString());
        Payment.setStatus((Payment::PaymentStatus)PaymentJsonObject.value("state").toInt()); // TODO: Fix this
        m_payments.append(Payment);

        endInsertRows();
    }
}

QString Payment::hash() const
{
    return m_hash;
}

void Payment::setHash(const QString &hash)
{
    m_hash = hash;
}

bool Payment::incoming() const
{
    return m_incoming;
}

void Payment::setIncoming(bool incoming)
{
    m_incoming = incoming;
}

int Payment::msatoshi() const
{
    return m_msatoshi;
}

void Payment::setMsatoshi(int msatoshi)
{
    m_msatoshi = msatoshi;
}

int Payment::timestamp() const
{
    return m_timestamp;
}

void Payment::setTimestamp(int timestamp)
{
    m_timestamp = timestamp;
}

QString Payment::destination() const
{
    return m_destination;
}

void Payment::setDestination(const QString &destination)
{
    m_destination = destination;
}

QString Payment::id() const
{
    return m_id;
}

void Payment::setId(const QString &id)
{
    m_id = id;
}

Payment::PaymentStatus Payment::status() const
{
    return m_status;
}

void Payment::setStatus(const PaymentStatus &status)
{
    m_status = status;
}
