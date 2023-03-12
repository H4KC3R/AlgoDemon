#include "camerathread.h"

CameraThread::CameraThread(FramePipeline *pipeline) : QThread(),
    pFramePipeline(pipeline)
{
    stopped = false;
}

void CameraThread::run() {
    if(myCamera->startLiveCapture()) {
        bool frameReady = false;
        while(1) {
            ////////////////  Остановка потока  /////////////////
            stoppedMutex.lock();
            if (stopped) {
                stopped=false;
                pCamera->stopLiveCapture();
                stoppedMutex.unlock();
                break;
            }
            stoppedMutex.unlock();
            /////////////////////////////////////////////////////////////

            ////////////////  Установка настроек камеры  /////////////////
            updateControlsMutex.lock();

            updateMembersMutex.unlock();
            /////////////////////////////////////////////////////////////

            while(frameReady == false)
                frameReady = pCamera->getImage(frame->w, frame->h, frame->bpp,
                                               frame->channels, frame->data);
            pFramePipeline->setFrame(frame);
            frameReady = false;
        }
    }
}

bool CameraThread::connectToCamera(char *id, StreamMode mode) {
    try {
        pCamera = new CameraQHYCCD(id);
    }
    catch(...){
        return false;
    }

    if(pCamera->connect(mode)) {
        isConnected = true;
        CamParameters params = myCamera->getCameraParameters();
        uint32_t length = params.mMaximgh * params.mMaximgw * 2;
        frame = new CamFrame(length);
        isMono = params.mIsMono;
        return true;
    }

    else
        return false;
}

void CameraThread::disconnectCamera() {
    if(isConnected) {
        pCamera->disconnect();
        delete pCamera;
        pCamera = nullptr;

        delete frame;
        frame = nullptr;
    }
}

void CameraThread::initializeControls(CameraControls control, double& min, double& max, double& step, double& currentVal) {
    if()

    bool result = (pCamera->getControlMinMaxStep(control, min, max, step);


}

void CameraThread::stopCaptureThread() {
    stoppedMutex.lock();
    stopped = true;
    stoppedMutex.unlock();
}

bool CameraThread::isCameraConnected() {
    return isConnected;
}


