#include "NodesModel.h"
#include "./3rdparty/qjsonrpc/src/qjsonrpcservicereply.h"

NodesModel::NodesModel(QJsonRpcSocket *rpcSocket)
{
    m_rpcSocket = rpcSocket;
    m_nodes = QList<Node>();
}

void NodesModel::updateNodes()
{
    QJsonRpcMessage message = QJsonRpcMessage::createRequest("listnodes", QJsonValue());
    QJsonRpcServiceReply* reply = m_rpcSocket->sendMessage(message);
    QObject::connect(reply, &QJsonRpcServiceReply::finished, this, &NodesModel::listNodesRequestFinished);
}

void NodesModel::listNodesRequestFinished()
{
    QJsonRpcServiceReply *reply = static_cast<QJsonRpcServiceReply *>(sender());
    QJsonRpcMessage message = reply->response();

    if (message.type() == QJsonRpcMessage::Response)
    {
        QJsonObject jsonObject = message.toObject();

        if (jsonObject.contains("result"))
        {
            QJsonObject resultObject = jsonObject.value("result").toObject();
            populateNodesFromJson(resultObject.value("nodes").toArray());
        }
    }
}

void NodesModel::populateNodesFromJson(QJsonArray jsonArray)
{
    m_nodes.clear();

    foreach (const QJsonValue &v, jsonArray)
    {

        QJsonObject NodeJsonObject = v.toObject();
        Node node;
//        Node.setLabel(NodeJsonObject.value("label").toString());
//        Node.setHash(NodeJsonObject.value("payment_hash").toString());
//        Node.setMsatoshi(NodeJsonObject.value("msatoshi").toInt());

        QString status = NodeJsonObject.value("status").toString();

//        if (status.toLower() == "paid") Node.setStatus(NodeTypes::NodeStatus::PAID);
//        else if (status.toLower() == "unpaid") Node.setStatus(NodeTypes::NodeStatus::UNPAID);
//        else if (status.toLower() == "expired") Node.setStatus(NodeTypes::NodeStatus::EXPIRED);

//        Node.setStatusString(status);

//        Node.setPayIndex(NodeJsonObject.value("pay_index").toInt());
//        Node.setMsatoshiReceived(NodeJsonObject.value("msatoshi_received").toInt());
//        Node.setPaidTimestamp(NodeJsonObject.value("paid_timestamp").toInt()); // TODO: Fix this
//        Node.setPaidAtTimestamp(NodeJsonObject.value("paid_at").toInt());
//        Node.setExpiryTime(NodeJsonObject.value("expiry_time").toInt());
//        Node.setExpiresAtTime(NodeJsonObject.value("expires_at").toInt());
//        Node.setBolt11(NodeJsonObject.value("bolt11").toString());

        m_nodes.append(node);
    }
}

Node::Node()
{}

QString Node::id() const
{
    return m_id;
}

void Node::setId(const QString &id)
{
    m_id = id;
}

QString Node::alias() const
{
    return m_alias;
}

void Node::setAlias(const QString &alias)
{
    m_alias = alias;
}

QString Node::color() const
{
    return m_color;
}

void Node::setColor(const QString &color)
{
    m_color = color;
}

int Node::lastTimestamp() const
{
    return m_lastTimestamp;
}

void Node::setLastTimestamp(int lastTimestamp)
{
    m_lastTimestamp = lastTimestamp;
}

NodeAddress::AddressType NodeAddress::addressType() const
{
    return m_addressType;
}

void NodeAddress::setAddressType(const AddressType &addressType)
{
    m_addressType = addressType;
}

QString NodeAddress::address() const
{
    return m_address;
}

void NodeAddress::setAddress(const QString &address)
{
    m_address = address;
}

int NodeAddress::port() const
{
    return m_port;
}

void NodeAddress::setPort(int port)
{
    m_port = port;
}
