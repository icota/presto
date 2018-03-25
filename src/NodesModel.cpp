#include "NodesModel.h"
#include "./3rdparty/qjsonrpc/src/qjsonrpcservicereply.h"
#include "macros.h"

NodesModel::NodesModel(QJsonRpcSocket *rpcSocket)
{
    m_rpcSocket = rpcSocket;
    m_nodes = QList<Node>();
}

void NodesModel::updateNodes()
{
    QJsonRpcMessage message = QJsonRpcMessage::createRequest("listnodes", QJsonValue());
    SEND_MESSAGE_CONNECT_SLOT(message, &NodesModel::updateNodesRequestFinished)
}

void NodesModel::updateNodesRequestFinished()
{
    GET_MESSAGE_DISCONNECT_SLOT(message, &NodesModel::updateNodesRequestFinished)
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

        QJsonObject nodeJsonObject = v.toObject();
        Node node;

        node.setAlias(nodeJsonObject.value("alias").toString());
        node.setColor(nodeJsonObject.value("color").toString());
        node.setLastTimestamp(nodeJsonObject.value("last_timestamp").toInt());
        node.setId(nodeJsonObject.value("nodeid").toString());

        QJsonArray nodeAddressesArray = nodeJsonObject.value("addresses").toArray();

        QList<NodeAddress> addressList;

        foreach (QJsonValue addressValue, nodeAddressesArray) {
            QJsonObject addressObject = addressValue.toObject();

            NodeAddress address;
            address.setPort(addressObject.value("port").toInt());
            address.setAddress(addressObject.value("address").toString());

            if (addressObject.value("type").toString() == "ipv4") {
                address.setAddressType(NodeAddress::IPv4);
            }
            else {
                address.setAddressType(NodeAddress::IPv6);
            }

            addressList.append(address);
        }

        node.setNodeAddressList(addressList);

        m_nodes.append(node);
    }
}

QList<Node> NodesModel::getNodes() const
{
    return m_nodes;
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

QList<NodeAddress> Node::nodeAddressList() const
{
    return m_nodeAddressList;
}

void Node::setNodeAddressList(const QList<NodeAddress> &nodeAddressList)
{
    m_nodeAddressList = nodeAddressList;
}

NodeAddress::NodeAddress()
{

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
