#ifndef SINGLEMODEWORKER_H
#define SINGLEMODEWORKER_H

#include <QObject>
#include <QMutex>
#include <QMutexLocker>
#include "cameraqhyccd.h"
#include "imageprocess.h"

class SingleModeWorker : public QObject
{
    Q_OBJECT
public:
    explicit SingleModeWorker(QObject *parent = nullptr);

    void setMutex(QMutex *newMutex);
    void setCamera(CameraQHYCCD *newCamera);

signals:
    void imageProcessed(CamImage img);
    void finished();
    void errorOccured();

public slots:
    void run();

private:
    QMutex* mMutex;
    CameraQHYCCD* mCamera;

};

#endif // SINGLEMODEWORKER_H
