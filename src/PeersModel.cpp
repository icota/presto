#include "PeersModel.h"
#include "./3rdparty/qjsonrpc/src/qjsonrpcservicereply.h"
#include "macros.h"

QHash<int, QByteArray> PeersModel::roleNames() const {
    QHash<int, QByteArray> roles;
    roles[ChannelRole] = "channel";
    roles[ConnectedRole] = "connected";
    roles[MSatoshiToUsRole] = "msatoshitous";
    roles[MSatoshiTotalRole] = "msatoshitotal";
    roles[NetAddressRole] = "netaddress";
    roles[PeerIdRole] = "peerid";
    roles[PeerStateRole] = "peerstate";
    roles[PeerStateStringRole] = "peerstatestring";
    return roles;
}

PeersModel::PeersModel(QJsonRpcSocket *rpcSocket)
{
    m_rpcSocket = rpcSocket;
    m_peers = QMap<QString, Peer>();
}

void PeersModel::updatePeers()
{
    QJsonRpcMessage message = QJsonRpcMessage::createRequest("listpeers", QJsonValue());
    SEND_MESSAGE_CONNECT_SLOT(message, &PeersModel::listPeersRequestFinished)
}

void PeersModel::listPeersRequestFinished()
{
    GET_MESSAGE_DISCONNECT_SLOT(message, &PeersModel::listPeersRequestFinished)
    if (message.type() == QJsonRpcMessage::Response)
    {
        QJsonObject jsonObject = message.toObject();

        QJsonArray peersArray = jsonObject.value("result").toObject().value("peers").toArray();
        populatePeersFromJson(peersArray);
    }
}

void PeersModel::connectToPeer(QString peerId, QString peerAddress)
{
    QJsonObject paramsObject;
    paramsObject.insert("id", peerId);
    paramsObject.insert("host", peerAddress);

    QJsonRpcMessage message = QJsonRpcMessage::createRequest("connect", paramsObject);
    SEND_MESSAGE_CONNECT_SLOT(message, &PeersModel::connectToPeerRequestFinished)
}

