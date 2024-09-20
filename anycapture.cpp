#include "anycapture.h"
#include "audiocapture.h"
#include <QSharedPointer>
#include <limits>

#ifdef Q_OS_WIN
#include <Windows.h>
#endif

#include <QObject>
S::S(QString addr, unsigned short port, QObject* parent)
    : QObject(parent)
    , m_addr(addr)
    , m_port(port)
{
    m_udpSocket.setSocketOption(QAbstractSocket::MulticastTtlOption, 1);
    if (m_udpSocket.bind(QHostAddress::AnyIPv4, m_port, QUdpSocket::ShareAddress | QUdpSocket::ReuseAddressHint))
    // if (m_udpSocket.bind(QHostAddress("192.168.137.1"), m_port, QUdpSocket::ShareAddress | QUdpSocket::ReuseAddressHint))
    {
        if (!m_udpSocket.joinMulticastGroup(m_addr))
            qDebug() << "加入组播失败";
    }
    else
        qDebug() << "ip绑定失败";
    if (!connect(&m_udpSocket, &QUdpSocket::readyRead, [=](){
            while (m_udpSocket.hasPendingDatagrams())
            {
                QNetworkDatagram data = m_udpSocket.receiveDatagram();
                emit readReady(&data);
            }
        }))
        qDebug() << "接收数据信号槽绑定失败";
    m_udpSocket.open(QIODevice::ReadWrite);
}

S::~S()
{
    m_udpSocket.leaveMulticastGroup(m_addr);
    m_udpSocket.close();
}

void S::send(const char *data, size_t len)
{
    m_udpSocket.writeDatagram(data, len, m_addr, m_port);
}

using namespace any_capture;

class AnyCapturePrivate
{
public:
    QSharedPointer<AudioCapture> m_audioInputCapture, m_audioOutputCapture;
};

AnyCapture::AnyCapture(QObject *parent)
    : QObject(parent)
    , d_ptr(new AnyCapturePrivate)
{

}

AnyCapture::~AnyCapture()
{
    stopCaptureAudioInput();
    stopCaptureAudioOutput();
}

QList<AudioDevice> AnyCapture::availableAudioInputs()
{
    return AudioCapture::availableAudios(AudioType::Audio_Input);
}

bool AnyCapture::startCaptureAudioInput(const AudioDevice &input, capture_callback callback, void *master)
{
    Q_D(AnyCapture);

    stopCaptureAudioInput();

    if (input.deviceHandle == nullptr) {
        return false;
    }

    d->m_audioInputCapture.reset(new AudioCapture(input));
    d->m_audioInputCapture->setCaptureParams(callback, master);
    if (callback)
        d->m_audioInputCapture->startCapture();

    return true;
}

void AnyCapture::stopCaptureAudioInput()
{
    Q_D(AnyCapture);

    if (!d->m_audioInputCapture.isNull())
        d->m_audioInputCapture->stopCapture();
}

QList<AudioDevice> AnyCapture::availableAudioOutputs()
{
    return AudioCapture::availableAudios(AudioType::Audio_Output);
}

bool AnyCapture::startCaptureAudioOutput(const AudioDevice &output, capture_callback callback, void *master)
{
    Q_D(AnyCapture);

    stopCaptureAudioOutput();

    if (output.deviceHandle == nullptr) {
        return false;
    }

    d->m_audioOutputCapture.reset(new AudioCapture(output));
    d->m_audioOutputCapture->setCaptureParams(callback, master);
    if (callback)
        d->m_audioOutputCapture->startCapture();

    return true;
}

void AnyCapture::stopCaptureAudioOutput()
{
    Q_D(AnyCapture);

    if (!d->m_audioOutputCapture.isNull())
        d->m_audioOutputCapture->stopCapture();
}

