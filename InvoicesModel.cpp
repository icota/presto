#include "InvoicesModel.h"

InvoicesModel::InvoicesModel(QObject *parent) : QObject(parent)
{

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
