#include "processingthread.h"

#include "mattoqimage.h"
#include "imageprocess.h"

ProcessingThread::ProcessingThread(FramePipeline *pipeline, bool isMonoFlag) : QThread(),
    pFramePipeline(pipeline), isMono(isMonoFlag)
{
    debayerOn = false;
    whiteBalanceOn = false;
    contrastOn = false;
    gammaContrastOn = false;

    contrastValue = 1.0;
    gammaContrastValue = 1.0;
    stopped = false;
}

ProcessingThread::~ProcessingThread() {
}

void ProcessingThread::startSingleProcess() {
    auto frame = pFramePipeline->getFirstFrame();
    updateMembersMutex.lock();
    int type = ImageProcess::getOpenCvType((BitMode)frame->mBpp, frame->mChannels);
    cvFrame = cv::Mat(frame->mHeight, frame->mWidth, type, frame->pData);
    cvProcessedFrame = cvFrame.clone();

    ////////////////////////////////////
    // Обработка изображения //
    ////////////////////////////////////
    if(!isMono && debayerOn)
        ImageProcess::debayerImg(cvFrame, cvFrame);

    if(!isMono && whiteBalanceOn)
        ImageProcess::whiteBalanceImg(cvFrame, cvFrame);

    if(!isMono && contrastOn)
        ImageProcess::contrastImg(cvFrame, cvFrame, contrastValue);

    if(!isMono && gammaContrastOn)
        ImageProcess::gammaContrastImg(cvFrame, cvFrame, gammaContrastValue);

    ////////////////////////////////////
    // Конец //
    ////////////////////////////////////
    qFrame = MatToQImage(cvFrame, cvFrame.type());
    updateMembersMutex.unlock();

    emit newFrame(qFrame);
}

void ProcessingThread::stopProcessingThread() {
    stoppedMutex.lock();
    stopped=true;
    stoppedMutex.unlock();
}

void ProcessingThread::run() {
    auto frame = pFramePipeline->getFirstFrame();
    while(1) {
        ////////////////  Остановка потока  /////////////////
        stoppedMutex.lock();
        if (stopped) {
            stopped=false;
            stoppedMutex.unlock();
            break;
        }
        stoppedMutex.unlock();
        /////////////////////////////////////////////////////////////

        updateMembersMutex.lock();
        // При остановке CameraThread, кадры не помещаются в pipeline
        // и pipeline закрыт на чтение
        if(!pFramePipeline->getPipelineActive()) {
            updateMembersMutex.unlock();
            continue;
        }

        int type = ImageProcess::getOpenCvType((BitMode)frame->mBpp, frame->mChannels);
        cvFrame = cv::Mat(frame->mHeight, frame->mWidth, type, frame->pData).clone();

        ////////////////////////////////////
        // Обработка изображения //
        ////////////////////////////////////
        if(!isMono && debayerOn)
            ImageProcess::debayerImg(cvFrame, cvFrame);

        if(!isMono && whiteBalanceOn)
            ImageProcess::whiteBalanceImg(cvFrame, cvFrame);

        if(!isMono && contrastOn)
            ImageProcess::contrastImg(cvFrame, cvFrame, contrastValue);

        if(!isMono && gammaContrastOn)
            ImageProcess::gammaContrastImg(cvFrame, cvFrame, gammaContrastValue);

        ////////////////////////////////////
        // Конец //
        ////////////////////////////////////

        qFrame = MatToQImage(cvFrame, cvFrame.type());
        updateMembersMutex.unlock();

        emit newFrame(qFrame);

        frame = pFramePipeline->nextFrame(frame);
    }
}

void ProcessingThread::updateImageProcessingSettings(ImageProcessingFlags imageProcessingFlags) {
    QMutexLocker locker(&updateMembersMutex);
    this->debayerOn = imageProcessingFlags.debayerOn;
    this->whiteBalanceOn = imageProcessingFlags.whiteBalanceOn;
    this->contrastOn = imageProcessingFlags.contrastOn;
    this->gammaContrastOn = imageProcessingFlags.gammaContrastOn;

    this->contrastValue = imageProcessingFlags.contrastValue;
    this->gammaContrastValue = imageProcessingFlags.gammaContrastValue;
}
