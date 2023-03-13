#ifndef CAMERATHREAD_H
#define CAMERATHREAD_H
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

    bool getIsMono() const;
    bool getIsLive() const;

private:
    bool isConnected = false;
    bool isMono;
    bool isLive;

    CameraQHYCCD* pCamera;
    FramePipeline* pFramePipeline;
    CamFrame* frame;
    QMutex stoppedMutex;
    QMutex updateControlsMutex;
    volatile bool stopped;

    ////////////////  Флаги /////////////////
    volatile bool isEGChanged = false;
    double gainToSet;
    double exposureToSet;

    volatile bool isBitChanged;
    BitMode bitToSet;

    volatile bool roiChanged;
    RoiBox roiToSet;
    ////////////////////////////////////////

public slots:
    void onEGChanged(double gain, double exposure);
    void onDepthChanged(BitMode bit);
    void onRoiChanged(RoiBox roi);

signals:
    void error(QString errorMessage);

    // QThread interface
protected:
    void run();
};

#endif // CAMERATHREAD_H
