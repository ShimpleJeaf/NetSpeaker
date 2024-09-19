#ifndef ANYCAPTURE_H
#define ANYCAPTURE_H

#include "captureglobal.h"
#include <QObject>

#include <mutex>
#include <vector>
#include <queue>
extern std::mutex g_m;
typedef std::vector<float> Data;
typedef std::queue<Data*> SQueue;
extern SQueue g_queue;

#include <QUdpSocket>
#include <QVariant>
#include <QNetworkDatagram>

class S : public QObject
{
    Q_OBJECT
public:
    S(QString addr = "239.0.0.1", unsigned short port = 13712, QObject* parent = nullptr)
        : QObject(parent)
        , m_addr(addr)
        , m_port(port)
    {
        m_udpSocket.setSocketOption(QAbstractSocket::MulticastTtlOption, 1);
        if (m_udpSocket.bind(QHostAddress::AnyIPv4, m_port, QUdpSocket::ShareAddress | QUdpSocket::ReuseAddressHint))
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

    ~S()
    {
        m_udpSocket.leaveMulticastGroup(m_addr);
    }

    void send(const char* data, size_t len);

signals:
    void ready(char* pData, size_t len);
    void data(float);

    void readReady(QNetworkDatagram* data);

private:
    QUdpSocket m_udpSocket;
    QHostAddress m_addr;
    unsigned short m_port;
};


QT_FORWARD_DECLARE_CLASS(AnyCapturePrivate);

class AnyCapture : public QObject
{
    Q_OBJECT

public:
    AnyCapture(QObject *parent = nullptr);
    ~AnyCapture();

    /**
     * @brief 获取有效的音频输入
     * @return QList<any_capture::AudioDevice>
     */
    static QList<any_capture::AudioDevice> availableAudioInputs();

    /**
     * @brief 开始采集音频输入
     * @param input 要采集的音频输入
     * @param callback 采集回调
     * @param master 采集回调指针
     * @return bool{true:成功; false:失败}
     */
    bool startCaptureAudioInput(const any_capture::AudioDevice &input, any_capture::capture_callback callback, void *master);

    /**
     * @brief 停止采集音频输入
     */
    void stopCaptureAudioInput();

    /**
     * @brief 获取有效的音频输出
     * @return QList<any_capture::AudioDevice>
     */
    static QList<any_capture::AudioDevice> availableAudioOutputs();

    /**
     * @brief 开始采集音频输出
     * @param output 要采集的音频输出
     * @param callback 采集回调
     * @param master 采集回调指针
     * @return bool{true:成功; false:失败}
     */
    bool startCaptureAudioOutput(const any_capture::AudioDevice &output, any_capture::capture_callback callback, void *master);

    /**
     * @brief 停止采集音频输出
     */
    void stopCaptureAudioOutput();

private:
    QScopedPointer<AnyCapturePrivate> d_ptr;
    Q_DECLARE_PRIVATE(AnyCapture);
};

#endif // ANYCAPTURE_H
