#ifndef MACROS_H
#define MACROS_H

#define SEND_MESSAGE_CONNECT_SLOT(message, slot) QJsonRpcServiceReply* reply = m_rpcSocket->sendMessage(message);\
QObject::connect(reply, &QJsonRpcServiceReply::finished, this, slot);

#define GET_MESSAGE_DISCONNECT_SLOT(message, slot) QJsonRpcServiceReply *reply = static_cast<QJsonRpcServiceReply *>(sender());\
QObject::disconnect(reply, &QJsonRpcServiceReply::finished, this, slot);\
QJsonRpcMessage message = reply->response();

#endif // MACROS_H
