#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "anycapture.h"

#include <QtCharts/QtCharts>
#include <QTimer>
#include <QAudioOutput>
#include <QMediaDevices>
#include <QAudioDevice>
#include <QAudioSink>
#include <limits>

int callback(const unsigned char *data, size_t dataSize, void *master)
{
    S* s = reinterpret_cast<S*>(master);
    s->send(reinterpret_cast<const char*>(data), dataSize);
    return 0;
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_chart(new QChart)
    , m_series(new QLineSeries)
    , m_axisX(new QValueAxis)
    , m_axisY(new QValueAxis)
    , m_timer(new QTimer(this))
    , m_capture(new AnyCapture)
    , m_s(new S)
    , m_startFlag(false)
{
    ui->setupUi(this);
    m_devices = AnyCapture::availableAudioOutputs();
    for (auto& dev : m_devices) {
        ui->devices->addItem(dev.deviceName);
        qDebug() << "deviceName: " << dev.deviceName;
        qDebug() << "channels: " << dev.channels;
        qDebug() << "sampleRate: " << dev.sampleRate;
        qDebug() << "sampleSize: " << dev.sampleSize;
    }

    auto charview = new QChartView(m_chart);
    ui->centralwidget->layout()->addWidget(charview);
    ui->type->addItem("发送");
    ui->type->addItem("接收");
    connect(ui->type, &QComboBox::currentIndexChanged, [](){});

    m_chart->addSeries(m_series);
    m_axisX->setRange(0, 2000);
    m_axisY->setRange(-1, 1);
    m_chart->addAxis(m_axisX, Qt::AlignBottom);
    m_series->attachAxis(m_axisX);
    m_chart->addAxis(m_axisY, Qt::AlignLeft);
    m_series->attachAxis(m_axisY);

    connect(ui->pushButton, &QPushButton::clicked, [=](){
        m_startFlag = !m_startFlag;
        if (m_startFlag)
        {
            ui->pushButton->setText("结束");
            m_capture->startCaptureAudioOutput(m_devices[ui->devices->currentIndex()], callback, m_s);
        }
        else
        {
            ui->pushButton->setText("开始");
            m_capture->stopCaptureAudioOutput();
        }
    });
    connect(ui->type, &QComboBox::currentIndexChanged, [=](int index){
        if (index == 0)
            ;
            // stop();
        if (index == 1)
        {
            m_capture->stopCaptureAudioOutput();
        }
        m_startFlag = false;
        ui->pushButton->setText("开始");
    });

    m_format.setSampleRate(48000);
    //设定声道数目，mono(平声道)的声道数目是1；stero(立体声)的声道数目是2
    m_format.setChannelCount(2);
    // m_format.setSampleSize(16);
    // m_format.setCodec("audio/pcm");   //编码器
    //设定高低位,LittleEndian（低位优先）,LargeEndian(高位优先)
    // m_format.setByteOrder(QPixelFormat::LittleEndian);
    m_format.setSampleFormat(QAudioFormat::Float);
    m_format.setChannelConfig(QAudioFormat::ChannelConfigStereo);

    auto dev(QMediaDevices::defaultAudioOutput());
    // m_format = dev.preferredFormat();
    if (dev.isFormatSupported(m_format))
    {
        m_audioSink = new QAudioSink(dev, m_format, this);
        m_audioSink->setBufferSize(102400);
        m_output = m_audioSink->start();
        qDebug() << "audio状态：" << m_audioSink->state();
    }
    else
        qDebug() << "输出音频格式不支持";
    connect(m_s, &S::readReady, [&](QNetworkDatagram* data){
        if (ui->type->currentIndex() == 0)
            return;

        // 数据转换
        auto srcData = /*qUncompress*/(data->data());
        // qDebug() << srcData.data();
        auto pSrc = reinterpret_cast<int*>(srcData.data());
        // 转换成float
        QByteArray dstData(srcData.size(), 0);
        auto pDst = reinterpret_cast<float*>(dstData.data());
        for (int i = 0; i < srcData.size()/4; i++, pSrc++, pDst++)
        {
            *pDst = static_cast<float>(*pSrc) / INT_MAX;
        }
        m_queue.push_back(dstData);
        if (m_queue.size() > 50)
        {
            m_queue.pop_front();
            qDebug() << "队列超过50，丢弃数据";
        }

        if (m_audioSink->state() == QAudio::IdleState
            && !m_queue.empty())
        {
            QByteArray d;
            // qDebug() << "queue size: " << m_queue.size(); // 大概会累计到27
            while (!m_queue.empty() /*&& d.size() < 90000*/)
            {
                d.append(m_queue.front());
                m_queue.pop_front();
            }
            m_output->write(d);

            // m_output->write(m_queue.front());
            // m_queue.pop_front();
            // qDebug() << "收到并写入数据";
        }
    });
}

MainWindow::~MainWindow()
{
    delete ui;
    delete m_chart;
    delete m_capture;
    delete m_s;
}
