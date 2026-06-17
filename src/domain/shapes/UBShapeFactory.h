#pragma once

#include <QObject>
#include <QColor>
#include <QGraphicsItem>

#include "UBAbstractGraphicsPathItem.h"
#include "UBShapeStyle.h"

#include "ui_shapeActions.h"

class UBAbstractGraphicsItem;
class UBBoardView;
class QMouseEvent;
class UBDrawingController;


class UBShapeFactory : public QObject
{
    Q_OBJECT

public:
    UBShapeFactory();
    void init();

    Ui::ShapeActions* shapeActions() const;

    static bool isShape(QGraphicsItem *item);
    static void desactivateEditionMode(QGraphicsItem *item);
    static bool isInEditMode(QGraphicsItem *item);

    enum ShapeType
    {
        Ellipse,
        Circle,
        Rectangle,
        Square,
        Line,
        Pen,
        RegularPolygon,
        Polygon,
        None
    };

    enum FillType
    {
        Transparent,
        Full,
        Dense,
        Diag,
        Gradient
    };

    QColor strokeColor();
    QColor fillFirstColor();
    QColor fillSecondColor();

    void returnToCreationMode(QGraphicsItem* item);
    void applyCurrentStyle(UBAbstractGraphicsItem *shape);
    void setFillingFirstColor(QColor color);
    void setFillingSecondColor(QColor color);
    void setGradientFillingProperty(UBAbstractGraphicsItem* shape);
    void updateFillingPropertyOnSelectedItems();

    FillType fillType();
    void setFillType(FillType fillType);

    void setStartArrowType(UBAbstractGraphicsPathItem::ArrowType arrowType);
    void setEndArrowType(UBAbstractGraphicsPathItem::ArrowType arrowType);

    QRectF reverseRect(const QRectF &rect);

    QVector<qreal> dashPattern() const;

public slots:
    void createRegularPolygon(int nVertices);
    void createEllipse(bool create);
    void createPolygon(bool create);
    void createCircle(bool create);
    void createRectangle(bool create);
    void createSquare(bool create);
    void createLine(bool create);

    void createPen(bool create);

    void prepareChangeFill();
    void changeFillColor(const QPointF& pos);

    void onMouseMove(QMouseEvent *event);
    void onMousePress(QMouseEvent *event);
    void onMouseRelease(QMouseEvent *event);

    void desactivate();
    void terminateShape();

    void setStrokeStyle(Qt::PenStyle penStyle);
    void setStrokeColor(QColor color);

    void setThickness(int thickness);

    void selectionChanged(UBAbstractGraphicsItem* item, bool selected);
    void applyStyle(const UBShapeStyle& style);
    void setCurrentStyle(const UBShapeStyle& style);

private:
    UBAbstractGraphicsItem* mCurrentShape{nullptr};
    UBBoardView* mBoardView{nullptr};
    UBShapeStyle mShapeStyle{};

    bool mIsCreating{false};
    bool mIsPress{false};
    bool mIsRegularShape{true};

    bool mFirstClickForFreeHand{true};

    ShapeType mShapeType{None};

    QColor mCurrentStrokeColor{Qt::black};
    QColor mCurrentFillFirstColor{Qt::transparent};
    QColor mCurrentFillSecondColor{Qt::transparent};

    Qt::BrushStyle mCurrentBrushStyle{Qt::SolidPattern};
    Qt::PenStyle mCurrentPenStyle{Qt::SolidLine};

    int mThickness{3};
    UBDrawingController *mDrawingController{nullptr};

    int mNVertices{0};

    FillType mFillType{Transparent};
    QRectF mBoundingRect{};

    UBAbstractGraphicsPathItem::ArrowType mStartArrowType{UBAbstractGraphicsPathItem::ArrowType_None};
    UBAbstractGraphicsPathItem::ArrowType mEndArrowType{UBAbstractGraphicsPathItem::ArrowType_None};

    bool mCursorMoved{false};

    QVector<qreal> mDotDashes{};

    Ui::ShapeActions* mShapeActions{nullptr};

    QSet<UBAbstractGraphicsItem*> mSelectedShapes{};

protected:
    UBAbstractGraphicsItem *instanciateCurrentShape();

};
