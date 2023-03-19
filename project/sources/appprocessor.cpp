#include "appprocessor.h"

AppProcessor::AppProcessor() {
    framePipeline = new FramePipeline();

    AutoExposureParams autoExposureParameters;
    autoExposureParameters.maxPercent = 50;
    autoExposureParameters.mean = 100;
    autoExposureParameters.maxRelCoeff = 50;
    autoExposureParameters.minRelCoef = 100;

    objectiveThread = new ObjectiveThread(framePipeline, true,
                                          0, 0, 0, 0,
                                          autoExposureParameters);

    isCamConnected = false;
}

AppProcessor::~AppProcessor() {
    if(isCamConnected)
        disconnectCamera();

    deleteFramePipeline();
    deleteObjectiveThread();
}

bool AppProcessor::connectToCamera(char *id, StreamMode mode) {
    cameraThread = new CameraThread(framePipeline);

    // Подключаемся к камере
    if(cameraThread->connectToCamera(id, mode)) {
        double step, val;
        double maxGain, minGain, maxExposure, minExposure;

        CamParameters camParameters = cameraThread->getParams();
        cameraThread->getControlSettings(gain, minGain, maxGain, step, val);
        cameraThread->getControlSettings(exposure, minExposure, maxExposure, step, val);

        objectiveThread->autoExposureHandler->setMinGain(minGain);
        objectiveThread->autoExposureHandler->setMaxGain(maxGain);

        objectiveThread->autoExposureHandler->setMinExposure(minExposure);
        objectiveThread->autoExposureHandler->setMaxExposure(maxExposure);

        objectiveThread->setIsMono(camParameters.mIsMono);

        processingThread = new ProcessingThread(framePipeline, camParameters.mIsMono);
        isCamConnected = true;
        return true;
    }
    else {
        deleteCameraThread();
        isCamConnected = false;
        return false;
    }
}

void AppProcessor::disconnectCamera() {
    if(objectiveThread->isRunning())
        stopObjectiveThread();
    if(processingThread->isRunning())
        stopProcessingThread();
    if(cameraThread->isRunning())
        stopCameraThread();

    clearFramePipeline();
    cameraThread->disconnectCamera();
    isCamConnected = false;

    deleteCameraThread();
    deleteProcessingThread();
    deleteObjectiveThread();

    deleteFramePipeline();
}

bool AppProcessor::runProcess() {
    if(processingThread->isRunning() || cameraThread->isRunning())
        return false;

    framePipeline->clearBuffer();

    framePipeline->activatePipelineRead(true);
    cameraThread->start();
    processingThread->start();
    objectiveThread->start();
    return true;
}

void AppProcessor::stopProcess() {
    if(processingThread->isRunning() || cameraThread->isRunning()) {
        stopObjectiveThread();
        stopProcessingThread();
        stopCameraThread();
        emit processFinished();
    }
}

void AppProcessor::stopCameraThread() {
    cameraThread->stopLiveCaptureThread();
    framePipeline->activatePipelineRead(false);
    cameraThread->wait();
}

void AppProcessor::stopProcessingThread() {
    processingThread->stopProcessingThread();
    processingThread->wait();
}

void AppProcessor::stopObjectiveThread() {
    objectiveThread->stopObjectiveThread();
    objectiveThread->wait();
}

void AppProcessor::deleteCameraThread() {
    delete cameraThread;
}

void AppProcessor::deleteProcessingThread() {
    delete processingThread;
}

void AppProcessor::deleteObjectiveThread() {
    delete objectiveThread;
}

void AppProcessor::clearFramePipeline() {
    framePipeline->clearBuffer();
}

void AppProcessor::deleteFramePipeline() {
    if(framePipeline) {
        delete framePipeline;
        framePipeline = nullptr;
    }
}


