#include "UB1HEditableGraphicsCircleItem.h"

UB1HEditableGraphicsCircleItem::UB1HEditableGraphicsCircleItem(QGraphicsItem* parent):
    UB1HEditableGraphicsBasicShapeItem(parent)
{
    initializeStrokeProperty();
    initializeFillingProperty();

    mRadius = 0;
    wIsNeg = false;
    hIsNeg = false;
}

UB1HEditableGraphicsCircleItem::~UB1HEditableGraphicsCircleItem()
{

}

UBItem *UB1HEditableGraphicsCircleItem::deepCopy() const
{
    UB1HEditableGraphicsCircleItem* copy = new UB1HEditableGraphicsCircleItem();

    copyItemParameters(copy);

    return copy;
}

void UB1HEditableGraphicsCircleItem::copyItemParameters(UBItem *copy) const
{
    UB1HEditableGraphicsBasicShapeItem::copyItemParameters(copy);

    UB1HEditableGraphicsCircleItem *cp = dynamic_cast<UB1HEditableGraphicsCircleItem*>(copy);

    if(!cp) return;

    cp->mRadius = mRadius;
    cp->wIsNeg = wIsNeg;
    cp->hIsNeg = hIsNeg;
}

QPointF UB1HEditableGraphicsCircleItem::center() const
{
    QPointF centre;

    centre.setX(pos().x() + mRadius);
    centre.setY(pos().y() + mRadius);

    return centre;
}

void UB1HEditableGraphicsCircleItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option)
    Q_UNUSED(widget)

    UBAbstractGraphicsItem::setStyle(painter);

    auto rx = wIsNeg ? -mRadius : mRadius;
    auto ry = hIsNeg ? -mRadius : mRadius;

    auto x = wIsNeg ? -mRadius : 0;
    auto y = hIsNeg ? -mRadius : 0;

    x *= 2;
    y *= 2;

    //N/C - NNE - 20140312 : Litle work around for avoid crash under MacOs 10.9
    QPainterPath path;
    path.addEllipse(QRect(x, y, mRadius*2, mRadius*2));

    painter->drawPath(path);

    if(isInEditMode()){
        QPen p;
        p.setColor(QColor(128, 128, 200));
        p.setStyle(Qt::DotLine);
        p.setWidth(pen().width());

        painter->setPen(p);
        painter->setBrush(QBrush());

        painter->drawRect(0, 0, rx*2, ry*2);
    }
}

QRectF UB1HEditableGraphicsCircleItem::boundingRect() const
{
    auto x = wIsNeg ? -mRadius : 0;
    auto y = hIsNeg ? -mRadius : 0;

    x *= 2;
    y *= 2;

    QRectF rect(x, y, mRadius*2, mRadius*2);

    rect = adjustBoundingRect(rect);

    if(isInEditMode()){
        qreal r = getHandle()->radius();

        rect.adjust(-r, -r, r, r);
    }

    return rect;
}

void UB1HEditableGraphicsCircleItem::onActivateEditionMode()
{
    getHandle()->setPos(mRadius*2, mRadius*2);
}

void UB1HEditableGraphicsCircleItem::updateHandle(UBAbstractHandle *handle)
{
    prepareGeometryChange();

    qreal maxSize = handle->radius() * 4;

    qreal r = qMin(handle->pos().x(), handle->pos().y()) / 2;

    if(r >= maxSize){
        mRadius = r;
    }

    getHandle()->setPos(mRadius*2, mRadius*2);

    if(hasGradient()){
        QLinearGradient g(QPointF(), QPointF(mRadius*2, 0));

        g.setColorAt(0, brush().gradient()->stops().at(0).second);
        g.setColorAt(1, brush().gradient()->stops().at(1).second);

        setBrush(g);
    }
}

QPainterPath UB1HEditableGraphicsCircleItem::shape() const
{
    QPainterPath path;
    if(isInEditMode()){
        path.addRect(boundingRect());
    }else{
        path.addEllipse(boundingRect());
    }

    return path;
}

QPainterPath UB1HEditableGraphicsCircleItem::painterPath() const
{
    QPainterPath path;
    path.addEllipse({mRadius, mRadius}, mRadius, mRadius);
    return path;
}

void UB1HEditableGraphicsCircleItem::setRadius(qreal radius)
{
    prepareGeometryChange();
    mRadius = radius;
}

void UB1HEditableGraphicsCircleItem::setRect(QRectF rect)
{
    prepareGeometryChange();

    auto w = rect.width();
    auto h = rect.height();

    wIsNeg = (w < 0);
    hIsNeg = (h < 0);

    if(wIsNeg) w = -w;
    if(hIsNeg) h = -h;

    mRadius = qMin(w, h);
    mRadius /= 2;

    setPos(rect.topLeft());

    if(hasGradient()){
        QLinearGradient g(QPointF(), QPointF(mRadius*2, 0));

        g.setColorAt(0, brush().gradient()->stops().at(0).second);
        g.setColorAt(1, brush().gradient()->stops().at(1).second);

        setBrush(g);
    }
}

QRectF UB1HEditableGraphicsCircleItem::rect() const
{
    QRectF r;
    r.setTopLeft(pos());

    auto rx = wIsNeg ? -mRadius : mRadius;
    auto ry = hIsNeg ? -mRadius : mRadius;

    r.setWidth(rx*2);
    r.setHeight(ry*2);

    return r;
}

qreal UB1HEditableGraphicsCircleItem::radius() const
{
    return mRadius;
}
