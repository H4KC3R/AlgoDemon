#include "singlemodethread.h"

SingleModeThread::SingleModeThread(QObject *parent) :
    QThread(parent) {

}

void SingleModeThread::run() {
    uint32_t ret = QHYCCD_ERROR;

    memset(cam.ImgData,0,cam.length*2);
//    memset(cam.ImgDataSave,0,cam.length);
    ret = GetQHYCCDSingleFrame(camhandle,&cam.imagew,&cam.imageh,&cam.bpp,&cam.channels,cam.ImgData);
    if(ret == QHYCCD_SUCCESS)
    {
        if(cam.canTransferData = true)
        {
            memset(cam.ImgDataSave,0,cam.length*2);
            memcpy(cam.ImgDataSave,cam.ImgData,cam.length*2);
        }
        cam.getData = 1;
    }
}
