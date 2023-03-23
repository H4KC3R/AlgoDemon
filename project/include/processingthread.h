#ifndef PROCESSINGTHREAD_H
#define PROCESSINGTHREAD_H

#include <QObject>
#include <QMutex>
#include <QThread>
#include <QtGui>

#include "structures.h"
#include "framepipeline.h"

#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/xphoto/white_balance.hpp>
#include <opencv2/imgproc.hpp>

class ProcessingThread : public QThread
{
    Q_OBJECT
public:
    ProcessingThread(FramePipeline* pipeline, bool isMonoFlag);

    ~ProcessingThread();

    void startSingleProcess();
    void stopProcessingThread();

private:
    FramePipeline* pFramePipeline;
    QImage qFrame;

    QMutex stoppedMutex;
    QMutex updateMembersMutex;
    volatile bool stopped;

    cv::Mat cvFrame;
    cv::Mat cvProcessedFrame;

    ////////////////  Флаги /////////////////
    bool isMono;
    bool debayerOn;
    bool whiteBalanceOn;
    bool contrastOn;
    bool gammaContrastOn;

    double contrastValue;
    double gammaContrastValue;
    ////////////////////////////////////////

    // QThread interface
protected:
    void run();

private slots:
    void updateImageProcessingSettings(ImageProcessingFlags imageProcessingFlags);

signals:
    void newFrame(const QImage &frame);
};

#endif // PROCESSINGTHREAD_H
