#ifndef PAYMENTSMODEL_H
#define PAYMENTSMODEL_H

#include <QObject>
#include <QAbstractItemModel>

#include "./3rdparty/qjsonrpc/src/qjsonrpcsocket.h"
#include "./3rdparty/qjsonrpc/src/qjsonrpcmessage.h"

class Payment
{
public:
    Payment()
    {}

    // Copied over from c-lightning
    enum PaymentStatus {
        PAYMENT_PENDING = 0,
        PAYMENT_COMPLETE = 1,
        PAYMENT_FAILED = 2
    };

    QString hash() const;
    void setHash(const QString &hash);

    bool incoming() const;
    void setIncoming(bool incoming);

    int msatoshi() const;
    void setMsatoshi(int msatoshi);

    int timestamp() const;
    void setTimestamp(int timestamp);

    QString destination() const;
    void setDestination(const QString &destination);

    QString id() const;
    void setId(const QString &id);

    PaymentStatus status() const;
    void setStatus(const PaymentStatus &status);

    QString statusString() const;
    void setStatusString(const QString &statusString);

private:
    QString m_hash;
    bool m_incoming;
    int m_msatoshi;
    int m_timestamp;
    QString m_destination;
    QString m_id;
    PaymentStatus m_status;
    QString m_statusString;
};

class PaymentsModel : public QAbstractListModel
{
    Q_OBJECT
public:
    enum PaymentRoles {
        HashRole = Qt::UserRole + 1,
        IncomingRole,
        MSatoshiRole,
        TimestampRole,
        DestinationRole,
        PaymentIdRole,
        PaymentStatusRole,
        PaymentStatusStringRole
    };

    PaymentsModel(QJsonRpcSocket* rpcSocket = 0);

    QHash<int, QByteArray> roleNames() const;

    int rowCount(const QModelIndex & parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;

    void updatePayments();

    int maxFeePercent() const;
    void setMaxFeePercent(int maxFeePercent);

public slots:
    void decodePayment(QString bolt11String);
    void pay(QString bolt11String);

private slots:
    void listPaymentsRequestFinished();
    void decodePaymentRequestFinished();
    void payRequestFinished();

signals:
    void paymentDecoded(int createdAt, QString currency, QString description,
                        int expiry, int minFinalCltvExpiry, int msatoshi,
                        QString payee, QString paymentHash, QString signature, int timestamp);

    void paymentPreimageReceived(QString preimage);
    void errorString(QString error);

private:
    void populatePaymentsFromJson(QJsonArray jsonObject);

private:
    QList<Payment> m_payments;
    QJsonRpcSocket* m_rpcSocket;

    int m_maxFeePercent;
};

#endif // PAYMENTSMODEL_H
