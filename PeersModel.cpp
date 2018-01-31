#include "PeersModel.h"
#include "qjsonrpc/qjsonrpcservicereply.h"

QHash<int, QByteArray> PeersModel::roleNames() const {
    QHash<int, QByteArray> roles;
    roles[ChannelRole] = "channel";
    roles[ConnectedRole] = "connected";
    roles[MSatoshiToUsRole] = "msatoshitous";
    roles[MSatoshiTotalRole] = "msatoshitotal";
    roles[NetAddressRole] = "netaddress";
    roles[PeerIdRole] = "peerid";
    roles[PeerStateRole] = "peerstate";
    return roles;
}

PeersModel::PeersModel(QJsonRpcSocket *rpcSocket)
{
    m_rpcSocket = rpcSocket;
    fetchPeers();
}

void PeersModel::fetchPeers()
{
    QJsonRpcMessage message = QJsonRpcMessage::createRequest("listpeers", QJsonValue());
    QJsonRpcServiceReply* reply = m_rpcSocket->sendMessage(message);
    QObject::connect(reply, SIGNAL(finished()), this, SLOT(listPeersRequestFinished()));
}

void PeersModel::listPeersRequestFinished()
{
    QJsonRpcServiceReply *reply = static_cast<QJsonRpcServiceReply *>(sender());
    QJsonRpcMessage message = reply->response();

    if (message.type() == QJsonRpcMessage::Response)
    {
        QJsonObject jsonObject = message.toObject();

        QJsonArray peersArray = jsonObject.value("result").toObject().value("peers").toArray();
        if (peersArray.count() > 0)
        {
            populatePeersFromJson(peersArray);
        }
    }
}

void PeersModel::connectToPeer(QString peerId, QString peerAddress)
{
    QJsonObject paramsObject;
    paramsObject.insert("id", peerId);
    paramsObject.insert("host", peerAddress);

    QJsonRpcMessage message = QJsonRpcMessage::createRequest("connect", paramsObject);
    QJsonRpcServiceReply* reply = m_rpcSocket->sendMessage(message);
    QObject::connect(reply, SIGNAL(finished()), this, SLOT(connectToPeerRequestFinished()));
}

void PeersModel::connectToPeerRequestFinished()
{
    QJsonRpcServiceReply *reply = static_cast<QJsonRpcServiceReply *>(sender());
    QJsonRpcMessage message = reply->response();

    if (message.type() == QJsonRpcMessage::Response)
    {
        QJsonObject jsonObject = message.toObject();

        if (jsonObject.contains("result"))
        {
            QJsonObject resultObject = jsonObject.value("result").toObject();
            if (resultObject.contains("id"))
            {
                // TODO: notify GUI that we've connected
                fetchPeers();
            }
        }
    }
}

void PeersModel::fundChannel(QString peerId, QString amountInSatoshi)
{
    QJsonObject paramsObject;
    paramsObject.insert("id", peerId);
    paramsObject.insert("satoshi", amountInSatoshi);

    QJsonRpcMessage message = QJsonRpcMessage::createRequest("fundchannel", paramsObject);
    QJsonRpcServiceReply* reply = m_rpcSocket->sendMessage(message);
    QObject::connect(reply, SIGNAL(finished()), this, SLOT(fundChannelRequestFinished()));
}

void PeersModel::fundChannelRequestFinished()
{
    QJsonRpcServiceReply *reply = static_cast<QJsonRpcServiceReply *>(sender());
    QJsonRpcMessage message = reply->response();

    if (message.type() == QJsonRpcMessage::Error)
    {
        emit errorString(message.toObject().value("error").toObject().value("message").toString());
        // show the error to user
    }

    if (message.type() == QJsonRpcMessage::Response)
    {
        QJsonObject jsonObject = message.toObject();

        if (jsonObject.contains("result"))
        {
            QJsonObject resultObject = jsonObject.value("result").toObject();
            if (resultObject.contains("id"))
            {
                // TODO: notify GUI that we've connected
                fetchPeers();
            }
        }
    }
}

int PeersModel::rowCount(const QModelIndex & parent) const
{
    Q_UNUSED(parent);
    return m_peers.count();
}

QVariant PeersModel::data(const QModelIndex &index, int role) const
{
    if (index.row() < 0 || index.row() >= m_peers.count())
        return QVariant();

    const Peer &peer = m_peers[index.row()];
    if (role == ChannelRole)
        return peer.channel();
    else if (role == ConnectedRole)
        return peer.connected();
    else if (role == MSatoshiToUsRole)
        return peer.msatoshiToUs();
    else if (role == MSatoshiTotalRole)
        return peer.msatoshiTotal();
    else if (role == NetAddressRole)
        return peer.netAddress();
    else if (role == PeerIdRole)
        return peer.id();
    else if (role == PeerStateRole)
        return peer.state();
    return QVariant();
}

void PeersModel::populatePeersFromJson(QJsonArray jsonArray)
{
    foreach (const QJsonValue &v, jsonArray)
    {
        beginInsertRows(QModelIndex(), rowCount(), rowCount());

        QJsonObject PeerJsonObject = v.toObject(); // TODO: Fix

        Peer peer;
        peer.setChannel(PeerJsonObject.value("channel").toString());
        peer.setConnected(PeerJsonObject.value("connected").toBool());
        peer.setMsatoshiToUs(PeerJsonObject.value("msatoshi_to_us").toInt());
        peer.setMsatoshiTotal(PeerJsonObject.value("msatoshi_total").toInt());
        peer.setNetAddress(PeerJsonObject.value("netaddr").toArray()[0].toString()); // TODO: Figure out why addresses are in an array
        peer.setId(PeerJsonObject.value("id").toString());

        QString state = PeerJsonObject.value("state").toString();

        if (state == "GOSSIPING") peer.setState(Peer::PeerState::UNINITIALIZED);
        else if (state == "WHATEVER") peer.setState(Peer::PeerState::UNINITIALIZED);


        peer.setState((Peer::PeerState)PeerJsonObject.value("state").toInt()); // TODO: Fix this
        m_peers.append(peer);

        endInsertRows();
    }
}

QString Peer::channel() const
{
    return m_channel;
}

void Peer::setChannel(const QString &channel)
{
    m_channel = channel;
}

bool Peer::connected() const
{
    return m_connected;
}

void Peer::setConnected(bool connected)
{
    m_connected = connected;
}

int Peer::msatoshiToUs() const
{
    return m_msatoshiToUs;
}

void Peer::setMsatoshiToUs(int msatoshiToUs)
{
    m_msatoshiToUs = msatoshiToUs;
}

int Peer::msatoshiTotal() const
{
    return m_msatoshiTotal;
}

void Peer::setMsatoshiTotal(int msatoshiTotal)
{
    m_msatoshiTotal = msatoshiTotal;
}

QString Peer::netAddress() const
{
    return m_netAddress;
}

void Peer::setNetAddress(const QString &netAddress)
{
    m_netAddress = netAddress;
}

QString Peer::id() const
{
    return m_id;
}

void Peer::setId(const QString &id)
{
    m_id = id;
}

Peer::PeerState Peer::state() const
{
    return m_state;
}

void Peer::setState(const PeerState &state)
{
    m_state = state;
}

