#ifndef DEEPNEURALNETWORMANAGER_H
#define DEEPNEURALNETWORMANAGER_H

#include <QObject>
#include <QDebug>
#include <QVector>
#include <QSharedPointer>
#include <opencv2/dnn.hpp>



/*
 * Класс, реализующий функционал по работе с глубокими нейронными сетями
 */

class DeepNeuralNetworManager : public QObject
{
    Q_OBJECT
    cv::dnn::Net net;
    cv::Mat blob;
    std::vector<std::string> unconnectedOutLayersNames;
public:
    DeepNeuralNetworManager();
    void loadNet(QString modelPath,QString configPath);
    void preprocess(const cv::Mat& frame, cv::Size in_size, float scale, const cv::Scalar& mean, bool swapRB);

};

#endif // DEEPNEURALNETWORMANAGER_H
