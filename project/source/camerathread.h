#ifndef CAMERATHREAD_H
#define CAMERATHREAD_H
#include <QThread>
#include "cameraqhyccd.h"
#include "imagepipeline.h"

class CameraThread : public QThread
{
    Q_OBJECT

public:
    CameraThread(ImagePipeline* pipeline);
    bool connectToCamera(char* id, StreamMode mode);
    void disconnectCamera();

    void stopCaptureThread();
    bool isCameraConnected();
    void updateFPS(int);

    int getAvgFPS();

private:
    ImagePipeline* pImagePipeline;
    CameraQHYCCD* pCamera;
    int avgFPS;

    // QThread interface
protected:
    void run();
};

#endif // CAMERATHREAD_H
