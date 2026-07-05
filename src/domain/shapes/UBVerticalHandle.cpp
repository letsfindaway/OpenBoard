#include "UBEditable.h"

#include "UBVerticalHandle.h"

UBVerticalHandle::UBVerticalHandle(bool paintIndicators)
    : mPaintIndicators{paintIndicators}
{
    mId = paintIndicators ? Other : Vertical;
}

UBVerticalHandle::UBVerticalHandle(UBVerticalHandle* const src):
    UBAbstractHandle(src)
  , mPaintIndicators{src->mPaintIndicators}
{

}

void UBVerticalHandle::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    QPointF p = parentItem()->mapFromScene(event->scenePos());

    this->setPos(pos().x(), p.y());

    mEditableObject->updateHandle(this);
    mEditableObject->setModified();
}

void UBVerticalHandle::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    UBAbstractHandle::mousePressEvent(event);
}

void UBVerticalHandle::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    UBAbstractHandle::mouseReleaseEvent(event);
}

void UBVerticalHandle::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    UBAbstractHandle::paint(painter, option, widget);

    if (mPaintIndicators)
    {
        const auto height = mRadius + 2;
        const auto width = height;
        const auto gap = 5;

        QPainterPath path;

        path.moveTo(-width, -mRadius - gap);
        path.lineTo(0, -mRadius - height - gap);
        path.lineTo(width, -mRadius - gap);
        path.closeSubpath();

        path.moveTo(-width, mRadius + gap);
        path.lineTo(0, mRadius + height + gap);
        path.lineTo(width, mRadius + gap);
        path.closeSubpath();

        painter->drawPath(path);
    }
}

UBItem *UBVerticalHandle::deepCopy() const
{
    UBVerticalHandle * copy = new UBVerticalHandle();

    copyItemParameters(copy);

    return copy;
}

void UBVerticalHandle::copyItemParameters(UBItem *copy) const
{
    UBVerticalHandle *cp = dynamic_cast<UBVerticalHandle*>(copy);
    if (cp)
    {
        cp->setTransform(this->transform());
        cp->setFlag(QGraphicsItem::ItemIsMovable, true);
        cp->setFlag(QGraphicsItem::ItemIsSelectable, true);
        cp->setData(UBGraphicsItemData::ItemLayerType, this->data(UBGraphicsItemData::ItemLayerType));
        cp->setData(UBGraphicsItemData::ItemLocked, this->data(UBGraphicsItemData::ItemLocked));

        cp->setPos(pos());
        cp->setEditableObject(cp->editableObject());
    }
}
