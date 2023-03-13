#include "cameraprocessor.h"

CameraProcessor::CameraProcessor() {
}

CameraProcessor::~CameraProcessor() {
    if(cameraThread->isCameraConnected())
        disconnectCamera();
}

bool CameraProcessor::connectToCamera(char *id, StreamMode mode) {
    framePipeline = new FramePipeline(imageBufferSize,dropFrame);
    cameraThread = new CameraThread(imageBuffer);

    // Подключаемся к камере
    if(cameraThread->connectToCamera(id, mode)) {
        double step, val;
        double maxGain, minGain, maxExposure, minExposure;

        cameraThread->getControlSettings(gain, minGain, maxGain, step, val);
        cameraThread->getControlSettings(exposure, minExposure, maxExposure, step, val);

        AutoExposureParams params{};
        processingThread = new ProcessingThread(imageBuffer, cameraThread->getIsMono(),
                                                maxExposure, minExposure,
                                                maxGain, minGain, params);
        return true;
    }
    else {
        deleteCaptureThread();
        deleteImageBuffer();
        return false;
    }
}

void CameraProcessor::disconnectCamera() {
    if(processingThread->isRunning())
        stopProcessingThread();
    if(cameraThread->isRunning())
        stopCameraThread();

    clearFramePipeline();
    captureThread->disconnectCamera();

    deleteCameraThread();
    deleteProcessingThread();
    deleteFramePipeline();
}

void CameraProcessor::runProcess() {
    if(processingThread->isRunning() || cameraThread->isRunning())
        return;

    framePipeline->clearBuffer();

    framePipeline->activatePipelineRead(true);
    cameraThread->start();
    processingThread->start();
}

void CameraProcessor::stopCameraThread() {
    captureThread->stopCaptureThread();
    framePipeline->activatePipelineRead(false);
    captureThread->wait();
}

void CameraProcessor::stopProcessingThread() {
    processingThread->stopProcessingThread();
    processingThread->wait();
}

void CameraProcessor::deleteCameraThread() {
    delete cameraThread;
}

void CameraProcessor::deleteProcessingThread() {
    delete processingThread;
}

void CameraProcessor::deleteFocusingThread() {
    delete focusingThread;
}

void CameraProcessor::clearFramePipeline() {
    framePipeline->clearBuffer();
}

void CameraProcessor::deleteFramePipeline() {
    delete framePipeline;
}


