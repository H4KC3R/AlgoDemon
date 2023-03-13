#ifndef PROCESSINGTHREAD_H
#define PROCESSINGTHREAD_H

#include <QObject>
#include <QMutex>
#include <QThread>
#include "structures.h"
#include "framepipeline.h"
#include "imageprocess.h"
#include "autoexposurehandler.h"

class ProcessingThread : public QThread
{
    Q_OBJECT
public:
    ProcessingThread(FramePipeline* pipeline, bool isMonoFlag,
                     double maxExposure, double minExposure, double maxGain,
                     double minGain, AutoExposureParams params);
    AutoExposureHandler* autoExposureHandler;

    ~ProcessingThread();
    void stopProcessingThread();

private:
    FramePipeline* pFramePipeline;
    QImage frame;

    QMutex stoppedMutex;
    QMutex updateMembersMutex;
    volatile bool stopped;

    double currentExposure;
    double currentGain;

    cv::Mat cvFrame;
    cv::Mat cvFrameGrayscale;

    ////////////////  Флаги /////////////////
    bool isMono;
    bool debayerOn;
    bool whiteBalanceOn;
    bool contrastOn;
    bool gammaContrastOn;
    bool autoExposureOn;

    double contrastParam;
    double gammaContrastParam;
    ////////////////////////////////////////

    // QThread interface
protected:
    void run();

private slots:
    void updateImageProcessingSettings(ImageProcessingFlags imageProcessingFlags);
    void onAutoExposureEnabled(double gain, double exposure);

signals:
    void newFrame(const QImage &frame);
    void newEGValues(double gain, double exposure);
    void error(QString errorMessage);
};

#endif // PROCESSINGTHREAD_H
