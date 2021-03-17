#include "opencvmanager.h"


OpenCVManager::OpenCVManager() : videoCapture(cv::VideoCapture())
{

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
