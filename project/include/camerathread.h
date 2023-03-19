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
    bool connectToCamera(char* id, StreamMode mode);
    void disconnectCamera();
    bool getControlSettings(CameraControls control, double& min, double& max, double& step, double& currentVal);

    void startSingleCapture();
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

    ////////////////  Флаги /////////////////
    volatile bool isBitChanged;
    BitMode bitToSet;

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
    void onBitChanged(BitMode bit);
    void onRoiChanged(RoiBox roi);

signals:
    void hardFault(QString errorMessage);
    void softFault(QString errorMessage);

    // QThread interface
protected:
    void run();
};

#endif // CAMERATHREAD_H
