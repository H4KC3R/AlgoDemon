#include "viewportcontroller.h"
#include <QGraphicsSceneMouseEvent>
#include <QCursor>


BEGIN_DORORO_NAMESPACE


ViewportController::ViewportController(QGraphicsItem *parent):
    QObject(nullptr), QGraphicsRectItem(parent)
{
    //setAcceptHoverEvents(true);
}

/*!
 * \brief В отличае от дефолтной реализации QGraphicsRectItem, устанавливает
 * только размеры прямоугольника с последующим смещением, позволяя избежать
 * проблем с неверным определением координат при перемещении.
 * \param r
 */
void ViewportController::setRect(const QRectF &r)
{
    ViewportController::setRect(r.x(), r.y(), r.width(), r.height());
}

void ViewportController::setRect(qreal x, qreal y, qreal width, qreal height)
{
    QGraphicsRectItem::setRect(0, 0, width, height);
    setPos(x, y);
    emit viewportChanged(rect());
}

QRectF ViewportController::rect() const
{
    QRectF ret = QGraphicsRectItem::rect();
    ret.moveTo(scenePos());
    return ret;
}

qreal ViewportController::width() const
{
    return QGraphicsRectItem::rect().width();
}

qreal ViewportController::height() const
{
    return QGraphicsRectItem::rect().height();
}


void ViewportController::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if (mIsMovableByMouse)
    {
        setPos(event->scenePos() - mShift);
        emit viewportChanged(rect());
        //event->accept();
        //QGraphicsRectItem::mouseMoveEvent(event);
    }
}

void ViewportController::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    Q_UNUSED(event);
    if (mIsMovableByMouse)
    {
        setCursor(QCursor(Qt::SizeAllCursor));
        mShift = event->pos();
    }
}

void ViewportController::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    Q_UNUSED(event);
    if (mIsMovableByMouse)
        unsetCursor();
}

/*
void ViewportController::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    Q_UNUSED(event);
    updateCursor(event->pos());

}

void ViewportController::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    Q_UNUSED(event);
    unsetCursor();
}

void ViewportController::hoverMoveEvent(QGraphicsSceneHoverEvent *event)
{
    Q_UNUSED(event);
    updateCursor(event->pos());
}

void ViewportController::updateCursor(const QPointF& p)
{
    constexpr int delta = 7;
    bool leftBoarder = abs(p.x() - 0) < delta;
    bool rightBoarder = abs(p.x() - width()) < delta;
    bool topBoarder = abs(p.y() - 0) < delta;
    bool bottomBoarder = abs(p.y() - height()) < delta;
    if ((leftBoarder && topBoarder) || (rightBoarder && bottomBoarder))
        setCursor(QCursor(Qt::SizeFDiagCursor));
    else if ((leftBoarder && bottomBoarder) || (rightBoarder && topBoarder))
        setCursor(QCursor(Qt::SizeBDiagCursor));
    else if (leftBoarder || rightBoarder)
        setCursor(QCursor(Qt::SizeHorCursor));
    else if (bottomBoarder || topBoarder)
        setCursor(QCursor(Qt::SizeVerCursor));
    else
        unsetCursor();
}
*/



END_DORORO_NAMESPACE
