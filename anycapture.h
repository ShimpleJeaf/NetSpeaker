#ifndef ANYCAPTURE_H
#define ANYCAPTURE_H

#include "captureglobal.h"
#include <QObject>

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
