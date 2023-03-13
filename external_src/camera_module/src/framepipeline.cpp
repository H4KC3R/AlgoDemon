#include "framepipeline.h"

FramePipeline::FramePipeline(size_t size) : mSize(size) {
    frameCount = 1;
}

FramePipeline::~FramePipeline() {
}

void FramePipeline::setFrame(CamFrame &frame) {
    std::unique_lock lock(mMutex);
    if(mList.size() == mSize){
        mList.pop_front();
        mList.push_back(frame);
    }
    else
        mList.emplace_back(frame);

    frameCount++;
}

int FramePipeline::getPipelineSize() {
    return mSize;
}

const std::list <CamFrame>::iterator FramePipeline::getFirstFrame() {
    std::list <CamFrame>::iterator it = mList.begin();
    while(it == mList.end() && pipelineActive) {
        std::shared_lock lock(mMutex);
        if(mList.size() != 0) {
            it = mList.begin();
        }
    }
    return it;
}

const std::list <CamFrame>::iterator FramePipeline::nextFrame(const std::list <CamFrame>::iterator& it) {
    std::list <CamFrame>::iterator next = std::next(it, 1);
    while (next == mList.end() && pipelineActive)
    {
        std::shared_lock lock(mMutex);
        next = std::next(it, 1);
    }
    return next;
}

void FramePipeline::clearBuffer() {
    if(mList.size() != 0) {
        std::unique_lock lock(mMutex);
        frameCount = 1;
        mList.clear();
    }
}

uint32_t FramePipeline::getFrameCount() const {
    return frameCount;
}

void FramePipeline::activatePipelineRead(bool activateFlag) {
    std::lock_guard<std::mutex> lock(activateFlagMutex);
    pipelineActive = activateFlag;
}



