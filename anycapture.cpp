#include "anycapture.h"
#include "audiocapture.h"
#include <QSharedPointer>

#ifdef Q_OS_WIN
#include <Windows.h>
#endif

std::mutex g_m;
SQueue g_queue;

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

void S::send(const char *data, size_t len)
{
    // const int* pF = reinterpret_cast<const int*>(data);
    // int a;
    // for (int i = 0; i < len/4; i++, pF++)
    //     a = *pF;
    m_udpSocket.writeDatagram(data, len, m_addr, m_port);
    // if (m_udpSocket.state() == QAbstractSocket::ConnectedState)
    // {
    //     int sendlen = m_udpSocket.write(data, len);
    //     qDebug() << sendlen;
    // }
    // else {
    //     qDebug() << "网络未连接";
    // }
}
