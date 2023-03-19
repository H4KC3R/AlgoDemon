#ifndef APPPROCESSOR_H
#define APPPROCESSOR_H

#include <QObject>

#include "framepipeline.h"

#include "camerathread.h"
#include "processingthread.h"
#include "objectivethread.h"

class AppProcessor : public QObject
{
    Q_OBJECT
public:
    AppProcessor();
    ~AppProcessor();
    FramePipeline* framePipeline = nullptr;

    CameraThread *cameraThread;
    ProcessingThread* processingThread;
    ObjectiveThread* objectiveThread;

    bool connectToCamera(char* id, StreamMode mode);
    void disconnectCamera();

    bool runProcess();
    void stopProcess();

    void stopCameraThread();
    void stopProcessingThread();
    void stopObjectiveThread();

    void deleteCameraThread();
    void deleteProcessingThread();
    void deleteObjectiveThread();

    void clearFramePipeline();
    void deleteFramePipeline();

private:
    bool isCamConnected;

signals:
    void processFinished();
};

#endif // APPPROCESSOR_H
