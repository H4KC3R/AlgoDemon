#ifndef OBJECTIVETHREAD_H
#define OBJECTIVETHREAD_H

#include <QObject>
#include <QMutex>
#include <QThread>
#include <QtGui>
#include <QVector>

#include "framepipeline.h"
#include "objectivecontroller.h"
#include "autoexposurehandler.h"
#include "focusingFSM.h"

class ObjectiveThread : public QThread
{
    Q_OBJECT

public:
    ObjectiveThread(FramePipeline* frame, bool isMonoFlag, double maxExposure, double minExposure,
                    double maxGain, double minGain,AutoExposureParams params);
    ~ObjectiveThread();

    AutoExposureHandler* autoExposureHandler;
    FocusingFSM* focusingFSM;

public:
    bool connectObjective(const char* serialPort);
    bool disconnectObjective();
    string setDiaphragmLevel(double value);
    string setFocusing(int value);
    string getCurrentFocusing(double& value);

    std::vector<double> getAppertureList();
    void setAppertureList(std::vector<double> appertures);

    void stopObjectiveThread();

    bool getIsMono() const;
    void setIsMono(bool newIsMono);

    bool focusingOn() const;

private:
    volatile bool stopped;
    QImage qFrame;
    cv::Mat cvFrame;
    cv::Mat cvProcessedFrame;

    FramePipeline* pFramePipeline;

    QMutex stoppedMutex;
    QMutex updateSettingsMutex;

    ObjectiveController* pObjective;

    double mCurrentExposure;
    double mCurrentGain;

    cv::Rect myRoi;

    double maxSharpnessMetric;
    int currentPosition;
    int sharpImagePositon;
    int step = 100;

    ////////////////  Флаги /////////////////
    bool isMono;
    bool mAutoExposureOn;
    bool mFocusingOn;
    ////////////////////////////////////////

private slots:
    void onAutoExposureEnabled(bool status, double gain, double exposure);
    void onAutoExposureSettingChanged(AutoExposureParams params);
    void onFocusingEnabled(bool status, cv::Rect roi);

signals:
    void newFocusingResult(const QImage& frame,double position);
    void newEGValues(double gain, double exposure);
    void focusingStop(QString msg);
    void autoExposureStop(QString msg);

    // QThread interface
protected:
    void run();
};

#endif // OBJECTIVETHREAD_H
