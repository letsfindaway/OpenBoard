#include "UBFreeHandle.h"
#include "UBEditable.h"
#include "domain/UBGraphicsScene.h"

UBFreeHandle::UBFreeHandle()
{
    mId = Free;
}

UBFreeHandle::UBFreeHandle(UBFreeHandle* const src):
    UBAbstractHandle(src)
{
}

void UBFreeHandle::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    auto scenePos = event->scenePos();

    if (scene()->isSnapping())
    {
        scenePos += scene()->snap(scenePos);
    }

    QPointF p = parentItem()->mapFromScene(scenePos);

    QPointF diff = p - pos();

    moveBy(diff.x(), diff.y());

    mEditableObject->updateHandle(this);
    mEditableObject->setModified();
}

void UBFreeHandle::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    UBAbstractHandle::mousePressEvent(event);
}

void UBFreeHandle::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    UBAbstractHandle::mouseReleaseEvent(event);
}

UBItem *UBFreeHandle::deepCopy() const
{
    UBFreeHandle * copy = new UBFreeHandle();

    copyItemParameters(copy);

    return copy;
}

void UBFreeHandle::copyItemParameters(UBItem *copy) const
{
    UBFreeHandle *cp = dynamic_cast<UBFreeHandle*>(copy);
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
