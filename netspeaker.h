#ifndef NETSPEAKER_H
#define NETSPEAKER_H

#include <QAudioDevice>
#include <QThread>
#include <QUdpSocket>
#include <QMutex>
#include <QAudioFormat>
#include <QAudioBuffer>

class Sender : public QThread
{
    Q_OBJECT
public:
    Sender(const QAudioDevice& device, QString addr = "239.0.0.1", unsigned short port = 13712, QObject* parent = nullptr);

    virtual void run() override;
    void stop();

public slots:
    void processBuffer(const QAudioBuffer& buffer);

private:
    QAudioDevice m_device;
    QAudioFormat m_format;
    QUdpSocket m_udpSocket;
    QHostAddress m_addr;
    unsigned short m_port;
};

class Recver : public QThread
{

};

class NetSpeaker
{

};

#endif // NETSPEAKER_H
