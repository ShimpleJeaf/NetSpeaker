#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QAudioFormat>
#include "anycapture.h"
#include <QQueue>
#include <QByteArray>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class QChart;
class QLineSeries;
class QValueAxis;
class QTimer;
class AnyCapture;
class QIODevice;
class QAudioSink;

#include <QUdpSocket>
#include <QVariant>
#include <QNetworkDatagram>

class S : public QObject
{
    Q_OBJECT
public:
    S(QString addr = "239.0.0.1", unsigned short port = 13712, QObject* parent = nullptr);
    ~S();

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

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    QChart* m_chart;
    QLineSeries* m_series;
    QValueAxis* m_axisX;
    QValueAxis* m_axisY;
    QTimer* m_timer;
    AnyCapture* m_capture;
    S* m_s;
    QList<any_capture::AudioDevice> m_devices;
    bool m_startFlag;
    QAudioFormat m_format;
    QIODevice* m_output;
    QAudioSink* m_audioSink;
    QQueue<QByteArray> m_queue;
};
#endif // MAINWINDOW_H
