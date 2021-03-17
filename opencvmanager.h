#ifndef OPENCVMANAGER_H
#define OPENCVMANAGER_H

#include <opencv2/dnn.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include "globals.h"
#include <QObject>



/*
 * Класс, реализующий функционал по обработке запросов к видеоустройству
 */

class OpenCVManager : public QObject
{
    Q_OBJECT
    cv::VideoCapture videoCapture;
public:
    OpenCVManager();
    ~OpenCVManager(){}
    bool openVideoCaputureDevice(SYS::VideoInputEnum inputType,QString fileName = "",uint8_t numb = 0);
};

#endif // OPENCVMANAGER_H
