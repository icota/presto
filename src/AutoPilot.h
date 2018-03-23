#ifndef AUTOPILOT_H
#define AUTOPILOT_H

#include <QObject>

class AutoPilot : public QObject
{
    Q_OBJECT
public:
    explicit AutoPilot(QObject *parent = nullptr);

signals:
    void success(QString peerId);
    void failure();

public slots:
    void start(int amountSatoshi, quint32 iteration = 0);
    void stop();

    void connectedToPeer(QString peerId);
    void channelFunded(QString peerId);
    void connectingFailed(QString peerId);
    void channelFundingFailed(QString peerId);

private:
    int m_autopilotChannelAmount;
    QString m_currentCandidateNodeId;
    quint32 m_autoPilotIteration;
};

#endif // AUTOPILOT_H
