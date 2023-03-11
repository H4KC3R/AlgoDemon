#include "imagepipeline.h"

ImagePipeline::ImagePipeline(size_t size) : mSize(size) {
    frameCount = 1;
}

ImagePipeline::~ImagePipeline() {
}

void ImagePipeline::setFrame(CamFrame &frame) {
    std::unique_lock lock(mMutex);
    if(mList.size() == mSize){
        mList.pop_front();
        mList.push_back(CamFrame(frame));
    }
    else
        mList.push_back(CamFrame(frame));

    frameCount++;
}

int ImagePipeline::getPipelineSize() {
    return mSize;
}

const std::list <CamFrame>::iterator ImagePipeline::getFirstFrame() {
    std::list <CamFrame>::iterator it = mList.begin();
    while(it == mList.end()) {
        std::shared_lock lock(mMutex);
        if(mList.size() != 0) {
            it = mList.begin();
        }
    }
    return it;
}

const std::list <CamFrame>::iterator ImagePipeline::nextFrame(const std::list <CamFrame>::iterator& it) {
    std::list <CamFrame>::iterator next = std::next(it, 1);
    while (next == mList.end())
    {
        std::shared_lock lock(mMutex);
        next = std::next(it, 1);
    }
    return next;
}

void ImagePipeline::clearBuffer() {
    if(mList.size() != 0) {
        std::unique_lock lock(mMutex);
        frameCount = 1;
        mList.clear();
    }
}

uint32_t ImagePipeline::getFrameCount() const {
    return frameCount;
}



