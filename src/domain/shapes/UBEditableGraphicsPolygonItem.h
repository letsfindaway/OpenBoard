#ifndef UBGRAPHICSPATHITEM_H
#define UBGRAPHICSPATHITEM_H

#include "UBAbstractEditableGraphicsPathItem.h"

class UBEditableGraphicsPolygonItem : public UBAbstractEditableGraphicsPathItem
{
public:
    UBEditableGraphicsPolygonItem(QGraphicsItem* parent = 0);
    ~UBEditableGraphicsPolygonItem();

    virtual void addPoint(const QPointF &point) override;
    inline bool isClosed() const {return mClosed;}
    inline void setClosed(bool closed);

    inline bool isOpened() const{ return mOpened; }
    void setOpened(bool opened);

    void reopen();

    void setIsInCreationMode(bool mode);

    // UBItem interface
    UBItem *deepCopy() const override;
    void copyItemParameters(UBItem *copy) const override;

    // QGraphicsItem interface
    enum { Type = UBGraphicsItemType::GraphicsPathItemType };
    virtual int type() const  override { return Type; }
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

    virtual QRectF boundingRect() const override;
    virtual QPainterPath shape() const override;
    virtual QPainterPath painterPath() const override;

    virtual void updateHandle(UBAbstractHandle *handle) override;
    virtual bool hasFillingProperty() const override;

private:
    bool mClosed;
    bool mOpened;
    bool mIsInCreationMode;

    QPointF mStartEndPoint[2];

    int HANDLE_SIZE; //in pixel
};

#endif // UBGRAPHICSPATHITEM_H
