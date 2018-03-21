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

private slots:
    void newConnection()
    {
        qDebug() << "New connection on android socket";
    }

    void readyRead()
    {
        qDebug() << "Data read on android socket";
    }

signals:
    void bolt11ReceivedThroughNfc(QString bolt11);

private:
    QLocalServer* m_socketServer;
    QLocalSocket* m_socket;
    QString m_socketServerPath;

private:
    static AndroidNfcHelper *sInstance;

};

#endif // ANDROIDHELPER_H
