#ifndef NFCCONTROLLER_H
#define NFCCONTROLLER_H

#include <QObject>
#include <QLocalServer>
#include <QLocalSocket>
#include <QTimer>

class NfcHelper : public QObject
{
    Q_OBJECT
public:
    explicit NfcHelper(QObject *parent = nullptr);



private:
    QLocalServer* m_socketServer;
    QLocalSocket* m_socket;

    QLocalSocket* m_unixSocket;

    QString m_socketServerPath;

    bool m_nfcTagPresentLastState;
    bool m_askForSocketData;

    QTimer* m_tagStatusCheckTimer;

    QString m_bolt11;

    QString m_socketPeerId;
    QString m_lightningLocalSocket;

private:
    void onNfcTagArrival();
    void onNfcTagDeparture();
    void sendBolt11ToHceDevice();
    void forwardDataToSocket(QByteArray socketData);
    void resetSocketConnection();
    void connectToLocalSocket();

public slots:
    void setBolt11(const QString &bolt11);
    void connectedToPeer(QString peerId);
    void unixSocketError(QLocalSocket::LocalSocketError unixSocketError);
    void unixSocketDisconnected();

private slots:
    void newConnection();
    void readyRead();
    void nfcTagStatusCheck();
    void socketDisconnected();

signals:

public slots:
};

#endif // NFCCONTROLLER_H
