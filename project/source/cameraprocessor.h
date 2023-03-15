#ifndef CAMERAPROCESSOR_H
#define CAMERAPROCESSOR_H

#include <QObject>

#include "framepipeline.h"

#include "camerathread.h"
#include "processingthread.h"

class CameraProcessor : public QObject
{
    Q_OBJECT
public:
    CameraProcessor();
    ~CameraProcessor();
    FramePipeline* framePipeline;

    CameraThread *cameraThread;
    ProcessingThread* processingThread;
    FocusingThread* focusingThread;

    bool connectToCamera(char* id, StreamMode mode);
    void disconnectCamera();

    bool runProcess();
    void stopProcess();

    void stopCameraThread();
    void stopProcessingThread();

    void startFocusingThread();
    void stopFocusingThread();

    void deleteCameraThread();
    void deleteProcessingThread();
    void deleteFocusingThread();

    void clearFramePipeline();
    void deleteFramePipeline();

signals:
    void processFinished();

};

#endif // CAMERAPROCESSOR_H
