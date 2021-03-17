#include "deepneuralnetwormanager.h"

DeepNeuralNetworManager::DeepNeuralNetworManager() : net(cv::dnn::Net())
{

}

void DeepNeuralNetworManager::loadNet(QString modelPath, QString configPath)
{
    net = cv::dnn::readNet(modelPath.toStdString(),configPath.toStdString());
    unconnectedOutLayersNames = net.getUnconnectedOutLayersNames();
}
