#include <QDir>
#include <QSettings>

#include "LightningModel.h"
#include "./3rdparty/qjsonrpc/src/qjsonrpcservicereply.h"

PeersModel *LightningModel::peersModel() const
{
    return m_peersModel;
}

PaymentsModel *LightningModel::paymentsModel() const
{
    return m_paymentsModel;
}

WalletModel *LightningModel::walletModel() const
{
    return m_walletModel;
}

InvoicesModel *LightningModel::invoicesModel() const
{
    return m_invoicesModel;
}

QString LightningModel::id() const
{
    return m_id;
}

int LightningModel::port() const
{
    return m_port;
}

QString LightningModel::address() const
{
    return m_address;
}

QString LightningModel::version() const
{
    return m_version;
}

int LightningModel::blockheight() const
{
    return m_blockheight;
}

QString LightningModel::network() const
{
    return m_network;
}

void LightningModel::updateInfo()
{
    QJsonRpcMessage message = QJsonRpcMessage::createRequest("getinfo", QJsonValue());
    QJsonRpcServiceReply* reply = m_rpcSocket->sendMessage(message);
    QObject::connect(reply, &QJsonRpcServiceReply::finished, this, &LightningModel::updateInfoRequestFinished);
}

void LightningModel::launchDaemon()
{
#ifdef Q_OS_ANDROID
    QDir::home().mkdir("bitcoin-data");
    QDir::home().mkdir("libexec");

    QDir libexecDir(QDir::homePath() + "/libexec");
    libexecDir.mkdir("c-lightning");

    QDir cLightningDir(libexecDir.absolutePath() + "/c-lightning");

    QDir programDir = QDir::home();
    programDir.cdUp();

    if (m_firstStart) {
        QStringList lightningBinaries;

        lightningBinaries << "lightning_channeld"
                          << "lightning_closingd"
                          << "lightning_gossipd"
                          << "lightning_hsmd"
                          << "lightning_onchaind"
                          << "lightning_openingd"
                          << "lightningd";

        foreach (QString binaryName, lightningBinaries) {
            QFile::copy(programDir.absolutePath() + "/lib/lib" + binaryName + ".so",
                        cLightningDir.absolutePath() + "/" + binaryName);
        }

        m_firstStart = false;
    }

    QString program = cLightningDir.absolutePath() + "/lightningd";
#else
    QDir programDir = QDir::home();
    QString program = programDir.absolutePath() + "/Code/lightning/lightningd/lightningd"; // Hardcode some location within a snap?
#endif

    qDebug() << "Starting: " << program;
    QStringList arguments;
    arguments << "--network=testnet"
              << "--log-level=debug";

#ifdef Q_OS_ANDROID
    arguments << "--lightning-dir=" + QDir::homePath() + "/lightning-data"
              << "--pid-file=" + QDir::homePath() + "/lightning.pid"
              << "--bitcoin-cli=" + programDir.absolutePath() + "/lib/libbitcoin-cli.so"
              << "--bitcoin-datadir=" + QDir::homePath() + "/bitcoin-data"
              << "--rpc-file=" + QDir::homePath() + "/lightning-rpc";
#else
    if (!m_bitcoinCliPath.isEmpty())
    {
        arguments << "--bitcoin-cli=" + m_bitcoinCliPath;
    }

    if (!m_bitcoinDataPath.isEmpty())
    {
        arguments << "--bitcoin-datadir=" + m_bitcoinDataPath;
    }
#endif

    if (!m_bitcoinRpcServerName.isEmpty())
    {
        arguments << "--bitcoin-rpcconnect=" + m_bitcoinRpcServerName
                  << "--bitcoin-rpcuser=" + m_bitcoinRpcUser
                  << "--bitcoin-rpcpassword=" + m_bitcoinRpcPassword;
    }

    qDebug() << "Arguments: " << arguments;

    m_lightningDaemonProcess->start(program, arguments);
    if (m_lightningDaemonProcess->waitForStarted())
    {
        qDebug()<<"Daemon started";
        retryRpcConnection();
    }
    else
    {
        qDebug() << "Couldn't start daemon: " << m_lightningDaemonProcess->error();
    }
}

