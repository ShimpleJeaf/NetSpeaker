#include "mainwindow.h"
#include <QApplication>
#include <QFile>

char g_buffer[1024000];

QTextStream stream;

void messageHandle(QtMsgType, const QMessageLogContext &, const QString & message)
{
    stream << message << Qt::endl;
    stream.flush();
}

int main(int argc, char *argv[])
{
    QFile file("log.txt");
    file.open(QFile::WriteOnly | QFile::Text);
    stream.setDevice(&file);
    stream << "log begin\n";
    stream.flush();
    qInstallMessageHandler(messageHandle);
    QApplication a(argc, argv);
    MainWindow w;
    // QObject::connect(&s, &S::ready, &w, &MainWindow::setData, Qt::QueuedConnection);
    // QObject::connect(&s, &S::data, &w, &MainWindow::setFloat, Qt::QueuedConnection);
    // capture.startCaptureAudioOutput(dev, &callback, &s);
    w.show();
    return a.exec();
    // capture.stopCaptureAudioOutput();
}
