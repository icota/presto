#ifndef PAYMENTSMODEL_H
#define PAYMENTSMODEL_H

#include <QObject>
#include <QAbstractItemModel>

#include "qjsonrpc/qjsonrpcsocket.h"
#include "qjsonrpc/qjsonrpcmessage.h"

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

private:
    QString m_hash;
    bool m_incoming;
    int m_msatoshi;
    int m_timestamp;
    QString m_destination;
    QString m_id;
    PaymentStatus m_status;
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
        PaymentStatusRole
    };

    QHash<int, QByteArray> roleNames() const;


    PaymentsModel(QJsonRpcSocket* rpcSocket = 0);
    void populatePaymentsFromJson(QJsonArray jsonObject);

    int rowCount(const QModelIndex & parent = QModelIndex()) const;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;

private slots:
    void requestFinished();

private:
    void fetchPayments();

private:
    QList<Payment> m_payments;
    QJsonRpcSocket* m_rpcSocket;
};

#endif // PAYMENTSMODEL_H