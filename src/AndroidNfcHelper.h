#ifndef ANDROIDHELPER_H
#define ANDROIDHELPER_H

#include <QDebug>

#include <QLocalServer>
#include <QDir>

#include "LightningModel.h"

class AndroidNfcHelper : public QObject
{
    Q_OBJECT
public:
    explicit AndroidNfcHelper(QObject *parent = nullptr);
    static AndroidNfcHelper* instance();
    void bolt11FromJni(QString bolt11);
    void linkDeactivated(int reason);

private slots:
    void newConnection();
    void readyRead();    
    void socketDisconnected();

public slots:
    void paymentDecoded(int createdAt, QString currency, QString description,
                        int expiry, int minFinalCltvExpiry, int msatoshi,
                        QString payee, QString paymentHash, QString signature,
                        int timestamp, QString bolt11);

    void connectedToPeer(QString peerId);
    void forwardDataToSocket(QByteArray socketData);

signals:
    void bolt11ReceivedThroughNfc(QString bolt11);

private:
    QLocalServer* m_socketServer;
    QLocalSocket* m_socket;
    QString m_socketServerPath;

    QString m_socketPeerId;

private:
    static AndroidNfcHelper *sInstance;
};

#endif // ANDROIDHELPER_H
