#ifndef OPENCVMANAGER_H
#define OPENCVMANAGER_H

#include <opencv2/dnn.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include "globals.h"
#include <QObject>
#include <QThread>
#include <QDebug>
#include <QScopedPointer>
#include <QSharedPointer>
#include <QMetaType>
#include <QEventLoop>
#include "deepneuralnetwormanager.h"
#include "fpsqueue.h"


/*
 * Классы, реализующие рабочие объекты для потоков сохранения видеофреймов очередь и обработки
 * этих фреймов из очереди
 */

class QueueWriter : public QObject
{
    Q_OBJECT
    bool process;
public:
    QueueWriter() : process(true) {}
public slots:
    void writeToQueue(cv::VideoCapture *cap, FPSQueue<cv::Mat> *framesQueue);
    void stopProcessLoop()
    {
        qDebug() << "stop capturing frames";
        process = false;
    }
signals:
    void terminateThread();
    void resultReady(const QString &result);
};

/*
 * Класс, реализующий функционал по обработке запросов к видеоустройству
 */

class OpenCVManager : public QObject
{
    Q_OBJECT
    cv::VideoCapture videoCapture;
    FPSQueue<cv::Mat> framesQueue;
    QSharedPointer<DeepNeuralNetworManager> deepNetworkManager;

    QueueWriter *queueWriter;//объект по записи в очередь фреймов
    QThread queueWriterThread;//отдельный поток для объекта по записи в очередь фреймов
    bool process; //true, если выполняется обработка очередного фрейма
public:
    OpenCVManager(QSharedPointer<DeepNeuralNetworManager> dnn);
    ~OpenCVManager();
    void createConnections();
    bool openVideoCaputureDevice(SYS::VideoInputEnum inputType,QString fileName = "",uint8_t numb = 0);
    void createOpenCVWindow();
    void processVideoFrames(); //основной метод по обработке кадров из видеопоследовательности.
                          //Содержит создание отдельного потока для сохранения фреймов в очередь и для обработки фреймов
    void stopCapturingFrames();
    void releaseResources();
public slots:
    void handleQueueWriterResult(const QString &result);
signals:
    void startQueueWriterThread(cv::VideoCapture *cap, FPSQueue<cv::Mat> *framesQueue);
    void stopWritingProcedure();
};



#endif // OPENCVMANAGER_H
