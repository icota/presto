#ifndef ANDROIDHELPER_H
#define ANDROIDHELPER_H

#include <QDebug>

#include <QLocalServer>
#include <QDir>

#include "LightningModel.h"

class AndroidHelper : public QObject
{
    Q_OBJECT
public:
    explicit AndroidHelper(QObject *parent = nullptr);

private slots:
    void newConnection()
    {
        qDebug() << "New connection on android socket";
    }

    void readyRead()
    {
        qDebug() << "Data read on android socket";
    }

private:
    QLocalServer* m_socketServer;
    QLocalSocket* m_socket;
    QString m_socketServerPath;

};

#endif // ANDROIDHELPER_H
