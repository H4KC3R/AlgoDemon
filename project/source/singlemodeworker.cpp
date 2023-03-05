#include "singlemodeworker.h"

SingleModeWorker::SingleModeWorker(QObject *parent)
    : QObject{parent}
{

}

void SingleModeWorker::setMutex(QMutex *newMutex) {
    mMutex = newMutex;
}

void SingleModeWorker::setCamera(CameraQHYCCD *newCamera) {
    mCamera = newCamera;
}

void SingleModeWorker::run() {
    if(mMutex == NULL || mCamera == NULL)
        emit errorOccured();

    QMutexLocker locker(mMutex);
    CamImage myImg;
    myImg.length = mCamera->getImgLength();

    uint8_t* data = new uint8_t[myImg.length * 2];
    mCamera->getImage(myImg.w, myImg.h, myImg.bpp, myImg.channels, data);
}
