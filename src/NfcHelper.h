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

    QString m_socketServerPath;

    bool m_nfcTagPresentLastState;
    bool m_askForSocketData;

    QTimer* m_tagStatusCheckTimer;

    QString m_bolt11;

    QString m_socketPeerId;

private:
    void onNfcTagArrival();
    void onNfcTagDeparture();
    void sendBolt11ToHceDevice();

public slots:
    void setBolt11(const QString &bolt11);

private slots:
    void newConnection();
    void readyRead();
    void nfcTagStatusCheck();

signals:

public slots:
};

#endif // NFCCONTROLLER_H
