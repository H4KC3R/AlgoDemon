#ifndef CAMERATHREAD_H
#define CAMERATHREAD_H
#include <QThread>
#include <QMutex>
#include "cameraqhyccd.h"
#include "imagepipeline.h"

class CameraThread : public QThread
{
    Q_OBJECT

public:
    CameraThread(ImagePipeline* pipeline);
    bool connectToCamera(char* id, StreamMode mode);
    void disconnectCamera();


    void startCaptureThread();
    void stopCaptureThread();
    bool isCameraConnected();

private:
    bool isConnected = false;
    ImagePipeline* pImagePipeline;
    CameraQHYCCD* pCamera;
    QMutex stoppedMutex;
    volatile bool stopped;

public slots:
    on_settingsChanged();

    // QThread interface
protected:
    void run();
};

#endif // CAMERATHREAD_H
