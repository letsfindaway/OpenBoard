#pragma once

#include "domain/UBItem.h"
#include "domain/shapes/UBShapeStyle.h"


#include <QAbstractGraphicsShapeItem>

class UBAbstractGraphicsItem : public UBItem, public UBGraphicsItem, public QAbstractGraphicsShapeItem
{
public:
    UBAbstractGraphicsItem(QGraphicsItem *parent = 0);

    virtual ~UBAbstractGraphicsItem();

    void applyStyle(const UBShapeStyle& style, bool isDark);
    UBShapeStyle shapeStyle() const;
    void setShapeStyle(const UBShapeStyle& style);

    bool hasFillingProperty() const;

    bool hasStrokeProperty() const;

    bool hasGradient() const;

    void setStyle(Qt::PenStyle penStyle);

    void setStyle(Qt::BrushStyle brushStyle);

    void setStyle(Qt::BrushStyle brushStyle, Qt::PenStyle penStyle);

    void setFillColor(const QColor& color);

    void setStrokeColor(const QColor& color);

    void setStrokeSize(int size);

    enum FillPattern{   // Warning : those values are persisted. Do NOT change this order. Only add new values at the end of enum.
        FillPattern_None,
        FillPattern_Diag1,
        FillPattern_Dot1
    };

    FillPattern fillPattern() const {return mFillPatern;}
    void setFillPattern(FillPattern pattern);

    // get the path of the shape in local coordinates
    // Note: shape() in contrast, returns the path of the outline of the shape, including pen width
    virtual QPainterPath painterPath() const = 0;

    // UBItem interface
//    void setUuid(const QUuid &pUuid);

    void initializeFillingProperty();

    void initializeStrokeProperty();

    //disambiguation from UBGraphicsItem and QabstractGraphicsShapeItem
    virtual int type() const  override = 0;

    //must be define, because the delegate use it
    virtual QRectF boundingRect() const  override { return QRect(); }

    virtual void copyItemParameters(UBItem *copy) const  override;

    virtual std::shared_ptr<UBGraphicsScene> scene() override;

protected:
    void setStyle(QPainter *painter);

    QRectF adjustBoundingRect(QRectF rect) const;

    QVariant itemChange(GraphicsItemChange change, const QVariant &value) override;

private:
    UBShapeStyle mShapeStyle{};
    FillPattern mFillPatern;
    QBitmap patternPoint();
    QBitmap patternDiag();
};

