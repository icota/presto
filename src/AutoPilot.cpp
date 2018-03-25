#include "AutoPilot.h"
#include "LightningModel.h"
#include "NodesModel.h"
#include "PeersModel.h"

#include <QCryptographicHash>
#include <QtEndian>
#include "3rdparty/QtCryptoHash/lib/include/qcryptohash.hpp"

AutoPilot::AutoPilot(QObject *parent) : QObject(parent)
{
    connect(LightningModel::instance()->peersModel(), &PeersModel::connectedToPeer,
            this, &AutoPilot::connectedToPeer);

    connect(LightningModel::instance()->peersModel(), &PeersModel::connectingFailed,
            this,  &AutoPilot::connectingFailed);

    connect(LightningModel::instance()->peersModel(), &PeersModel::channelFunded,
            this, &AutoPilot::channelFunded);

    connect(LightningModel::instance()->peersModel(), &PeersModel::channelFundingFailed,
            this, &AutoPilot::channelFundingFailed);
}

void AutoPilot::go(int amountSatoshi, quint32 iteration)
{
    // https://lists.linuxfoundation.org/pipermail/lightning-dev/2018-March/001108.html
    m_autopilotChannelAmount = amountSatoshi;

    QList<Node> nodes = LightningModel::instance()->nodesModel()->getNodes();
    if (nodes.isEmpty()) {
        emit failure();
        return;
    }

    if (m_autoPilotIteration == (quint32)-1) {
        return;
    }

    QString ourId = LightningModel::instance()->id();
    QCryptographicHash::Algorithm standardAlgorithm1 = QCryptographicHash::Sha256;
    QCryptoHash::Algorithm standardAlgorithm2 = QCryptoHash::RMD160;

    quint32 i = iteration;
    m_autoPilotIteration = i;

    quint32 networkOrderI = qToBigEndian(i);

    QCryptographicHash ourHashSha256(standardAlgorithm1);
    ourHashSha256.addData((char*)&networkOrderI, 4);
    ourHashSha256.addData(QByteArray::fromHex(ourId.toLatin1()).data(), 33);
    QByteArray ourHash = QCryptoHash::hash(ourHashSha256.result(), standardAlgorithm2);

    QMap<QByteArray, QString> hashedNodes;

    foreach (Node node, nodes) {
        QCryptographicHash nodeHashSha256(standardAlgorithm1);
        nodeHashSha256.addData((char*)&networkOrderI, 4);
        nodeHashSha256.addData(QByteArray::fromHex(node.id().toLatin1()).data());
        QByteArray nodeHash = QCryptoHash::hash(nodeHashSha256.result(), standardAlgorithm2);
        hashedNodes.insert(nodeHash, node.id());
    }

    // Add our id to the working set
    hashedNodes.insert(ourHash, ourId);

    int byte = 0;
    int bit = 0;

    QMap<QByteArray, QString> rejectedNodes;

    while (hashedNodes.size() > 2) {
        rejectedNodes.clear();
        QMapIterator<QByteArray, QString> mapIter(hashedNodes);
        // Compare highest bits
        while (mapIter.hasNext()) {
            mapIter.next();
            if (((mapIter.key().at(byte) >> bit) & 0x01) != ((ourHash.at(byte) >> bit) & 0x01)) {
                rejectedNodes.insert(mapIter.key(), mapIter.value());
                hashedNodes.remove(mapIter.key());
            }
        }
        bit++;
        if (bit == 8) {
            bit = 0;
            byte++;
        }
    }

    // Insert ourselves and the last batch of rejected nodes
    hashedNodes.insert(ourHash, ourId);

    QMapIterator<QByteArray, QString> rejectedMapIter(rejectedNodes);
    while (rejectedMapIter.hasNext()) {
        rejectedMapIter.next();
        hashedNodes.insert(rejectedMapIter.key(), rejectedMapIter.value());
    }

    // QMaps are sorted, I think
    QString candidateNodeId;
    bool foundOurNodeId = false;

    QMapIterator<QByteArray, QString> mapIter(hashedNodes);
    while (mapIter.hasNext()) {
        mapIter.next();
        if (foundOurNodeId) {
            candidateNodeId = mapIter.value();
        }
        if (mapIter.value() == ourId) {
            foundOurNodeId = true;
        }
    }

    // Looks like we are last
    if (candidateNodeId.isEmpty()) {
        candidateNodeId = hashedNodes.first();
    }

    // Any idea how to connect to our candidate?
    QString candidateNodeAddress;
    foreach (Node node, nodes) {
        if (node.id() == candidateNodeId) {
            if (!node.nodeAddressList().isEmpty()) {
                candidateNodeAddress = node.nodeAddressList().at(0).address();
            }
        }
    }

    if (candidateNodeAddress.isEmpty()) {
        i++;
        go(amountSatoshi, i);
    }
    else {
        m_currentCandidateNodeId = candidateNodeId;
        LightningModel::instance()->peersModel()->connectToPeer(candidateNodeId, candidateNodeAddress);
    }
}

void AutoPilot::stop()
{
    m_currentCandidateNodeId = "";
    m_autoPilotIteration = -1;
}

void AutoPilot::connectedToPeer(QString peerId)
{
    if (peerId == m_currentCandidateNodeId) {
        LightningModel::instance()->peersModel()->fundChannel(peerId, m_autopilotChannelAmount);
    }
}

void AutoPilot::channelFunded(QString peerId)
{
    if (peerId == m_currentCandidateNodeId) {
        emit success(peerId);
        m_currentCandidateNodeId = "";
        m_autoPilotIteration = 0;
    }
}

void AutoPilot::connectingFailed(QString peerId)
{
    if (peerId == m_currentCandidateNodeId) {
        m_currentCandidateNodeId = "";
        m_autoPilotIteration++;
        go(m_autopilotChannelAmount, m_autoPilotIteration);
    }
}

void AutoPilot::channelFundingFailed(QString peerId)
{
    if (peerId == m_currentCandidateNodeId) {
        m_currentCandidateNodeId = "";
        m_autoPilotIteration++;
        go(m_autopilotChannelAmount, m_autoPilotIteration);
    }
}
