#include "camerathread.h"

CameraThread::CameraThread()
{

}

bool CameraThread::connectToCamera(char *id, StreamMode mode) {
    try {
        pCamera = new CameraQHYCCD(id);
    }
    catch(...){
        return false;
    }

    if(pCamera->connect(mode)) {
        isConnected = false;
        return true;
    }

    else
        return false;
}

bool CameraThread::isCameraConnected() {
    return isConnected;
}

void CameraThread::run()
{

}
