#include "cameraprocessor.h"

CameraProcessor::CameraProcessor() {
}

CameraProcessor::~CameraProcessor() {
    if(cameraThread->isCameraConnected())
        disconnectCamera();
}

bool CameraProcessor::connectToCamera(char *id, StreamMode mode) {
    framePipeline = new FramePipeline();
    cameraThread = new CameraThread(framePipeline);

    // Подключаемся к камере
    if(cameraThread->connectToCamera(id, mode)) {
        double step, val;
        double maxGain, minGain, maxExposure, minExposure;

        cameraThread->getControlSettings(gain, minGain, maxGain, step, val);
        cameraThread->getControlSettings(exposure, minExposure, maxExposure, step, val);

        AutoExposureParams autoExposureParameters;
        autoExposureParameters.maxPercent = 50;
        autoExposureParameters.mean = 100;
        autoExposureParameters.maxRelCoeff = 50;
        autoExposureParameters.minRelCoef = 100;

        CamParameters camParameters = cameraThread->getParams();

        processingThread = new ProcessingThread(framePipeline, camParameters.mIsMono,
                                                maxExposure, minExposure,
                                                maxGain, minGain, autoExposureParameters);
        return true;
    }
    else {
        deleteCameraThread();
        deleteFramePipeline();
        return false;
    }
}

void CameraProcessor::disconnectCamera() {
    if(processingThread->isRunning())
        stopProcessingThread();
    if(cameraThread->isRunning())
        stopCameraThread();

    clearFramePipeline();
    cameraThread->disconnectCamera();

    deleteCameraThread();
    deleteProcessingThread();
    deleteFramePipeline();
}

bool CameraProcessor::runProcess() {
    if(processingThread->isRunning() || cameraThread->isRunning())
        return false;

    framePipeline->clearBuffer();

    framePipeline->activatePipelineRead(true);
    cameraThread->start();
    processingThread->start();
    return true;
}

void CameraProcessor::stopProcess() {
    if(processingThread->isRunning() || cameraThread->isRunning()) {
        stopProcessingThread();
        stopCameraThread();
        emit processFinished();
    }
}

void CameraProcessor::stopCameraThread() {
    cameraThread->stopLiveCaptureThread();
    framePipeline->activatePipelineRead(false);
    cameraThread->wait();
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


