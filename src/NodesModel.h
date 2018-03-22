#ifndef NODESMODEL_H
#define NODESMODEL_H

#include <QObject>

#include "./3rdparty/qjsonrpc/src/qjsonrpcsocket.h"
#include "./3rdparty/qjsonrpc/src/qjsonrpcmessage.h"

class NodeAddress
{
    public:

    enum AddressType {
        IPv4,
        IPv6
    };

    NodeAddress();

    AddressType addressType() const;
    void setAddressType(const AddressType &addressType);

    QString address() const;
    void setAddress(const QString &address);

    int port() const;
    void setPort(int port);

private:
    enum AddressType m_addressType;
    QString m_address;
    int m_port;
};

class Node
{
public:
    Node();

    QString id() const;
    void setId(const QString &id);

    QString alias() const;
    void setAlias(const QString &alias);

    QString color() const;
    void setColor(const QString &color);

    int lastTimestamp() const;
    void setLastTimestamp(int lastTimestamp);

    QList<NodeAddress> nodeAddressList() const;
    void setNodeAddressList(const QList<NodeAddress> &nodeAddressList);

private:
    QString m_id;
    QString m_alias;
    QString m_color;
    int m_lastTimestamp;
    QList<NodeAddress> m_nodeAddressList;
};

class NodesModel : public QObject
{
    Q_OBJECT
public:
    NodesModel(QJsonRpcSocket *rpcSocket);
    void updateNodes();


    Node getRandomAutoconnectNode();
    QList<Node> getNodes() const;

private slots:
    void updateNodesRequestFinished();

private:
    void populateNodesFromJson(QJsonArray jsonArray);

private:
    QList<Node> m_nodes;
    QJsonRpcSocket* m_rpcSocket;
};

#endif // NODESMODEL_H
