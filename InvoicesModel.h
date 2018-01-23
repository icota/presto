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

class InvoicesModel : public QObject
{
    Q_OBJECT
public:
    explicit InvoicesModel(QObject *parent = nullptr);

signals:

public slots:
};

#endif // INVOICESMODEL_H
