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
            if(isEGChanged) {
                isEGChanged = false;
                pCamera->setExposure(exposureToSet);
                pCamera->setGain(gainToSet);
            }

            if(isBitChanged) {
                isBitChanged = false;
                pCamera->setImageBitMode(bitToSet);
            }

            if(roiChanged) {
                roiChanged = false;
                pCamera->setImageSize(roiToSet.startX, roiToSet.startY,
                                      roiToSet.sizeX, roiToSet.sizeY);
            }

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

        if(!params.mIsLiveMode)
            pCamera->setImageBitMode(bit16);

        isLive = params.mIsLiveMode;
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

bool CameraThread::getControlSettings(CameraControls control, double& min, double& max, double& step, double& currentVal) {
    switch(control){
    case transferbit:
        if(!isLive)
            pCamera->setImageBitMode(bit16);
        currentVal = pCamera->getImageBitMode();
        bitToSet = (BitMode)currentVal;
        break;
    case gain:
        currentVal = pCamera->getGain();
        gainToSet = currentVal;
        break;
    case exposure:
        currentVal = pCamera->getExposure();
        exposureToSet = currentVal;
        break;
    }

    bool result = pCamera->getControlMinMaxStep(control, min, max, step);
    return result;
}

void CameraThread::startSingleCapture() {
    ////////////////  Установка настроек камеры  /////////////////
    if(isEGChanged) {
        isEGChanged = false;
        pCamera->setExposure(exposureToSet);
        pCamera->setGain(gainToSet);
    }

    if(isBitChanged) {
        isBitChanged = false;
        pCamera->setImageBitMode(bitToSet);
    }

    if(roiChanged) {
        roiChanged = false;
        pCamera->setImageSize(roiToSet.startX, roiToSet.startY,
                              roiToSet.sizeX, roiToSet.sizeY);
    }

    /////////////////////////////////////////////////////////////

    if(pCamera->startSingleCapture()) {
        pCamera->getImage(frame->w, frame->h, frame->bpp,
                           frame->channels, frame->data);
        pFramePipeline->setFrame(frame);
    }
}

void CameraThread::stopLiveCaptureThread() {
    stoppedMutex.lock();
    stopped = true;
    stoppedMutex.unlock();
}

bool CameraThread::isCameraConnected() {
    return isConnected;
}

bool CameraThread::getIsMono() const {
    return isMono;
}

bool CameraThread::getIsLive() const {
    return isLive;
}

void CameraThread::onEGChanged(double gain, double exposure) {
    QMutexLocker locker(&updateControlsMutex);
    this->gainToSet = gain;
    this->exposureToSet = exposure;
    this->isEGChanged = true;
}

void CameraThread::onDepthChanged(BitMode bit) {
    QMutexLocker locker(&updateControlsMutex);
    this->bitToSet = bit;
    this->isBitChanged = true;
}

void CameraThread::onRoiChanged(RoiBox roi) {
    QMutexLocker locker(&updateControlsMutex);
    CamParameters params = pCamera->getCameraParameters();

    // Если все нули
    if((roi.startX + roi.startY + roi.sizeX + roi.sizeY) == 0) {
        roi.startX  = 0;
        roi.startY = 0;
        roi.sizeX = params.mMaximgw;
        roi.sizeY = params.mMaximgh;
        this->roiChanged = true;
    }
    else if((roi.startX + roi.sizeX <= params.mMaximgw) && (roi.startY + roi.sizeY <= params.mMaximgh)) {
        this->roiToSet = roi;
        this->roiChanged = true;
    }
    else
        emit error("Заданный размер выходит за пределы допустимого размера!\n");
}
