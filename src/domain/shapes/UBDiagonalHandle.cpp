#include "UBDiagonalHandle.h"

#include "UBEditable.h"
#include "domain/UBGraphicsScene.h"

UBDiagonalHandle::UBDiagonalHandle()
{
    mId = Diagonal;
}

UBDiagonalHandle::UBDiagonalHandle(UBDiagonalHandle* const src):
    UBAbstractHandle(src)
{
}

void UBDiagonalHandle::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    auto scenePos = event->scenePos();

    if (scene()->isSnapping())
    {
        scenePos += scene()->snap(scenePos);
    }

    QPointF p = parentItem()->mapFromScene(scenePos);

    QPointF diff(p - pos());
    moveBy(diff.x(), diff.y());
    mEditableObject->updateHandle(this);
    mEditableObject->setModified();
}

void UBDiagonalHandle::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    UBAbstractHandle::mousePressEvent(event);
}

void UBDiagonalHandle::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    UBAbstractHandle::mouseReleaseEvent(event);
}

UBItem *UBDiagonalHandle::deepCopy() const
{
    UBDiagonalHandle * copy = new UBDiagonalHandle();

    copyItemParameters(copy);

    return copy;
}

void UBDiagonalHandle::copyItemParameters(UBItem *copy) const
{
    UBDiagonalHandle *cp = dynamic_cast<UBDiagonalHandle*>(copy);
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
