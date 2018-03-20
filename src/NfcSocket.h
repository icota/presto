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

    QString m_socketServerPath;

    bool m_nfcTagPresentLastState;
    bool m_askForSocketData;

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
