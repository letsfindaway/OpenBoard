#ifndef UBVERTICALHANDLE_H
#define UBVERTICALHANDLE_H

#include "UBAbstractHandle.h"

class UBVerticalHandle : public UBAbstractHandle
{
public:
    UBVerticalHandle(bool paintIndicators = false);

    UBVerticalHandle(UBVerticalHandle* const src);

    UBItem *deepCopy() const;
    void copyItemParameters(UBItem *copy) const;

protected:
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

private:
    bool mPaintIndicators{false};
};

#endif // UBVERTICALHANDLE_H
