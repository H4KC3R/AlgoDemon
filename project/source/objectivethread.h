#ifndef OBJECTIVETHREAD_H
#define OBJECTIVETHREAD_H

#include <QObject>
#include <QMutex>
#include <QThread>
#include <QtGui>

#include "framepipeline.h"
#include "objectivecontroller.h"
#include "autoexposurehandler.h"
#include "imageblurmetric.h"

class ObjectiveThread : public QThread
{
    Q_OBJECT

public:
    ObjectiveThread(FramePipeline* frame, AutoExposureParams params);
    ~ObjectiveThread();
    AutoExposureHandler* autoExposureHandler;

public:
    double findZero();

    bool connectObjective(char* serialPort);
    bool disconnectObjective();
    string setDiaphragmLevel(double value);
    string setFocusing(int value);
    string getCurrentFocusing(double& value);

    void stopFocusingThread();

private:
    volatile bool stopped;
    QImage qFrame;
    cv::Mat cvFrame;

    FramePipeline* pFramePipeline;

    QMutex stoppedMutex;
    QMutex objectiveControlMutex;
    QMutex updateSettingsMutex;

    ObjectiveController* pObjective;

    double mCurrentExposure;
    double mCurrentGain;

    ////////////////  Флаги /////////////////
    bool mAutoExposureOn;
    bool mFocusingOn;
    ////////////////////////////////////////

private slots:
    void onAutoExposureEnabled(double status, double gain, double exposure);
    void focusingEnabled(bool status);

signals:
    void newFocusingResult(const QImage& frame,double position);
    void newEGValues(double gain, double exposure);
    void error(QString error);

    // QThread interface
protected:
    void run();
};

#endif // OBJECTIVETHREAD_H
