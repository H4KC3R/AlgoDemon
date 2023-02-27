#ifndef VIEWPORTCONTROLLER_H
#define VIEWPORTCONTROLLER_H

#include "../common.h"
#include <QObject>
#include <QGraphicsRectItem>


BEGIN_DORORO_NAMESPACE

class ViewportController : public QObject, public QGraphicsRectItem
{
    Q_OBJECT
    bool mIsMovableByMouse = true;
    //bool mIsResizableByMouse = true;

    QPointF mShift = {0,0};

public:
    explicit ViewportController(QGraphicsItem *parent = nullptr);

    void setMovableByMouse(bool flag) {mIsMovableByMouse = flag;}
    bool isMovableByMouse() const     {return mIsMovableByMouse;}
//    void setResizableByMouse(bool flag) {mIsResizableByMouse = flag;}
//    bool isResizableByMouse() const     {return mIsResizableByMouse;}

    auto rect() const -> QRectF;
    qreal width() const;
    qreal height() const;

    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;
public slots:
    void setRect(const QRectF &rectangle);
    void setRect(qreal x, qreal y, qreal width, qreal height);

signals:
    void viewportChanged(QRectF);
};

END_DORORO_NAMESPACE

#endif // VIEWPORTCONTROLLER_H
