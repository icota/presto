#include "PeersModel.h"

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
    QJsonRpcMessage message = QJsonRpcMessage::createRequest("getpeers", QJsonValue());
    /*QJsonRpcServiceReply* reply = */m_rpcSocket->sendMessage(message);
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

    const Peer &Peer = m_peers[index.row()];
    if (role == ChannelRole)
        return Peer.channel();
    else if (role == ConnectedRole)
        return Peer.connected();
    else if (role == MSatoshiToUsRole)
        return Peer.msatoshiToUs();
    else if (role == MSatoshiTotalRole)
        return Peer.msatoshiTotal();
    else if (role == NetAddressRole)
        return Peer.netAddress();
    else if (role == PeerIdRole)
        return Peer.id();
    else if (role == PeerStateRole)
        return Peer.state();
    return QVariant();
}

void PeersModel::populatePeersFromJson(QJsonObject jsonObject)
{
    foreach (const QJsonValue &v, jsonObject)
    {
        beginInsertRows(QModelIndex(), rowCount(), rowCount());

        QJsonObject PeerJsonObject = v.toArray()[0].toObject(); // TODO: Fix
        Peer peer;
        peer.setChannel(PeerJsonObject.value("channel").toString());
        peer.setConnected(PeerJsonObject.value("connected").toBool());
        peer.setMsatoshiToUs(PeerJsonObject.value("msatoshi_to_us").toInt());
        peer.setMsatoshiTotal(PeerJsonObject.value("msatoshi_total").toInt());
        peer.setNetAddress(PeerJsonObject.value("netaddr").toArray()[0].toString()); // TODO: Figure out why addresses are in an array
        peer.setId(PeerJsonObject.value("peerid").toString());
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

