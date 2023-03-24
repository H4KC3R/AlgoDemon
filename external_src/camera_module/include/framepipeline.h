#ifndef FRAMEPIPELINE_H
#define FRAMEPIPELINE_H

#include "camframe.h"
#include <list>
#include <iterator>
#include <mutex>
#include <shared_mutex>

#include <iostream>

class FramePipeline
{
public:
    FramePipeline(size_t size = 5);

    ~FramePipeline();

    void setFrame(CamFrame& frame);

    const std::list <CamFrame>::iterator getFirstFrame();

    const std::list <CamFrame>::iterator nextFrame(const std::list <CamFrame>::iterator& it);

    int getPipelineSize();

    void clearBuffer();

    uint32_t getFrameCount() const;

    void activatePipelineRead(bool activateFlag);

    bool getPipelineActive() const;

private:
    size_t mSize;
    uint32_t frameCount;
    std::list<CamFrame> mList;
    std::shared_mutex mMutex;

    std::mutex activateFlagMutex;
    volatile bool pipelineActive;
};

#endif // FRAMEPIPELINE_H
