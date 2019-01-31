#ifndef WALLETMODEL_H
#define WALLETMODEL_H

#include <QObject>
#include <QAbstractItemModel>

class FundsTransaction
{
public:
    FundsTransaction()
    {}

    QString txId() const;
    void setTxId(const QString &txId);

    int outputs() const;
    void setOutputs(int outputs);

    int amountSatoshi() const;
    void setAmountSatoshi(int amountSatoshi);

private:
    QString m_txId;
    int m_outputs;
    int m_amountSatoshi;

};

class WalletModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(int totalAvailableFunds READ totalAvailableFunds NOTIFY totalAvailableFundsChanged)
public:

    enum FundingRoles {
        TxidRole = Qt::UserRole + 1,
        OutputRole,
        SatoshiRole
    };

    WalletModel();

    QHash<int, QByteArray> roleNames() const;

    int rowCount(const QModelIndex & parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;

    /*Q_INVOKABLE*/ int totalAvailableFunds();

    void updateFunds();

signals:
    void totalAvailableFundsChanged();
    void newAddress(QString newAddress);
    void errorString(QString error);

public slots:
    void requestNewAddress();
    void withdrawFunds(QString destinationAddress, QString amountInSatoshi);

private slots:
    void listFundsRequestFinished();
    void newAddressRequestFinished();
    void withdrawFundsRequestFinished();

private:
    QList<FundsTransaction> m_funds;

private:
    void populateFundsFromJson(QJsonArray jsonArray);
};

#endif // WALLETMODEL_H
