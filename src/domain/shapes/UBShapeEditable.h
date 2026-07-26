#ifndef UBSHAPEEDITABLE_H
#define UBSHAPEEDITABLE_H

#include "UBAbstractGraphicsItem.h"
#include "UBEditable.h"

class UBAbstractEditableGraphicsShapeItem : public UBAbstractGraphicsItem, public UBAbstractEditable
{
public:
    UBAbstractEditableGraphicsShapeItem(QGraphicsItem *parent = 0);

    virtual ~UBAbstractEditableGraphicsShapeItem(){ }
protected:
    virtual QPainterPath shape() const override;

    virtual void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;
    virtual void focusOutEvent(QFocusEvent *event) override;
    virtual void paintCenterMark(QPainter *painter);

    virtual void onActivateEditionMode();

    virtual void deactivateEditionMode() override;

    int mMultiClickState;

    bool mHasMoved;
};

#endif // UBSHAPEEDITABLE_H