void PeersModel::connectToPeerRequestFinished()
{
    GET_MESSAGE_DISCONNECT_SLOT(message, &PeersModel::connectToPeerRequestFinished)
    if (message.type() == QJsonRpcMessage::Error)
    {
        emit errorString(message.toObject().value("error").toObject().value("message").toString());
    }

    if (message.type() == QJsonRpcMessage::Response)
    {
        QJsonObject jsonObject = message.toObject();

        if (jsonObject.contains("result"))
        {
            QJsonObject resultObject = jsonObject.value("result").toObject();
            if (resultObject.contains("id"))
            {
                updatePeers();
                emit connectedToPeer(resultObject.value("id").toString());
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
    SEND_MESSAGE_CONNECT_SLOT(message, &PeersModel::fundChannelRequestFinished)
}

void PeersModel::fundChannelRequestFinished()
{
    GET_MESSAGE_DISCONNECT_SLOT(message, &PeersModel::fundChannelRequestFinished)
    if (message.type() == QJsonRpcMessage::Error)
    {
        emit errorString(message.toObject().value("error").toObject().value("message").toString());
    }

    if (message.type() == QJsonRpcMessage::Response)
    {
        QJsonObject jsonObject = message.toObject();

        if (jsonObject.contains("result"))
        {
            QJsonObject resultObject = jsonObject.value("result").toObject();
            if (resultObject.contains("id"))
            {
                updatePeers();
                emit channelFunded(resultObject.value("id").toString());
            }
        }
    }
}

void PeersModel::closeChannel(QString peerId)
{
    QJsonObject paramsObject;
    paramsObject.insert("id", peerId);

    foreach (Peer peer, m_peers) {
        if (peer.id() == peerId) {
            if (peer.stateString().isEmpty()) {
                QJsonRpcMessage message = QJsonRpcMessage::createRequest("disconnect", paramsObject);
                SEND_MESSAGE_CONNECT_SLOT(message, &PeersModel::disconnectRequestFinished)
                return;
            }
        }
    }

    QJsonRpcMessage message = QJsonRpcMessage::createRequest("close", paramsObject);
    SEND_MESSAGE_CONNECT_SLOT(message, &PeersModel::closeChannelRequestFinished)
}

void PeersModel::closeChannelRequestFinished()
{
    GET_MESSAGE_DISCONNECT_SLOT(message, &PeersModel::closeChannelRequestFinished)
    if (message.type() == QJsonRpcMessage::Error)
    {
        emit errorString(message.toObject().value("error").toObject().value("message").toString());
    }

    if (message.type() == QJsonRpcMessage::Response)
    {
        QJsonObject jsonObject = message.toObject();

        if (jsonObject.contains("result"))
        {
            updatePeers();
        }
    }
}

void PeersModel::disconnectRequestFinished()
{
    GET_MESSAGE_DISCONNECT_SLOT(message, &PeersModel::disconnectRequestFinished)
    if (message.type() == QJsonRpcMessage::Error)
    {
        emit errorString(message.toObject().value("error").toObject().value("message").toString());
    }

    if (message.type() == QJsonRpcMessage::Response)
    {
        QJsonObject jsonObject = message.toObject();

        if (jsonObject.contains("result"))
        {
            updatePeers();
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

    const Peer &peer = m_peers[m_peers.keys()[index.row()]];
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
    else if (role == PeerStateStringRole)
        return peer.stateString();
    return QVariant();
}

void PeersModel::populatePeersFromJson(QJsonArray jsonArray)
{
    // Let's make a list of ids from JSON
    QStringList jsonIds;
    foreach (const QJsonValue &v, jsonArray) {
        QJsonObject peerJsonObject = v.toObject();
        jsonIds.append(peerJsonObject.value("id").toString());
    }

    // Remove the ones not in JSON
    foreach (QString id, m_peers.keys()) {
            if (!jsonIds.contains(id)) {
            // not there remove his
            beginRemoveRows(QModelIndex(),
                            m_peers.keys().indexOf(id),
                            m_peers.keys().indexOf(id));
            m_peers.remove(id);
            endRemoveRows();
        }
    }


    foreach (const QJsonValue &v, jsonArray)
    {
        QJsonObject peerJsonObject = v.toObject();

        Peer peer;

        peer.setId(peerJsonObject.value("id").toString());
        peer.setNetAddress(peerJsonObject.value("netaddr").toArray()[0].toString()); // TODO: Figure out why addresses are in an array
        peer.setConnected(peerJsonObject.value("connected").toBool());

        QJsonArray channelsJsonArray = peerJsonObject.value("channels").toArray();
        // FIX: Store the whole array somehow

        // FIX: msatoshi stuff probably needs to be a long rather than an int
        peer.setMsatoshiToUs(channelsJsonArray[0].toObject().value("msatoshi_to_us").toInt());
        peer.setMsatoshiTotal(peerJsonObject.value("msatoshi_total").toInt());

        QString state = channelsJsonArray[0].toObject().value("state").toString();
        peer.setStateString(state);

        if (state == "GOSSIPING") peer.setState(Peer::PeerState::UNINITIALIZED);
        else if (state == "WHATEVER") peer.setState(Peer::PeerState::UNINITIALIZED);

        peer.setState((Peer::PeerState)peerJsonObject.value("state").toInt());

        if (m_peers.contains(peer.id())) {
            m_peers.insert(peer.id(), peer);
            // TODO: how to call this bastard
            dataChanged(index(0, 0), index(rowCount() - 1, 0));
        }
        else {
            beginInsertRows(QModelIndex(), rowCount(), rowCount());
            m_peers.insert(peer.id(), peer);
            endInsertRows();
        }
    }

    emit totalAvailableFundsChanged();

    //    else if (m_peers.count() < previousPeerCount) {
    //        // FIXXX
    //        //beginRemoveRows(index, 0, previousPeerCount);
    //        endRemoveRows();
    //    }
}

int PeersModel::totalAvailableFunds()
{
    int sumOfAvailableFunds = 0;
    foreach (Peer peer, m_peers) {
        sumOfAvailableFunds += peer.msatoshiToUs();
    }
    return sumOfAvailableFunds;
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

QString Peer::stateString() const
{
    return m_stateString;
}

void Peer::setStateString(const QString &stateString)
{
    m_stateString = stateString;
}

