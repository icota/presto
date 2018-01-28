#ifndef INVOICESMODEL_H
#define INVOICESMODEL_H

#include <QObject>
#include <QAbstractItemModel>

#include "qjsonrpc/qjsonrpcsocket.h"
#include "qjsonrpc/qjsonrpcmessage.h"

class Invoice
{
public:
    Invoice();

    enum InvoiceStatus {
        UNPAID,
        PAID,
        EXPIRED
    };


    QString label() const;
    void setLabel(const QString &label);

    QString hash() const;
    void setHash(const QString &hash);

    int msatoshi() const;
    void setMsatoshi(int msatoshi);

    InvoiceStatus status() const;
    void setStatus(const InvoiceStatus &status);

    int payIndex() const;
    void setPayIndex(int payIndex);

    int msatoshiReceived() const;
    void setMsatoshiReceived(int msatoshiReceived);

    int paidTimestamp() const;
    void setPaidTimestamp(int paidTimestamp);

    int paidAtTimestamp() const;
    void setPaidAtTimestamp(int paidAtTimestamp);

    int expiryTime() const;
    void setExpiryTime(int expiryTime);

    int expiresAtTime() const;
    void setExpiresAtTime(int expiresAtTime);

private:
    QString m_label;
    QString m_hash;
    int m_msatoshi;
    InvoiceStatus m_status;
    int m_payIndex;
    int m_msatoshiReceived;
    int m_paidTimestamp;
    int m_paidAtTimestamp;
    int m_expiryTime;
    int m_expiresAtTime;
};

class InvoicesModel : public QAbstractListModel
{
    Q_OBJECT
public:
    enum InvoiceRoles {
        LabelRole = Qt::UserRole + 1,
        HashRole,
        MSatoshiRole,
        StatusRole,
        PayIndexRole,
        MSatishiReceivedRole,
        PaidTimestampRole,
        PaidAtTimestampRole,
        ExpiryTimeRole,
        ExpiresAtRole
    };

    InvoicesModel(QJsonRpcSocket* rpcSocket = 0);

    QHash<int, QByteArray> roleNames() const;

    int rowCount(const QModelIndex & parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;

signals:

private slots:
    void listInvoicesRequestFinished();

private:
    void fetchInvoices();
    void populateInvoicesFromJson(QJsonArray jsonArray);

private:
    QList<Invoice> m_invoices;
    QJsonRpcSocket* m_rpcSocket;

public slots:
};

#endif // INVOICESMODEL_H
