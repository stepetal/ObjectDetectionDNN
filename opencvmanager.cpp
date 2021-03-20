#include "opencvmanager.h"


OpenCVManager::OpenCVManager() : videoCapture(cv::VideoCapture()),

                                queueWriter(new QueueWriter()),
                                queueReader(new QueueReader()),
                                process(false)
{
    createConnections();
    qRegisterMetaType<cv::VideoCapture*>("VideoCapture");
    qRegisterMetaType<FPSQueue<cv::Mat>*>("FPSQueue");
    qRegisterMetaType<QSharedPointer<DeepNeuralNetworManager>>("DNNManager");

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
                qDebug() << "Quit from reader thread";
                queueWriterThread.quit();
            });
    connect(queueReader,&QueueReader::terminateThread,this,
            [&](){
                qDebug() << "Quit from writer thread";
                queueReaderThread.quit();
            });


    connect(this,&OpenCVManager::stopWritingProcedure,queueWriter,&QueueWriter::stopProcessLoop);
    connect(this,&OpenCVManager::stopWritingProcedure,queueReader,&QueueReader::stopProcessLoop);

    connect(this,&OpenCVManager::startQueueWriterThread,queueWriter,&QueueWriter::writeToQueue);
    connect(this,&OpenCVManager::startQueueReaderThread,queueReader,&QueueReader::readFromQueue);

    connect(queueWriter,&QueueWriter::resultReady,this,&OpenCVManager::handleQueueWriterResult);
    connect(queueReader,&QueueReader::resultReady,this,&OpenCVManager::handleQueueReaderResult);

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
    cv::Mat frame = cv::Mat::zeros(300,300,CV_8UC3);
    std::string label = "Press 'q' for exit";
    int baseLine;
    cv::Size labelSize = getTextSize(label, cv::FONT_HERSHEY_SIMPLEX, 0.5, 1, &baseLine);
    auto top = cv::max(0, labelSize.height);
    cv::rectangle(frame, cv::Point(0, top - labelSize.height),
              cv::Point(0 + labelSize.width, top + baseLine), cv::Scalar::all(255), cv::FILLED);
    cv::putText(frame,label,cv::Point(0,top),cv::FONT_HERSHEY_SIMPLEX,0.5,cv::Scalar());
    cv::imshow("Object detection with DNN",frame);
}

void OpenCVManager::processVideoFrames()
{
    qDebug() << "Main thread id is: " << QThread::currentThreadId();
    emit lockMainWindow(false);
    process = true;
    queueWriter->moveToThread(&queueWriterThread);
    queueReader->moveToThread(&queueReaderThread);
    queueWriterThread.start();
    queueReaderThread.start();
    emit startQueueWriterThread(&videoCapture,&framesQueue);
    emit startQueueReaderThread(&framesQueue,deepNetworkManager);
    //обработка кадров с предсказанными вероятностями обнаружения
    while(true)
    {
        if((char)cv::waitKey(10) == 'q')
        {
            stopCapturingFrames();
            emit lockMainWindow(true);
            break;
        }
        //показ фреймов с обнаруженными объектами
        if(deepNetworkManager->getNumberOfPredictedFrames() > 1)
        {
            auto cur_frame = deepNetworkManager->getPredictedFrame();
            if(!cur_frame.empty())
            {
                //qDebug() << "Predictions FPS is: " << deepNetworkManager->getPredictionsFPS();
                cv::imshow("Object detection with DNN",cur_frame);
            }
        }
        //qDebug() << "Read frames FPS is: " << framesQueue.getFPS();
        QEventLoop evLoop;
        evLoop.processEvents();
    }
}

void OpenCVManager::stopCapturingFrames()
{
    process = false;
    emit stopWritingProcedure();
}

void OpenCVManager::releaseResources()
{
    queueWriter->stopProcessLoop();
    queueReader->stopProcessLoop();
    videoCapture.release();
    cv::destroyAllWindows();
}

void OpenCVManager::handleQueueWriterResult(const QString &result)
{
    qDebug() << result;
}

void OpenCVManager::handleQueueReaderResult(const QString &result)
{
    qDebug() << "result";
}

void QueueWriter::writeToQueue(cv::VideoCapture *cap, FPSQueue<cv::Mat> *framesQueue)
{
    qDebug() << "QueueWriter thread id is: " << QThread::currentThreadId();
    cv::Mat frame;
    setProcessState(true);
    while (isInProcessState())
    {
        *cap >> frame;
        if(!frame.empty())
        {
            framesQueue->push(frame.clone());
        }
        else
        {
            emit resultReady("Frame is empty");
            break;
        }
        //для того, чтобы не зависал пользовательский интерфейс, нужно обрабатывать возникающие события
        QEventLoop evLoop;
        evLoop.processEvents();
    }
    emit resultReady("Normal exit from queue writing thread procedure");
    emit terminateThread();
}

void QueueReader::readFromQueue(FPSQueue<cv::Mat> *framesQueue,QSharedPointer<DeepNeuralNetworManager> dnnManager)
{
    qDebug() << "QueueReader thread id is: " << QThread::currentThreadId();
    cv::Mat frame;
    while(isInProcessState())
    {
        if(!framesQueue->isEmpty())
        {
            frame = framesQueue->get();

            framesQueue->clear();
            dnnManager->processFrame(frame,cv::Size(416,416),1/255.0,cv::Scalar());
        }
        //для того, чтобы не зависал пользовательский интерфейс, нужно обрабатывать возинкающие события
        QEventLoop evLoop;
        evLoop.processEvents();
    }
    emit resultReady("Normal exit from queue reading thread procedure");
    emit terminateThread();
}
