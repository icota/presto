#ifndef LIGHTNINGMODEL_H
#define LIGHTNINGMODEL_H

#include <QLocalSocket>
#include <QTimer>
#include <QProcess>

#include "PeersModel.h"
#include "PaymentsModel.h"
#include "WalletModel.h"
#include "InvoicesModel.h"

#include "./3rdparty/qjsonrpc/src/qjsonrpcsocket.h"
#include "./3rdparty/qjsonrpc/src/qjsonrpcmessage.h"


class LightningModel : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString id READ id NOTIFY infoChanged)
    Q_PROPERTY(int port READ port NOTIFY infoChanged)
    Q_PROPERTY(QString address READ address NOTIFY infoChanged)
    Q_PROPERTY(QString version READ version NOTIFY infoChanged)
    Q_PROPERTY(int blockheight READ blockheight NOTIFY infoChanged)
    Q_PROPERTY(QString network READ network NOTIFY infoChanged)

    Q_PROPERTY(QString serverName READ serverName WRITE setServerName NOTIFY serverNameChanged)

public:
    LightningModel(QString serverName = QString(""), QObject *parent = 0);

    PeersModel *peersModel() const;
    PaymentsModel *paymentsModel() const;
    WalletModel *walletModel() const;
    InvoicesModel *invoicesModel() const;

    QString id() const;
    int port() const;
    QString address() const;
    QString version() const;
    int blockheight() const;
    QString network() const;

    QString serverName() const;
    void setServerName(const QString &serverName);

private:
    void updateInfo();
    void launchDaemon();
    void rpcNotConnected();

private:
    QLocalSocket* m_unixSocket;
    QJsonRpcSocket* m_rpcSocket;
    QList<QJsonRpcServiceReply*> m_repliesList;
    PeersModel* m_peersModel;
    PaymentsModel* m_paymentsModel;
    WalletModel* m_walletModel;
    InvoicesModel* m_invoicesModel;
    QTimer* m_updatesTimer;

    QString m_serverName;

    QProcess* m_lightningDaemonProcess;

    QString m_id;
    int m_port;
    QString m_address;
    QString m_version;
    int m_blockheight;
    QString m_network;

private slots:
    void rpcConnected();
    void rpcMessageReceived(QJsonRpcMessage message);
    void unixSocketError(QLocalSocket::LocalSocketError unixSocketError);
    void updateModels();
    void updateInfoRequestFinished();

signals:
    void infoChanged();
    void serverNameChanged();

};

#endif // LIGHTNINGMODEL_H
