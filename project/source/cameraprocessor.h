#ifndef CAMERAPROCESSOR_H
#define CAMERAPROCESSOR_H

#include <QObject>

#include "imagepipeline.h"

#include "camerathread.h"
#include "processingthread.h"
#include "focusingthread.h"

class CameraProcessor : public QObject
{
    Q_OBJECT
public:
    CameraProcessor();
    ~CameraProcessor();
    ImagePipeline *imagePipeline;

    CameraThread *cameraThread;
    ProcessingThread* processingThread;
    FocusingThread* focusingThread;

    bool connectToCamera(char* id, );
    void disconnectCamera();

    void startCaptureThread();
    void stopCaptureThread();

    void stopProcessingThread();
    void stopFocusingThread();

    void deleteCaptureThread();
    void deleteProcessingThread();
    void deleteFocusingThread();

private:

signals:

};

#endif // CAMERAPROCESSOR_H
