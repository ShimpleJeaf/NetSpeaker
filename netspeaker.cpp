#include "netspeaker.h"

// #include <QAudioDeviceInfo>
#include <QAudioFormat>
#include <QDebug>
// #include <QAudioProbe>

Sender::Sender(const QAudioDevice& device, QString addr, unsigned short port, QObject* parent)
    : QThread(parent)
    , m_device(device)
    , m_addr(addr)
    , m_port(port)
{
    // m_format.setSampleRate(16000);
    // //设定声道数目，mono(平声道)的声道数目是1；stero(立体声)的声道数目是2
    // m_format.setChannelCount(2);
    // m_format.setSampleSize(16);
    // m_format.setCodec("audio/pcm");   //编码器
    // //设定高低位,LittleEndian（低位优先）,LargeEndian(高位优先)
    // m_format.setByteOrder(QAudioFormat::LittleEndian);
    // m_format.setSampleType(QAudioFormat::SignedInt);

    // m_udpSocket.setSocketOption(QAbstractSocket::MulticastTtlOption, 1);
    // if (m_udpSocket.bind(QHostAddress::AnyIPv4, m_port, QUdpSocket::ShareAddress | QUdpSocket::ReuseAddressHint))
    // {
    //     m_udpSocket.joinMulticastGroup(m_addr);
    // }
    // else qDebug() << "bind failed";

    // connect(&m_probe, &QAudioProbe::audioBufferProbed, this, &Sender::processBuffer);
    // // m_probe.setSource(m_device);
}

void Sender::run()
{
    while (true)
    {

    }
}

void Sender::stop()
{
    terminate();
    wait();
}

void Sender::processBuffer(const QAudioBuffer &buffer)
{

}
