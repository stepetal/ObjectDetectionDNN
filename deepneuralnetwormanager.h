#ifndef DEEPNEURALNETWORMANAGER_H
#define DEEPNEURALNETWORMANAGER_H

#include <QObject>
#include <QFile>
#include <QTextStream>
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
    float threshold;
    cv::Mat image;
    QList<QString> classes;
public:
    DeepNeuralNetworManager();
    void setThreshold(float thrsh) {threshold = thrsh;}
    void loadNet(QString modelPath,QString configPath);
    void processFrame(cv::Mat &frame, cv::Size in_size, float scale, const cv::Scalar& mean);
    void drawPredictions(int classId,float conf,int left,int top,int right,int bottom,cv::Mat& frame);
    void testNetOnImage(QString imagePath);
    bool networkIsValid(){ return !net.empty();}
    void readFileWithClassesNames(QString filePath);
protected:
    void preprocess(cv::Mat &frame, cv::Size in_size, float scale, const cv::Scalar& mean);
    void postprocess(cv::Mat& frame, const std::vector<cv::Mat>& outs);



};



#endif // DEEPNEURALNETWORMANAGER_H
