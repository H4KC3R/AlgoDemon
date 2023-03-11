#ifndef PROCESSINGTHREAD_H
#define PROCESSINGTHREAD_H

#include <QObject>
#include <QMutex>
#include <QThread>

class ProcessingThread : public QThread
{
    Q_OBJECT
public:
    ProcessingThread();

private:
    bool stopped;
    QMutex stoppedMutex;
    QMutex updateMembersMutex;




    // QThread interface
protected:
    void run();
};

#endif // PROCESSINGTHREAD_H