QString LightningModel::serverName() const
{
    return m_lightningRpcSocket;
}

void LightningModel::setServerName(const QString &serverName)
{
    m_lightningRpcSocket = serverName;
}

void LightningModel::updateInfoRequestFinished()
{
    QJsonRpcServiceReply *reply = static_cast<QJsonRpcServiceReply *>(sender());
    QJsonRpcMessage message = reply->response();

    if (message.type() == QJsonRpcMessage::Error)
    {
        //emit errorString(message.toObject().value("error").toObject().value("message").toString());
    }

    if (message.type() == QJsonRpcMessage::Response)
    {
        QJsonObject resultsObject = message.toObject().value("result").toObject();

        QJsonArray addressesArray = resultsObject.value("address").toArray();


        m_address = addressesArray[0].toObject().value("address").toString();
        m_blockheight = resultsObject.value("blockheight").toInt();
        m_id = resultsObject.value("id").toString();
        m_network = resultsObject.value("network").toString();
        m_port = resultsObject.value("port").toInt();
        m_version = resultsObject.value("version").toString();

        emit infoChanged();
    }
}

void LightningModel::lightningProcessFinished(int exitCode)
{
    qDebug() << "Lightning daemon finished, exit code: " << exitCode;
    //qDebug() << m_lightningDaemonProcess->readAllStandardOutput();
    QString stdError = m_lightningDaemonProcess->readAllStandardError();
    if (stdError.contains("Could not locate RPC credentials")) {
        emit errorString("Wrong RPC Credentials"); // Fixme: handle this properly
        emit rpcConnectionError();
    }
    qDebug() << stdError;
}

LightningModel::LightningModel(QString serverName, QObject *parent) {
    {
        Q_UNUSED(parent);

        if (serverName.isEmpty()) {
#ifdef Q_OS_ANDROID
            m_lightningRpcSocket = QDir::homePath() + "/lightning-rpc";
#else
            m_lightningRpcSocket = QDir::homePath() + "/.lightning/lightning-rpc";
#endif
        }
        else {
            m_lightningRpcSocket = serverName;
        }

        m_firstStart = true;

        QSettings settings;

        m_bitcoinRpcServerName = settings.value("nodeAddress").toString();
        m_bitcoinRpcUser = settings.value("nodeRpcUsername").toString();
        m_bitcoinRpcPassword = settings.value("nodeRpcPassword").toString();

        m_lightningDaemonProcess = new QProcess(this);
        QObject::connect(m_lightningDaemonProcess, SIGNAL(finished(int)),
                         this, SLOT(lightningProcessFinished(int)));

        m_connectedToDaemon = false;

        m_address = QString();
        m_blockheight = 0;
        m_id = QString();
        m_network = QString();
        m_port = 0;
        m_version = QString();

        m_connectionRetryTimer = new QTimer();
        m_unixSocket = new QLocalSocket();
        m_rpcSocket = new QJsonRpcSocket(m_unixSocket);

        m_peersModel = new PeersModel(m_rpcSocket);
        m_paymentsModel = new PaymentsModel(m_rpcSocket);
        m_walletModel = new WalletModel(m_rpcSocket);
        m_invoicesModel = new InvoicesModel(m_rpcSocket);

        m_nodesModel = new NodesModel(m_rpcSocket);

        QObject::connect(m_unixSocket, SIGNAL(error(QLocalSocket::LocalSocketError)),
                         this, SLOT(unixSocketError(QLocalSocket::LocalSocketError)));

        QObject::connect(m_rpcSocket, &QJsonRpcAbstractSocket::messageReceived, this, &LightningModel::rpcMessageReceived);

        m_unixSocket->connectToServer(m_lightningRpcSocket);

        if (m_unixSocket->waitForConnected())
        {
            rpcConnected();
        }
        else
        {
            // No daemon so lets launch our own
            launchDaemon();
        }

    }
}

