#ifndef DEEPNEURALNETWORMANAGER_H
#define DEEPNEURALNETWORMANAGER_H

#include <QObject>
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
    std::vector<std::string> unconnectedOutLayersNames;
public:
    DeepNeuralNetworManager();

    void loadNet(QString modelPath,QString configPath);

};

#endif // DEEPNEURALNETWORMANAGER_H
