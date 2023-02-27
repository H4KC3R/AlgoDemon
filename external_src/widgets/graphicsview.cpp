#include "graphicsview.h"
#include <QWheelEvent>
#include <QMouseEvent>
#include <QTimeLine>

BEGIN_DORORO_NAMESPACE



GraphicsView::GraphicsView(QWidget *parent):
    QGraphicsView(parent)
{
    setDragMode(QGraphicsView::ScrollHandDrag);
    setMouseTracking(true);
    setCursor(Qt::ArrowCursor);
    setResizeAnchor(QGraphicsView::AnchorUnderMouse);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
}

void GraphicsView::mouseMoveEvent(QMouseEvent *event)
{
    if(event->buttons() & Qt::LeftButton)
    {
        QGraphicsView::mouseMoveEvent(event);
        emit viewportChanged(viewportRect());
    }
    QPointF pos = mapToScene(event->pos());
    int x = trunc(pos.rx());
    int y = trunc(pos.ry());

    emit mouseScenePosition({x,y});
    QGraphicsView::mouseMoveEvent(event);

}

void GraphicsView::wheelEvent(QWheelEvent *event)
{
    double angle = event->angleDelta().y();
    mFactor = qPow(1.0003, angle);

    mVpos = event->position();
    mSpos = mapToScene(QPoint(mVpos.x(), mVpos.y()));

    QTimeLine* mAnim  = new QTimeLine(350, this);
    mAnim->setUpdateInterval(20);
    connect(mAnim, SIGNAL (valueChanged(qreal)), SLOT (scalingTime(qreal)));
    connect(mAnim, SIGNAL (finished()), SLOT (animFinished()));
    mAnim->start();
}

void GraphicsView::mousePressEvent(QMouseEvent *event)
{
    QGraphicsView::mousePressEvent(event);
    if (event->button() == Qt::RightButton)
        emit mouseRightButtonClick(event);
}

void GraphicsView::resizeEvent(QResizeEvent *event)
{
    QGraphicsView::resizeEvent(event);
    emit viewportChanged(viewportRect());
}

QRectF GraphicsView::viewportRect() const
{
    return mapToScene(viewport()->geometry()).boundingRect();
}

void GraphicsView::setViewportRect(const QRectF &rect)
{
    ensureVisible(rect, 0, 0);
    emit viewportChanged(viewportRect());
}

void GraphicsView::scalingTime(qreal x) {
    Q_UNUSED(x);
    scale(mFactor, mFactor);
    centerOn(mSpos);
    QPointF deltaViewportPos = mVpos - QPointF(viewport()->width() / 2.0, viewport()->height() / 2.0);
    QPointF viewportCenter = mapFromScene(mSpos) - deltaViewportPos;
    centerOn(mapToScene(viewportCenter.toPoint()));
    emit viewportChanged(viewportRect());
}

void GraphicsView::animFinished() {
    sender()->deleteLater();
}



END_DORORO_NAMESPACE