void LightningModel::rpcConnected()
{
    m_connectionRetryTimer->stop();
    m_connectedToDaemon = true;

    updateModels();

    // Don't update the nodes all the time
    m_nodesModel->updateNodes();

    m_updatesTimer = new QTimer();

    // Update the models every 15 secs
    m_updatesTimer->setInterval(15000);
    m_updatesTimer->setSingleShot(false);
    QObject::connect(m_updatesTimer, &QTimer::timeout, this, &LightningModel::updateModels);
    m_updatesTimer->start();
}

void LightningModel::retryRpcConnection()
{
    m_connectionRetryTimer->stop();

    // Let's retry every 10 secs
    m_connectionRetryTimer->setInterval(10000);
    m_connectionRetryTimer->setSingleShot(true);

    // Reentrant slot right here
    QObject::connect(m_connectionRetryTimer, &QTimer::timeout, this, &LightningModel::retryRpcConnection);
    m_connectionRetryTimer->start();

    if (m_unixSocket->state() == QLocalSocket::UnconnectedState) {
        m_unixSocket->connectToServer(m_lightningRpcSocket);
        if (m_unixSocket->waitForConnected())
        {
            rpcConnected();
        }
    }
}

QString LightningModel::bitcoinDataPath() const
{
    return m_bitcoinDataPath;
}

void LightningModel::setBitcoinDataPath(const QString &bitcoinDataPath)
{
    m_bitcoinDataPath = bitcoinDataPath;
}

void LightningModel::startAutopilot(int amountSatoshi)
{
    m_autopilotChannelAmount = amountSatoshi;

    Node randomNode = m_nodesModel->getRandomAutoconnectNode();

    if (!randomNode.id().isEmpty()) {
        m_autopilotPeerId = randomNode.id();

        QObject::connect(m_peersModel, &PeersModel::connectedToPeer,
                         [=] (QString peerId) { if (peerId == m_autopilotPeerId)
                m_peersModel->fundChannel(peerId, QString::number(m_autopilotChannelAmount)); });

        m_peersModel->connectToPeer(m_autopilotPeerId, randomNode.nodeAddressList().at(0).address());
    }

    m_nodesModel->updateNodes();
}

QString LightningModel::bitcoinCliPath() const
{
    return m_bitcoinCliPath;
}

void LightningModel::setBitcoinCliPath(const QString &bitcoinCliPath)
{
    m_bitcoinCliPath = bitcoinCliPath;
}

QString LightningModel::bitcoinRpcPassword() const
{
    return m_bitcoinRpcPassword;
}

void LightningModel::setBitcoinRpcPassword(const QString &bitcoinRpcPassword)
{
    m_bitcoinRpcPassword = bitcoinRpcPassword;
}

QString LightningModel::bitcoinRpcUser() const
{
    return m_bitcoinRpcUser;
}

void LightningModel::setBitcoinRpcUser(const QString &bitcoinRpcUser)
{
    m_bitcoinRpcUser = bitcoinRpcUser;
}

QString LightningModel::bitcoinRpcServerName() const
{
    return m_bitcoinRpcServerName;
}

void LightningModel::setBitcoinRpcServerName(const QString &bitcoinRpcServerName)
{
    m_bitcoinRpcServerName = bitcoinRpcServerName;
}

bool LightningModel::connectedToDaemon() const
{
    return m_connectedToDaemon;
}

void LightningModel::rpcMessageReceived(QJsonRpcMessage message)
{
    //qDebug() << message.toJson();
}

void LightningModel::unixSocketError(QLocalSocket::LocalSocketError socketError)
{
    //qDebug() << "Couldn't connect to daemon: " << socketError;
    if (socketError != QLocalSocket::OperationError) {
        //launchDaemon();
    }
}

void LightningModel::updateModels()
{
    m_paymentsModel->updatePayments();
    m_walletModel->updateFunds();
    m_invoicesModel->updateInvoices();
    updateInfo();
    // This calls needs to go last cause it hates concurrency
    m_peersModel->updatePeers();
}
