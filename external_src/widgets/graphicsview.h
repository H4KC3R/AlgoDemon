#ifndef GRAPHICSVIEW_H
#define GRAPHICSVIEW_H

#include "../common.h"
#include <QGraphicsView>


BEGIN_DORORO_NAMESPACE


class GraphicsView : public QGraphicsView
{
    Q_OBJECT
public:
    explicit GraphicsView(QWidget* parent = nullptr);
    void mouseMoveEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
    auto viewportRect() const -> QRectF;
public slots:
    void setViewportRect(const QRectF& rect);
signals:
    void mouseRightButtonClick(QMouseEvent*);
    void mouseScenePosition(QPoint);
    void viewportChanged(QRectF);
private slots:
    void scalingTime(qreal x);
    void animFinished();

private:
    double mFactor = 1.;
    QPointF mVpos;
    QPointF mSpos;
};

END_DORORO_NAMESPACE
#endif // GRAPHICSVIEW_H
