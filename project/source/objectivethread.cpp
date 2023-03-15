#include "objectivethread.h"

ObjectiveThread::ObjectiveThread(FramePipeline* pipeline, AutoExposureParams params)
    : QThread(),
      pFramePipeline(pipeline)
{
    pObjective = nullptr;
    autoExposureHandler = new AutoExposureHandler(params);
    stopped = false;
    autoExposureOn = false;
    focusingOn = false;
}

ObjectiveThread::~ObjectiveThread() {
    delete autoExposureHandler;
    if(pObjective)
        delete pObjective;
}

double ObjectiveThread::findZero() {
    QMutexLocker locker(&objectiveControlMutex);
    return 0;
}

bool ObjectiveThread::connectObjective(char* serialPort) {
    QMutexLocker locker(&objectiveControlMutex);
    try {
        pObjective = new ObjectiveController(serialPort);
        if(pObjective->connectToController(serialPort))
            return true;
        else
            return false;
    }
    catch(...) {
        return false;
    }
}

bool ObjectiveThread::disconnectObjective() {
    QMutexLocker locker(&objectiveControlMutex);
    if(pObjective->disconnectController())
        return true;
    else
        return false;
}

string ObjectiveThread::setDiaphragmLevel(double value) {
    QMutexLocker locker(&objectiveControlMutex);
    pObjective->setDiaphragmLevel(value);
    return pObjective->currentError();
}

string ObjectiveThread::setFocusing(int value) {
    QMutexLocker locker(&objectiveControlMutex);
    pObjective->setFocusing(value);
    return pObjective->currentError();
}

string ObjectiveThread::getCurrentFocusing(double& value) {
    QMutexLocker locker(&objectiveControlMutex);
    value = pObjective->getCurrentFocusing();
    return pObjective->currentError();
}

void ObjectiveThread::stopFocusingThread() {
    stoppedMutex.lock();
    stopped=true;
    stoppedMutex.unlock();
}

void ObjectiveThread::onAutoExposureEnabled(double status, double gain, double exposure) {
    QMutexLocker locker(&updateSettingsMutex);
    this->mAutoExposureOn = status;
    this->mCurrentGain = gain;
    this->mCurrentExposure = exposure;

}

void ObjectiveThread::focusingEnabled(bool status) {
    QMutexLocker locker(&updateSettingsMutex);
    if(pObjective && pObjective->isContollerActive())
        this->mFocusingOn = status;
}

void ObjectiveThread::run() {
    auto frame = pFramePipeline->getFirstFrame();
    while(1) {
           ////////////////  Остановка потока  /////////////////
           stoppedMutex.lock();
           if (stopped) {
               stopped=false;
               qDebug() << "stopped";
               stoppedMutex.unlock();
               break;
           }
           stoppedMutex.unlock();
           /////////////////////////////////////////////////////////////

           updateSettingsMutex.lock();
           int type = ImageProcess::getOpenCvType((BitMode)frame->mBpp, frame->mChannels);
           cvFrame = cv::Mat(frame->mHeight, frame->mWidth, type, frame->pData);

           /// TO DO GRAYSCALE FORMAT

           ////////////////////////////////////
           // Обработка изображения //
           ////////////////////////////////////
           if(mAutoExposureOn) {
               if(autoExposureHandler->correct(cvFrame, mCurrentExposure, mCurrentGain)) {
                   currentGain = autoExposureHandler->getGain();
                   currentExposure = autoExposureHandler->getExposure();
                   emit newEGValues(currentGain, currentExposure);
               }
               else
                   emit error("Неверный формат изображения!\nТребуется: 8 бит");
           }

           if(mFocusingOn) {

           }

           ////////////////////////////////////
           // Конец //
           ////////////////////////////////////

           qDebug() << "Objective Thread";
           qFrame = MatToQImage(cvFrame);
           updateMembersMutex.unlock();

           emit newFrame(qFrame);
           frame = pFramePipeline->nextFrame(frame);
       }
}

