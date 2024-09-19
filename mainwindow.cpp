#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "anycapture.h"

#include <QtCharts/QtCharts>
#include <QTimer>
#include <QAudioOutput>
#include <QMediaDevices>
#include <QAudioDevice>
#include <QAudioSink>

int callback(const unsigned char *data, size_t dataSize, void *master)
{
    // std::lock_guard<std::mutex> lk(g_m);
    // std::memcpy(g_buffer, data, dataSize);
    S* s = reinterpret_cast<S*>(master);
    // emit s->ready(g_buffer, dataSize);

    s->send(reinterpret_cast<const char*>(data), dataSize);

    // auto pF = reinterpret_cast<const float*>(data);
    // size_t flen = dataSize / 4;
    // auto pV = new Data(flen);
    // for (int i = 0; i < flen; i++, pF++)
    // {
    //     (*pV)[i] = *pF;
    // }
    // std::lock_guard<std::mutex> lk(g_m);
    // g_queue.push(pV);
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

    // connect(m_timer, &QTimer::timeout, this, &MainWindow::onTimer);
    // m_timer->start(100);

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
            stop();
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

    auto dev(QMediaDevices::defaultAudioOutput());
    if (dev.isFormatSupported(m_format))
    {
        m_audioSink = new QAudioSink(dev, m_format, this);
        m_audioSink->setBufferSize(10240000);
        m_output = m_audioSink->start();
        qDebug() << "audio状态：" << m_audioSink->state();
    }
    else
        qDebug() << "输出音频格式不支持";
    connect(m_s, &S::readReady, [&](QNetworkDatagram* data){
        if (m_audioSink->state() == QAudio::IdleState)
        {
            m_output->write(data->data());
            qDebug() << m_audioSink->error();
            qDebug() << "收到并写入数据";
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

void MainWindow::setData(char *pData, size_t len)
{
    std::lock_guard<std::mutex> lk(g_m);
    float* pF = reinterpret_cast<float*>(pData);
    size_t flen = len / 4;
    for (size_t i = 0; i < flen; i++, pF++)
    {
        m_series->append(i, *pF);
    }
}

void MainWindow::setFloat(float data)
{
    static int i = 0;
    // m_series->append(i, data);
    i = (i+1) % 48000;
}

void MainWindow::onTimer()
{
    Data* pD;
    std::unique_lock<std::mutex> lk(g_m);
    if (g_queue.empty())
        return;
    pD = g_queue.front();
    g_queue.pop();
    lk.unlock();

    int i = 0;
    static QList<QPointF> data;
    data.clear();
    for (int j = 0; j < pD->size(); j+=1)
    // for (auto& d : *pD)
    {
        data.push_back(QPointF(i++, (*pD)[j]));
        i++;
    }
    m_series->replace(data);
    m_chart->show();
    delete pD;
}

void MainWindow::play()
{

}

void MainWindow::stop()
{

}
