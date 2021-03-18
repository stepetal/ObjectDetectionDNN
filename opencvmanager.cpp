#include "opencvmanager.h"


OpenCVManager::OpenCVManager(QSharedPointer<DeepNeuralNetworManager> dnn) : videoCapture(cv::VideoCapture()),
                                                                            deepNetworkManager(dnn),
                                                                            queueWriter(new QueueWriter()),
                                                                            process(false)
{
    createConnections();
    qRegisterMetaType<cv::VideoCapture*>("VideoCapture");
    qRegisterMetaType<FPSQueue<cv::Mat>*>("FPSQueue");

}

OpenCVManager::~OpenCVManager()
{
    //delete videoCapture;
    //videoCapture.release();


}

void OpenCVManager::createConnections()
{
    //connect(&queueWriterThread,&QThread::finished,queueWriter,&QObject::deleteLater);
    connect(queueWriter,&QueueWriter::terminateThread,this,
            [&](){
                qDebug() << "Quit from thread";
                queueWriterThread.quit();
            });
    connect(this,&OpenCVManager::startQueueWriterThread,queueWriter,&QueueWriter::writeToQueue);
    connect(this,&OpenCVManager::stopWritingProcedure,queueWriter,&QueueWriter::stopProcessLoop);
    connect(queueWriter,&QueueWriter::resultReady,this,&OpenCVManager::handleQueueWriterResult);
}

bool OpenCVManager::openVideoCaputureDevice(SYS::VideoInputEnum inputType, QString fileName, uint8_t numb)
{

    switch(inputType)
    {
        case SYS::VideoInputEnum::WEB:
            videoCapture.open(numb); //open first available webcam
            break;
        case SYS::VideoInputEnum::FILE:
            videoCapture.open(fileName.toStdString());
        default:
        break;
    };
    if(!videoCapture.isOpened())
    {
        return false;
    }
    return true;
}

void OpenCVManager::createOpenCVWindow()
{
    cv::namedWindow("Object detection with DNN",cv::WINDOW_NORMAL);
}

void OpenCVManager::processVideoFrames()
{
    qDebug() << "Main thread id is: " << QThread::currentThreadId();

    process = true;
    queueWriter->moveToThread(&queueWriterThread);
    queueWriterThread.start();
    emit startQueueWriterThread(&videoCapture,&framesQueue);
//    while(cv::waitKey(0))
//    {
//        //some work
//        QEventLoop evLoop;
//        evLoop.processEvents();
//    }
//    emit stopWritingProcedure();
    //queueWriterThread.quit();
    //queueWriterThread.wait();
}

void OpenCVManager::stopCapturingFrames()
{
    process = false;
    emit stopWritingProcedure();
}

void OpenCVManager::releaseResources()
{
    queueWriter->stopProcessLoop();
    videoCapture.release();
    cv::destroyAllWindows();
}

void OpenCVManager::handleQueueWriterResult(const QString &result)
{
    qDebug() << result;
}



void QueueWriter::writeToQueue(cv::VideoCapture *cap, FPSQueue<cv::Mat> *framesQueue)
{
    qDebug() << "QueueWriter thread id is: " << QThread::currentThreadId();
    cv::Mat frame;
    process = true;
    while (process)
    {
        *cap >> frame;
        if(!frame.empty())
        {
            cv::imshow("Object detection with DNN",frame);
            //framesQueue->push(frame.clone());
        }
        else
        {
            emit resultReady("Frame is empty");
            break;
        }
        QEventLoop evLoop;
        evLoop.processEvents();
    }
    emit resultReady("Normal exit from thread procedure");
    emit terminateThread();
}
