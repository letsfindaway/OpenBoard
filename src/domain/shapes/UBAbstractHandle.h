#ifndef UBABSTRACTHANDLE_H
#define UBABSTRACTHANDLE_H

#include <QGraphicsItem>
#include <QObject>

#include <QPainter>

#include "domain/UBItem.h"

class UBAbstractEditable;

enum HandleId
{
    Free,
    Horizontal,
    Vertical,
    Diagonal,
    Stretch,
    Other
};

class UBAbstractHandle : public QGraphicsItem, public UBItem
{
public:

    virtual ~UBAbstractHandle(){ }

    UBAbstractHandle(UBAbstractHandle* const src);

    void setId(int id)
    {
        mId = id;
    }

    int getId()
    {
        return mId;
    }

    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

    QRectF boundingRect() const override;

    void setEditableObject(UBAbstractEditable *eo)
    {
        mEditableObject = eo;
    }

    UBAbstractEditable *editableObject()
    {
        return mEditableObject;
    }

    // enum { Type = UBGraphicsItemType::GraphicsHandle };
    virtual int type() const  override { return Type; }

    int radius() const
    {
        return this->mRadius;
    }

    virtual std::shared_ptr<UBGraphicsScene> scene() override;

protected:
    UBAbstractHandle();

    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;

protected:
    int mId;
    bool mClick;
    int mRadius;

    UBAbstractEditable *mEditableObject;
};

#endif // UBABSTRACTHANDLE_H
