#include "AutoPilot.h"
#include "LightningModel.h"
#include "NodesModel.h"
#include "PeersModel.h"

#include <QCryptographicHash>

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

void AutoPilot::start(int amountSatoshi, quint32 iteration)
{
    // https://lists.linuxfoundation.org/pipermail/lightning-dev/2018-March/001108.html
    m_autopilotChannelAmount = amountSatoshi;
    QList<Node> nodes = LightningModel::instance()->nodesModel()->getNodes();
    QString ourId = LightningModel::instance()->id();
    QCryptographicHash::Algorithm standardAlgorithm = QCryptographicHash::Sha1;
    quint32 i = iteration;

retry:
    m_autoPilotIteration = i;
    QCryptographicHash ourHash(standardAlgorithm);
    ourHash.addData((char*)&i, 4);
    ourHash.addData(QByteArray::fromHex(ourId.toLatin1()).data(), 33);
    QByteArray ourHashResult = ourHash.result();

    QMap<QByteArray, QString> hashedNodes;

    foreach (Node node, nodes) {
        QCryptographicHash nodeHash(standardAlgorithm);
        ourHash.addData((char*)&i, 4);
        nodeHash.addData(QByteArray::fromHex(node.id().toLatin1()).data());
        hashedNodes.insert(nodeHash.result(), node.id());
    }

    int byte = 0;
    int bit = 0;

    QPair<QByteArray, QString> lastRemovedNode;

    while (hashedNodes.size() > 2) {
        QMapIterator<QByteArray, QString> mapIter(hashedNodes);
        // Compare highest bits
        while (mapIter.hasNext()) {
            mapIter.next();
            if (((mapIter.key().at(byte) >> bit) & 0x01) != ((ourHashResult.at(byte) >> bit) & 0x01)) {
                lastRemovedNode.first = mapIter.key();
                lastRemovedNode.second = mapIter.value();
                hashedNodes.remove(mapIter.key());
            }
        }
        bit++;
        if (bit == 8) {
            bit = 0;
            byte++;
        }
    }

    // Insert ourselves and the last removed node
    hashedNodes.insert(ourHashResult, ourId);
    hashedNodes.insert(lastRemovedNode.first, lastRemovedNode.second);

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
        goto retry;
    }
    else {
        m_currentCandidateNodeId = candidateNodeId;
        LightningModel::instance()->peersModel()->connectToPeer(candidateNodeId, candidateNodeAddress);
    }
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
        start(m_autopilotChannelAmount, m_autoPilotIteration);
    }
}

void AutoPilot::channelFundingFailed(QString peerId)
{
    if (peerId == m_currentCandidateNodeId) {
        m_currentCandidateNodeId = "";
        m_autoPilotIteration++;
        start(m_autopilotChannelAmount, m_autoPilotIteration);
    }
}
