#include "UBShapeEditable.h"

#include "domain/UBGraphicsDelegateFrame.h"
#include "domain/UBGraphicsScene.h"

UBAbstractEditableGraphicsShapeItem::UBAbstractEditableGraphicsShapeItem(QGraphicsItem *parent):
    UBAbstractGraphicsItem(parent)
{
    mMultiClickState = 0;
    mHasMoved = false;
}

QPainterPath UBAbstractEditableGraphicsShapeItem::shape() const
{
    QPainterPath outline;

    if (isInEditMode())
    {
        outline.addRect(boundingRect());
    }
    else
    {
        QPainterPathStroker stroker{pen()};
        stroker.setDashPattern(Qt::SolidLine);

        if (pen().width() < 3)
        {
            stroker.setWidth(3);
        }

        const auto path = painterPath();
        outline = stroker.createStroke(path);

        if (hasFillingProperty())
        {
            outline = outline.united(path);
        }
    }

    return outline;
}

void UBAbstractEditableGraphicsShapeItem::onActivateEditionMode()
{
    //NOOP
}

void UBAbstractEditableGraphicsShapeItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    mMultiClickState++;
    mHasMoved = false;
}

void UBAbstractEditableGraphicsShapeItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    prepareGeometryChange();

    if(!mHasMoved)
    {
        if (!Delegate()->isLocked())
        {
            if(mMultiClickState %2 == 0){
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
    else
    {
        if(!isInEditMode()){
            mMultiClickState = 0;
        }else{
            mMultiClickState--;
        }
    }

    mHasMoved = false;
}

void UBAbstractEditableGraphicsShapeItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if (!Delegate()->isLocked())
    {
        mHasMoved = true;

        if(!isInEditMode()){
            Delegate()->mouseMoveEvent(event);
        }
    }
}

void UBAbstractEditableGraphicsShapeItem::focusOutEvent(QFocusEvent *event)
{
    Q_UNUSED(event)

    if(mMultiClickState %2 == 1){
        prepareGeometryChange();
        mMultiClickState = 0;
        showEditMode(false);
    }
}

void UBAbstractEditableGraphicsShapeItem::paintCenterMark(QPainter* painter)
{
    if (mHasMoved || (Delegate() && Delegate()->frame() && Delegate()->frame()->moving()))
    {
        painter->save();

        // determine contrast color to item
        auto fillColor = painter->brush().color();

        if (fillColor == Qt::transparent)
        {
            // get background color
            fillColor = scene()->isDarkBackground() ? Qt::black : Qt::white;
        }

        fillColor = fillColor.toHsv();

        // compute a color which has the opposite color and brightness
        auto hue = fillColor.hueF() + 0.5;
        hue -= (long)hue;

        if (hue < 0.)
        {
            hue = 0;
        }

        const auto saturation = fillColor.saturationF();

        // prefer brighter colors
        auto value = fillColor.valueF() * fillColor.valueF() + 0.5;
        value -= (long)value;

        const auto markColor = QColor::fromHsvF(hue, saturation, value);

        painter->setBrush(QBrush());
        QPen p;
        p.setColor(markColor);
        p.setWidth(3);
        painter->setPen(p);

        const auto center = boundingRect().center();
        painter->drawLine(center - QPointF{5, 0}, center + QPointF{5, 0});
        painter->drawLine(center - QPointF{0, 5}, center + QPointF{0, 5});

        painter->restore();
    }
}

void UBAbstractEditableGraphicsShapeItem::deactivateEditionMode()
{
    prepareGeometryChange();

    mMultiClickState = 0;
    showEditMode(false);
}
