#include "camerathread.h"

CameraThread::CameraThread(FramePipeline *pipeline) : QThread(),
    pFramePipeline(pipeline)
{
    stopped = false;
}

void CameraThread::run() {
    if(pCamera->startLiveCapture()) {
        fps = 0;
        frameCount = 0;
        t_start = time(NULL);

        bool frameReady = false;
        while(1) {
            /////////////////////  Остановка потока  //////////////////////
            stoppedMutex.lock();
            if (stopped) {
                stopped=false;
                pCamera->startLiveCapture();
                stoppedMutex.unlock();
                break;
            }
            stoppedMutex.unlock();
            //////////////////////////////////////////////////////////////

            updateControlsMutex.lock();
            ////////////////  Установка настроек камеры  /////////////////
            setSettings();
            /////////////////////////////////////////////////////////////
            updateControlsMutex.unlock();

            frame.mTime_start = time(NULL);
            while(frameReady == false) {
                if(stopped == true)
                    break;
                frameReady = pCamera->getImage(frame.mWidth, frame.mHeight, frame.mBpp,
                                               frame.mChannels, frame.pData);
            }

            frame.mTime_end = time(NULL);

            t_end = time(NULL);
            frameCount++;
            if(t_end - t_start >= 5) {
                fps = (double)frameCount / 5.0;
                t_start = time(NULL);
                frameCount = 0;
            }

            emit newFpsValue(fps, gainToSet, exposureToSet);

            pFramePipeline->setFrame(frame);
            frameReady = false;
        }
    }
    else
        emit hardFault("Невозможно начать съемку!\nПроверьте соединение");
}

bool CameraThread::connectToCamera(char *id, StreamMode mode, BitMode bit) {
    try {
        pCamera = new CameraQHYCCD(id);
    }
    catch(...){
        return false;
    }

    if(pCamera->connect(mode)) {
        isConnected = true;
        CamParameters params = pCamera->getCameraParameters();
        uint32_t length = params.mMaximgh * params.mMaximgw * 2;
        frame.allocateFrame(length);
        isLive = params.mIsLiveMode;

        if(!params.mIsLiveMode)
            pCamera->setImageBitMode(bit16);
        else
            pCamera->setImageBitMode(bit);
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
    }
}

bool CameraThread::getControlSettings(CameraControls control, double& min, double& max, double& step, double& currentVal) {
    if (control == usbtraffic) {
        currentVal = pCamera->getFps();
        fpsToSet = currentVal;
    }
    else if (control == gain){
        currentVal = pCamera->getGain();
        gainToSet = currentVal;
    }
    else if(control == exposure) {
        currentVal = pCamera->getExposure();
        exposureToSet = currentVal;
    }
    else
        return false;

    bool result = pCamera->getControlMinMaxStep(control, min, max, step);
    return result;
}

void CameraThread::stopLiveCaptureThread() {
    stoppedMutex.lock();
    stopped = true;
    stoppedMutex.unlock();
}

bool CameraThread::isCameraConnected() {
    return isConnected;
}

CamParameters CameraThread::getParams() const {
    return pCamera->getCameraParameters();
}

bool CameraThread::getIsLive() const {
    return isLive;
}

void CameraThread::setSettings() {
    if(isFpsChanged) {
        isFpsChanged = false;
        pCamera->setFps(fpsToSet);
    }

    if(isEGChanged) {
        isEGChanged = false;
        pCamera->setExposure(exposureToSet);
        pCamera->setGain(gainToSet);
    }

    if(isRoiChanged) {
        isRoiChanged = false;
        pCamera->setImageSize(roiToSet.startX, roiToSet.startY,
                              roiToSet.sizeX, roiToSet.sizeY);
    }
}

void CameraThread::onFpsChanged(double fps) {
    QMutexLocker locker(&updateControlsMutex);
    this->fpsToSet = fps;
    this->isFpsChanged = true;
}

void CameraThread::onEGChanged(double gain, double exposure) {
    QMutexLocker locker(&updateControlsMutex);
    this->gainToSet = gain;
    this->exposureToSet = exposure;
    this->isEGChanged = true;
}

void CameraThread::onRoiChanged(RoiBox roi) {
    QMutexLocker locker(&updateControlsMutex);
    CamParameters params = pCamera->getCameraParameters();

    // Если все нули
    if((roi.startX + roi.startY + roi.sizeX + roi.sizeY) == 0) {
        roiToSet.startX  = 0;
        roiToSet.startY = 0;
        roiToSet.sizeX = params.mMaximgw;
        roiToSet.sizeY = params.mMaximgh;
        this->isRoiChanged = true;
    }
    else if((roi.startX + roi.sizeX <= params.mMaximgw) && (roi.startY + roi.sizeY <= params.mMaximgh)) {
        this->roiToSet = roi;
        this->isRoiChanged = true;
    }
    else
        emit softFault("Заданный размер выходит за пределы допустимого размера!\n");
}
