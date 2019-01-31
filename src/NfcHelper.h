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
    bool m_nfcTagPresentLastState;
    bool m_askForSocketData;

    QTimer* m_tagStatusCheckTimer;

    QString m_bolt11;

private:
    void onNfcTagArrival();
    void onNfcTagDeparture();
    void sendBolt11ToHceDevice();

public slots:
    void setBolt11(const QString &bolt11);
    void connectedToPeer(QString peerId);

private slots:
    void newConnection();
    void readyRead();
    void nfcTagStatusCheck();

signals:

public slots:
};

#endif // NFCCONTROLLER_H
