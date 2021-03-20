#include "deepneuralnetwormanager.h"
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>


DeepNeuralNetworManager::DeepNeuralNetworManager() : threshold(0.25)
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

void DeepNeuralNetworManager::testNetOnImage(QString imagePath)
{
    cv::Mat frame = cv::imread(imagePath.toStdString(),cv::IMREAD_COLOR);
    if(!frame.empty())
    {
        //cv::resize(frame,frame,cv::Size(416,416));
        //cv::imshow("Image after resize",frame);
        processFrame(frame,cv::Size(416,416),1/255.0,cv::Scalar());
        cv::imshow("Image",frame);
    }
}

void DeepNeuralNetworManager::readFileWithClassesNames(QString filePath)
{
    QFile fileWithClassesNames(filePath);
    if(fileWithClassesNames.open(QFile::ReadOnly))
    {
        QTextStream textStream(&fileWithClassesNames);
        while(!textStream.atEnd())
        {
            auto curLine = textStream.readLine();
            if(!curLine.isEmpty())
            {
                classes.append(curLine);
            }
        }
    }
}

void DeepNeuralNetworManager::preprocess(cv::Mat &frame,
                                         cv::Size in_size,
                                         float scale,
                                         const cv::Scalar& mean)
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
    cv::dnn::blobFromImage(frame,blob,scale,in_size,cv::Scalar(),true,false);

    net.setInput(blob);

    if(net.getLayer(0)->outputNameToIndex("im_info") != -1)
    {
        cv::resize(frame,frame,in_size);
        cv::Mat im_info = (cv::Mat_<float>(1,3) << in_size.height,in_size.width,1.6f);
        net.setInput(im_info,"im_info");
    }
}

void DeepNeuralNetworManager::postprocess(cv::Mat &frame, const std::vector<cv::Mat> &outs)
{

    static std::vector<int> outLayers = net.getUnconnectedOutLayers();
    static std::string outLayerType = net.getLayer(outLayers[0])->type;

    std::vector<int> classIds;
    std::vector<float> confidences;
    std::vector<cv::Rect> boxes;
    if (outLayerType == "DetectionOutput")
    {
        // Network produces output blob with a shape 1x1xNx7 where N is a number of
        // detections and an every detection is a vector of values
        // [batchId, classId, confidence, left, top, right, bottom]
        CV_Assert(outs.size() > 0);
        for (size_t k = 0; k < outs.size(); k++)
        {
            float* data = (float*)outs[k].data;
            for (size_t i = 0; i < outs[k].total(); i += 7)
            {
                float confidence = data[i + 2];
                if (confidence > threshold)
                {
                    int left   = (int)data[i + 3];
                    int top    = (int)data[i + 4];
                    int right  = (int)data[i + 5];
                    int bottom = (int)data[i + 6];
                    int width  = right - left + 1;
                    int height = bottom - top + 1;
                    if (width <= 2 || height <= 2)
                    {
                        left   = (int)(data[i + 3] * frame.cols);
                        top    = (int)(data[i + 4] * frame.rows);
                        right  = (int)(data[i + 5] * frame.cols);
                        bottom = (int)(data[i + 6] * frame.rows);
                        width  = right - left + 1;
                        height = bottom - top + 1;
                    }
                    classIds.push_back((int)(data[i + 1]) - 1);  // Skip 0th background class id.
                    boxes.push_back(cv::Rect(left, top, width, height));
                    confidences.push_back(confidence);
                }
            }
        }
    }
    else if (outLayerType == "Region")
    {
        for (size_t i = 0; i < outs.size(); ++i)
        {
            // Network produces output blob with a shape NxC where N is a number of
            // detected objects and C is a number of classes + 4 where the first 4
            // numbers are [center_x, center_y, width, height]
            float* data = (float*)outs[i].data;
            for (int j = 0; j < outs[i].rows; ++j, data += outs[i].cols)
            {
                cv::Mat scores = outs[i].row(j).colRange(5, outs[i].cols);
                cv::Point classIdPoint;
                double confidence;
                minMaxLoc(scores, 0, &confidence, 0, &classIdPoint);
                if (confidence > threshold)
                {
                    int centerX = (int)(data[0] * frame.cols);
                    int centerY = (int)(data[1] * frame.rows);
                    int width = (int)(data[2] * frame.cols);
                    int height = (int)(data[3] * frame.rows);
                    int left = centerX - width / 2;
                    int top = centerY - height / 2;

                    classIds.push_back(classIdPoint.x);
                    confidences.push_back((float)confidence);
                    boxes.push_back(cv::Rect(left, top, width, height));
                }
            }
        }
    }
    else
        CV_Error(cv::Error::StsNotImplemented, "Unknown output layer type: " + outLayerType);


    for (size_t idx = 0; idx < boxes.size(); ++idx)
    {
        cv::Rect box = boxes[idx];
        drawPredictions(classIds[idx], confidences[idx], box.x, box.y,
                 box.x + box.width, box.y + box.height, frame);
    }
}

void DeepNeuralNetworManager::drawPredictions(int classId, float conf, int left, int top, int right, int bottom, cv::Mat &frame)
{
    cv::rectangle(frame, cv::Point(left, top), cv::Point(right, bottom), cv::Scalar(0, 255, 0));

    std::string label = cv::format("%.2f", conf);
    if (!classes.isEmpty())
    {
        CV_Assert(classId < (int)classes.length());
        label = classes.at(classId).toStdString() + ": " + label;
    }

    int baseLine;
    cv::Size labelSize = getTextSize(label, cv::FONT_HERSHEY_SIMPLEX, 0.5, 1, &baseLine);

    top = cv::max(top, labelSize.height);
    cv::rectangle(frame, cv::Point(left, top - labelSize.height),
              cv::Point(left + labelSize.width, top + baseLine), cv::Scalar::all(255), cv::FILLED);
    putText(frame, label, cv::Point(left, top), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar());
}

void DeepNeuralNetworManager::processFrame(cv::Mat &frame, cv::Size in_size, float scale, const cv::Scalar &mean)
{
    preprocess(frame,in_size,scale,mean);
    std::vector<cv::Mat> outs;
    net.forward(outs,unconnectedOutLayersNames);
    postprocess(frame,outs);
}


