#ifndef LIGHTNINGMODEL_H
#define LIGHTNINGMODEL_H

#include <QLocalSocket>
#include <QTimer>
#include <QProcess>

#include "PeersModel.h"
#include "PaymentsModel.h"
#include "WalletModel.h"
#include "InvoicesModel.h"

#include "NodesModel.h"

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

    Q_PROPERTY(bool connectedToDaemon READ connectedToDaemon WRITE setConnectedToDaemon NOTIFY infoChanged)

    Q_PROPERTY(QString serverName READ serverName WRITE setServerName NOTIFY serverNameChanged)

public:
    LightningModel(QString serverName = QString(""), QObject *parent = 0);

    static LightningModel* instance();

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

    bool connectedToDaemon() const;

    QString bitcoinRpcServerName() const;
    void setBitcoinRpcServerName(const QString &bitcoinRpcServerName);

    QString bitcoinRpcUser() const;
    void setBitcoinRpcUser(const QString &bitcoinRpcUser);

    QString bitcoinRpcPassword() const;
    void setBitcoinRpcPassword(const QString &bitcoinRpcPassword);

    QString bitcoinCliPath() const;
    void setBitcoinCliPath(const QString &bitcoinCliPath);

    QString bitcoinDataPath() const;
    void setBitcoinDataPath(const QString &bitcoinDataPath);

    QString manualAddress() const;
    void setManualAddress(const QString &manualAddress);

private:
    static LightningModel *sInstance;

public slots:
    void startAutopilot(int amountSatoshi);

private:
    void updateInfo();
    void launchDaemon();
    void retryRpcConnection();
    void setConnectedToDaemon(bool connectedToDaemon);

private:
    QLocalSocket* m_unixSocket;
    QJsonRpcSocket* m_rpcSocket;
    QList<QJsonRpcServiceReply*> m_repliesList;
    PeersModel* m_peersModel;
    PaymentsModel* m_paymentsModel;
    WalletModel* m_walletModel;
    InvoicesModel* m_invoicesModel;

    NodesModel* m_nodesModel;

    QTimer* m_updatesTimer;

    QString m_lightningRpcSocket;
    QTimer* m_connectionRetryTimer;
    bool m_connectedToDaemon;

    QProcess* m_lightningDaemonProcess;

    QString m_id;
    int m_port;
    QString m_address;
    QString m_version;
    int m_blockheight;
    QString m_network;

    QString m_manualAddress;

    QString m_bitcoinRpcServerName;
    QString m_bitcoinRpcUser;
    QString m_bitcoinRpcPassword;

    QString m_bitcoinCliPath;
    QString m_bitcoinDataPath;

    int m_autopilotChannelAmount;
    QString m_autopilotPeerId;

    bool m_firstStart;


private slots:
    void rpcConnected();
    void rpcMessageReceived(QJsonRpcMessage message);
    void unixSocketError(QLocalSocket::LocalSocketError unixSocketError);
    void unixSocketDisconnected();
    void updateModels();
    void updateInfoRequestFinished();
    void lightningProcessFinished(int exitCode);

signals:
    void infoChanged();
    void serverNameChanged();
    void errorString(QString error);
    void rpcConnectionError();

};

#endif // LIGHTNINGMODEL_H
