#ifndef FPSQUEUE_H
#define FPSQUEUE_H

#include<opencv2/core.hpp>

#include <QQueue>
#include <QMutex>
#include <QMutexLocker>

/*
 * Класс, реализующий очередь для доступа к полученным из видопоследовательности фреймам
 */

template<typename T>
class FPSQueue : public QQueue<T>
{
public:
    unsigned int counter;
    FPSQueue() : counter(0) {}
    void push(const T& entry)
    {
        QMutexLocker lock(&mutex);
        QQueue::push_back(entry);
        counter++;
        if (counter == 1)
        {
            tick_meter.reset();
            tick_meter.start();
        }
    }

    T get()
    {
        QMutexLocker lock(&mutex);
        if(!isEmpty())
        {
            T entry = front();
            pop_front();
            return entry;
        }
    }

    float getFPS()
    {
        tick_meter.stop();
        auto fps = counter / tick_meter.getTimeSec();
        tick_meter.start();
        return static_cast<float>(fps);
    }

    void clear()
    {
        QMutexLocker lock(&mutex);
        while(!isEmpty())
        {
            pop_front();
        }
    }
private:
    cv::TickMeter tick_meter;
    QMutex mutex;
};


#endif // FPSQUEUE_H
