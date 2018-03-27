#include "PaymentsModel.h"
#include "./3rdparty/qjsonrpc/src/qjsonrpcservicereply.h"
#include "macros.h"

QHash<int, QByteArray> PaymentsModel::roleNames() const {
    QHash<int, QByteArray> roles;
    roles[HashRole] = "hash";
    roles[IncomingRole] = "incoming";
    roles[MSatoshiRole] = "msatoshi";
    roles[TimestampRole] = "timestamp";
    roles[DestinationRole] = "destination";
    roles[PaymentIdRole] = "paymentid";
    roles[PaymentStatusRole] = "paymentstatus";
    roles[PaymentStatusStringRole] = "paymentstatusstring";
    return roles;
}

PaymentsModel::PaymentsModel(QJsonRpcSocket *rpcSocket)
{
    m_rpcSocket = rpcSocket;
    m_payments = QList<Payment>();

    setMaxFeePercent(100);
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

    const Payment &payment = m_payments[index.row()];
    if (role == HashRole)
        return payment.hash();
    else if (role == IncomingRole)
        return payment.incoming();
    else if (role == MSatoshiRole)
        return payment.msatoshi();
    else if (role == TimestampRole)
        return payment.timestamp();
    else if (role == DestinationRole)
        return payment.destination();
    else if (role == PaymentIdRole)
        return payment.id();
    else if (role == PaymentStatusRole)
        return payment.status();
    else if (role == PaymentStatusStringRole)
        return payment.statusString();
    return QVariant();
}

void PaymentsModel::updatePayments()
{
    QJsonRpcMessage message = QJsonRpcMessage::createRequest("listpayments", QJsonValue());
    SEND_MESSAGE_CONNECT_SLOT(message, &PaymentsModel::listPaymentsRequestFinished)
}

void PaymentsModel::listPaymentsRequestFinished()
{
    GET_MESSAGE_DISCONNECT_SLOT(message, &PaymentsModel::listPaymentsRequestFinished)
    if (message.type() == QJsonRpcMessage::Response)
    {
        QJsonObject jsonObject = message.toObject();

        if (jsonObject.contains("result"))
        {
            QJsonObject resultObject = jsonObject.value("result").toObject();
            populatePaymentsFromJson(resultObject.value("payments").toArray());
        }
    }
}

void PaymentsModel::decodePayment(QString bolt11String)
{
    m_lastBolt11DecodeAttempt = bolt11String;
    QJsonRpcMessage message = QJsonRpcMessage::createRequest("decodepay", QJsonValue(bolt11String));
    SEND_MESSAGE_CONNECT_SLOT(message, &PaymentsModel::decodePaymentRequestFinished)
}

void PaymentsModel::decodePaymentRequestFinished()
{
    GET_MESSAGE_DISCONNECT_SLOT(message, &PaymentsModel::decodePaymentRequestFinished)
    if (message.type() == QJsonRpcMessage::Error)
    {
        emit errorString(message.toObject().value("error").toObject().value("message").toString());
    }

    if (message.type() == QJsonRpcMessage::Response)
    {
        QJsonObject jsonObject = message.toObject();

        if (jsonObject.contains("result"))
        {
            QJsonObject resultObject = jsonObject.value("result").toObject();
            int createdAt = resultObject.value("created_at").toInt();
            QString currency = resultObject.value("currency").toString();
            QString description = resultObject.value("description").toString();
            int expiry = resultObject.value("expiry").toInt();
            int minFinalCltvExpiry = resultObject.value("min_final_cltv_expiry").toInt();

            int msatoshi = -1;
            if (resultObject.contains("msatoshi")) {
                msatoshi = resultObject.value("msatoshi").toInt();
            }

            QString payee = resultObject.value("payee").toString();
            QString paymentHash = resultObject.value("payment_hash").toString();
            QString signature = resultObject.value("signature").toString();
            int timestamp = resultObject.value("timestamp").toInt();

            emit paymentDecoded(createdAt, currency, description,
                                expiry, minFinalCltvExpiry, msatoshi,
                                payee, paymentHash, signature, timestamp, m_lastBolt11DecodeAttempt);
        }
    }
}

void PaymentsModel::pay(QString bolt11String, int msatoshiAmount)
{
    QJsonObject paramsObject;
    paramsObject.insert("bolt11", bolt11String);
    if (msatoshiAmount > 0) {
        paramsObject.insert("msatoshi", QString::number(msatoshiAmount));
    }
    paramsObject.insert("maxfeepercent", QString::number(m_maxFeePercent / 100));

    QJsonRpcMessage message = QJsonRpcMessage::createRequest("pay", paramsObject);
    SEND_MESSAGE_CONNECT_SLOT(message, &PaymentsModel::payRequestFinished)
}

void PaymentsModel::payRequestFinished()
{
    GET_MESSAGE_DISCONNECT_SLOT(message, &PaymentsModel::payRequestFinished)
    // save it somewhere
    QString bolt11 = reply->request().toObject().value("params").toObject().value("bolt11").toString();

    if (message.type() == QJsonRpcMessage::Error)
    {
        emit errorString(message.toObject().value("error").toObject().value("message").toString());
    }

    if (message.type() == QJsonRpcMessage::Response)
    {
        QJsonObject jsonObject = message.toObject();

        if (jsonObject.contains("result"))
        {
            QJsonObject resultObject = jsonObject.value("result").toObject();
            if (resultObject.contains("preimage"))
            {
                emit paymentPreimageReceived(resultObject.value("preimage").toString());
                updatePayments();
            }
        }
    }
}

void PaymentsModel::populatePaymentsFromJson(QJsonArray jsonArray)
{
    m_payments.clear();
    endResetModel();

    foreach (const QJsonValue &v, jsonArray)
    {
        beginInsertRows(QModelIndex(), rowCount(), rowCount());

        QJsonObject PaymentJsonObject = v.toObject();
        Payment payment;
        payment.setId(PaymentJsonObject.value("id").toString());
        payment.setIncoming(PaymentJsonObject.value("incoming").toBool());
        payment.setMsatoshi(PaymentJsonObject.value("msatoshi").toInt());
        payment.setTimestamp(PaymentJsonObject.value("timestamp").toInt());
        payment.setDestination(PaymentJsonObject.value("destination").toString()); // TODO: Figure out why addresses are in an array
        payment.setHash(PaymentJsonObject.value("payment_hash").toString());
        payment.setStatus((Payment::PaymentStatus)PaymentJsonObject.value("status").toInt()); // TODO: Fix this
        payment.setStatusString(PaymentJsonObject.value("status").toString());
        m_payments.append(payment);

        endInsertRows();
    }
}

int PaymentsModel::maxFeePercent() const
{
    return m_maxFeePercent;
}

void PaymentsModel::setMaxFeePercent(int maxFeePercent)
{
    m_maxFeePercent = maxFeePercent;
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

QString Payment::statusString() const
{
    return m_statusString;
}

void Payment::setStatusString(const QString &statusString)
{
    m_statusString = statusString;
}


