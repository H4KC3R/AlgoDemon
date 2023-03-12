#ifndef CAMERATHREAD_H
#define CAMERATHREAD_H
#include <QThread>
#include <QMutex>
#include "cameraqhyccd.h"
#include "framepipeline.h"

class CameraThread : public QThread
{
    Q_OBJECT

public:
    CameraThread(FramePipeline* pipeline);
    bool connectToCamera(char* id, StreamMode mode);
    void disconnectCamera();
    void initializeControls(CameraControls control, double& min, double& max, double& step, double& currentVal);

    void startSingleCapture();
    void stopCaptureThread();
    bool isCameraConnected();

private:
    bool isConnected = false;
    bool isMono;

    CameraQHYCCD* pCamera;
    FramePipeline* pFramePipeline;
    CamFrame* frame;
    QMutex stoppedMutex;
    QMutex updateControlsMutex;
    volatile bool stopped;

public slots:
    onControlChanged();

    // QThread interface
protected:
    void run();
};

#endif // CAMERATHREAD_H
