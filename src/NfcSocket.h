#ifndef NFCCONTROLLER_H
#define NFCCONTROLLER_H

#include <QObject>
#include <QLocalServer>
#include <QLocalSocket>
#include <QTimer>

class NfcSocket : public QObject
{
    Q_OBJECT
public:
    explicit NfcSocket(QObject *parent = nullptr);

private:
    QLocalServer* m_socketServer;
    QLocalSocket* m_socket;

    bool m_nfcTagPresentLastState;

    QTimer* m_tagStatusCheckTimer;

    QByteArray bolt11;

    QString m_nfcPeerId;

private:
    void onNfcTagArrival();
    void onNfcTagDeparture();
    void sendBolt11ToHceDevice();


private slots:
    void newConnection();
    void readyRead();
    void nfcTagStatusCheck();

signals:

public slots:
};

#endif // NFCCONTROLLER_H
