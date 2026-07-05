#ifndef UB3HANDLESEDITABLE_H
#define UB3HANDLESEDITABLE_H

#include "UBShapeEditable.h"

class UB3HEditablesGraphicsBasicShapeItem : public UBAbstractEditableGraphicsShapeItem
{
public:
    UB3HEditablesGraphicsBasicShapeItem(QGraphicsItem *parent = 0);

    QRectF adjustBoundingRect(QRectF rect) const;
};

#endif // UB3HANDLESEDITABLE_H
