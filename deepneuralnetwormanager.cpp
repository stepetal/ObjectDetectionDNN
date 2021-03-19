#include "deepneuralnetwormanager.h"
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>


DeepNeuralNetworManager::DeepNeuralNetworManager()
{
}

void DeepNeuralNetworManager::loadNet(QString modelPath, QString configPath)
{
    net = cv::dnn::readNet(modelPath.toStdString(),configPath.toStdString());

    //net = cv::dnn::readNetFromTensorflow("G:/Stepanov/Projects/ComputerVision/ObjectDetectionProject/ObjectDetectionProject/opencv_face_detector_uint8.pb",
    //                                     "G:/Stepanov/Projects/ComputerVision/ObjectDetectionProject/ObjectDetectionProject/opencv_face_detector.pbtxt");
    unconnectedOutLayersNames = net.getUnconnectedOutLayersNames();
    qDebug() << "Network has been loaded";
}

void DeepNeuralNetworManager::preprocess(const cv::Mat &frame,
                                         cv::Size in_size,
                                         float scale,
                                         const cv::Scalar& mean,
                                         bool swapRB)
{
    if(net.empty())
    {
        return;
    }
    if (in_size.width <= 0)
    {
        in_size.width = frame.cols;
    }
    if (in_size.height <= 0)
    {
        in_size.height = frame.rows;
    }
    cv::dnn::blobFromImage(frame,blob,1.0,in_size,cv::Scalar(),swapRB,false,CV_8U);

    net.setInput(blob,"",scale,mean);

    if(net.getLayer(0)->outputNameToIndex("im_info") != -1)
    {
        cv::resize(frame,frame,in_size);
        cv::Mat im_info = (cv::Mat_<float>(1,3) << in_size.height,in_size.width,1.6f);
        net.setInput(im_info,"im_info");
    }
}
