#include "UBAbstractEditableGraphicsPathItem.h"

// #include "UBFreeHandle.h"

UBAbstractEditableGraphicsPathItem::UBAbstractEditableGraphicsPathItem(QGraphicsItem *parent):
    UBAbstractGraphicsPathItem(parent)
{
    mHasMoved = false;
}

void UBAbstractEditableGraphicsPathItem::onActivateEditionMode()
{
    //NOOP
}

void UBAbstractEditableGraphicsPathItem::drawArrows()
{
    UBAbstractGraphicsPathItem::drawArrows();

    // Draw Arrows UNDER (z-order) Handles, if handles are shown.
    if (isInEditMode())
    {
        if (startArrowGraphicsItem()){
            startArrowGraphicsItem()->stackBefore(mHandles.first());
        }

        if (endArrowGraphicsItem()){
            endArrowGraphicsItem()->stackBefore(mHandles.last());
            endArrowGraphicsItem()->stackBefore(mHandles.first());
        }
    }
}

void UBAbstractEditableGraphicsPathItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    mHasMoved = false;
}

void UBAbstractEditableGraphicsPathItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    prepareGeometryChange();

    if (!mHasMoved)
    {
        if (!Delegate()->isLocked())
        {
            if (!isInEditMode())
            {
                onActivateEditionMode();

                Delegate()->showFrame(false);
                setFocus();
                showEditMode(true);
            }
            else
            {
                showEditMode(false);
                Delegate()->positionHandles();
                Delegate()->showFrame(true);
            }
        }
    }

    mHasMoved = false;
}

QRectF UBAbstractEditableGraphicsPathItem::boundingRect() const
{
    QRectF rect = path().boundingRect();

    rect = UBAbstractGraphicsPathItem::adjustBoundingRect(rect);

    if(isInEditMode()){
        qreal r = mHandles.first()->radius();

        rect.adjust(-r, -r, r, r);
    }

    return rect;
}

void UBAbstractEditableGraphicsPathItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if (!Delegate()->isLocked())
    {
        mHasMoved = true;

        if(!isInEditMode()){
            Delegate()->mouseMoveEvent(event);
        }
    }
}

void UBAbstractEditableGraphicsPathItem::focusOutEvent(QFocusEvent *event)
{
    Q_UNUSED(event)

    if (isInEditMode())
    {
        prepareGeometryChange();
        showEditMode(false);
    }
}

void UBAbstractEditableGraphicsPathItem::focusHandle(UBAbstractHandle *handle)
{
    Q_UNUSED(handle)

    Delegate()->showFrame(false);
}

void UBAbstractEditableGraphicsPathItem::deactivateEditionMode()
{
    prepareGeometryChange();

    showEditMode(false);
}

QPainterPath UBAbstractEditableGraphicsPathItem::shape() const
{
    QPainterPath path;
    if(isInEditMode()){
        path.addRect(boundingRect());
        return path;
    }else{
        return this->path();
    }
}
