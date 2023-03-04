#ifndef SINGLEMODETHREAD_H
#define SINGLEMODETHREAD_H

#include <QThread>

class SingleModeThread : public QThread
{
    Q_OBJECT
public:
    explicit SingleModeThread(QObject *parent = 0);

public:
    void run();
};


#endif // SINGLEMODETHREAD_H
