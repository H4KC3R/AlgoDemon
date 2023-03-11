#include "processingthread.h"

ProcessingThread::ProcessingThread()
{

}

void ProcessingThread::run() {
    while(1) {
        // Поток остановиться если stopped=TRUE //
        stoppedMutex.lock();
        if (stopped)
        {
            stopped=false;
            stoppedMutex.unlock();
            break;
        }
        stoppedMutex.unlock();



    }

}
