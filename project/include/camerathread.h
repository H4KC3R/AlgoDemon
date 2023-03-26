#ifndef CAMERATHREAD_H
#define CAMERATHREAD_H
#include <QCoreApplication>
#include <QThread>
#include <QMutex>
#include "structures.h"
#include "cameraqhyccd.h"
#include "framepipeline.h"

class CameraThread : public QThread
{
    Q_OBJECT

public:
    CameraThread(FramePipeline* pipeline);
    bool connectToCamera(char* id, StreamMode mode, BitMode bit);
    void disconnectCamera();
    bool getControlSettings(CameraControls control, double& min, double& max, double& step, double& currentVal);

    void stopLiveCaptureThread();
    bool isCameraConnected();

    CamParameters getParams() const;
    bool getIsLive() const;

private:
    bool isConnected = false;
    bool isLive = false;

    CameraQHYCCD* pCamera;
    FramePipeline* pFramePipeline;
    CamFrame frame;
    QMutex stoppedMutex;
    QMutex updateControlsMutex;
    volatile bool stopped;

    time_t t_start;
    time_t t_end;
    uint32_t frameCount = 0;
    double fps = 0;

    ////////////////  Флаги /////////////////
    volatile bool isFpsChanged;
    double fpsToSet;

    volatile bool isEGChanged = false;
    double gainToSet;
    double exposureToSet;

    volatile bool isRoiChanged;
    RoiBox roiToSet;
    ////////////////////////////////////////

private:
    void setSettings();

public slots:
    void onFpsChanged(double fps);
    void onEGChanged(double gain, double exposure);
    void onRoiChanged(RoiBox roi);

signals:
    void hardFault(QString errorMessage);
    void softFault(QString errorMessage);
    void newFpsValue(double fps, double gain, double exposure);

    // QThread interface
protected:
    void run();
};

#endif // CAMERATHREAD_H
