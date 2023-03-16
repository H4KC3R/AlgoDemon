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

    contrastParam = 1;
    gammaContrastParam = 1;
    stopped = false;
}

ProcessingThread::~ProcessingThread() {
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
            qDebug() << "stopped";
            stoppedMutex.unlock();
            break;
        }
        stoppedMutex.unlock();
        /////////////////////////////////////////////////////////////

        updateMembersMutex.lock();
        int type = ImageProcess::getOpenCvType((BitMode)frame->mBpp, frame->mChannels);
        cvFrame = cv::Mat(frame->mHeight, frame->mWidth, type, frame->pData);

        ////////////////////////////////////
        // Обработка изображения //
        ////////////////////////////////////
        qDebug() << "Debayer";
        if(!isMono && debayerOn)
            ImageProcess::debayerImg(cvFrame, cvFrame);

        if(!isMono && whiteBalanceOn)
            ImageProcess::whiteBalanceImg(cvFrame, cvFrame);

        if(!isMono && contrastOn)
            ImageProcess::contrastImg(cvFrame, cvFrame, contrastParam);

        if(!isMono && gammaContrastOn)
            ImageProcess::gammaContrastImg(cvFrame, cvFrame, gammaContrastParam);

        ////////////////////////////////////
        // Конец //
        ////////////////////////////////////

        qDebug() << "processing";
        qFrame = MatToQImage(cvFrame);
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

    this->contrastParam = imageProcessingFlags.contrastParam;
    this->gammaContrastParam = imageProcessingFlags.gammaContrastParam;
}
