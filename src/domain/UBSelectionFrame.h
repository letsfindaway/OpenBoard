#ifndef UBSELECTIONFRAME_H
#define UBSELECTIONFRAME_H

#include <QGraphicsRectItem>
#include <QtGui>

class UBGraphicsItemDelegate;

class UBSelectionFrame : public QObject, public QGraphicsRectItem
{
    Q_OBJECT

public:
    UBSelectionFrame();

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    QRectF boundingRect() const;
    QPainterPath shape() const;

    void setLocalBrush(const QBrush &pBrush) {mLocalBrush = pBrush;}
    QBrush localBrush() const {return mLocalBrush;}
    void setEnclosedItems(const QList<UBGraphicsItemDelegate*> pEncItems) {mEnclosedtems = pEncItems; updateRect();}
    void setEnclosedItems(const QList<QGraphicsItem*> pGraphicsItems);
    void updateRect();
    void updateScale();
    bool isEmpty() const {return this->rect().isEmpty();}

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);

private slots:
    void setAntiScaleRatio(qreal pAntiscaleRatio) {mAntiscaleRatio = pAntiscaleRatio;}
    void onZoomChanged(qreal pZoom);

private:
    void translateItem(QGraphicsItem *item, const QPointF &translatePoint);

private:
    int mThickness;
    qreal mAntiscaleRatio;
    QList<UBGraphicsItemDelegate*> mEnclosedtems;
    QBrush mLocalBrush;

    QPointF mPressedPos;
    QPointF mLastMovedPos;
    QPointF mLastTranslateOffset;
};

#endif // UBSELECTIONFRAME_H