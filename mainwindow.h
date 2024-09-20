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
class S;
class QIODevice;
class QAudioSink;

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
