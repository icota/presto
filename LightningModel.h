#ifndef LIGHTNINGMODEL_H
#define LIGHTNINGMODEL_H

#include <QLocalSocket>

#include "PeersModel.h"
#include "PaymentsModel.h"
#include "WalletModel.h"

#include "qjsonrpc/qjsonrpcsocket.h"
#include "qjsonrpc/qjsonrpcmessage.h"


class LightningModel : public QObject
{
    Q_OBJECT
public:
    LightningModel(QObject *parent = 0);
    PeersModel *peersModel() const;
    PaymentsModel *paymentsModel() const;
    WalletModel *walletModel() const;

private:
    QLocalSocket* m_unixSocket;
    QJsonRpcSocket* m_rpcSocket;
    QList<QJsonRpcServiceReply*> m_repliesList;
    PeersModel* m_peersModel;
    PaymentsModel* m_paymentsModel;
    WalletModel* m_walletModel;

private slots:
    void rpcConnected();
    void rpcMessageReceived(QJsonRpcMessage message);
    void unixSocketError(QLocalSocket::LocalSocketError unixSocketError);

};

#endif // LIGHTNINGMODEL_H
