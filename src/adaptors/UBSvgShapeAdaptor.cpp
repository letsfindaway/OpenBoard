#include "UBSvgShapeAdaptor.h"

#include "board/UBBoardController.h"

#include "core/UBApplication.h"

#include "domain/UBGraphicsScene.h"
#include "domain/shapes/UB1HEditableGraphicsCircleItem.h"
#include "domain/shapes/UB1HEditableGraphicsSquareItem.h"
#include "domain/shapes/UBAbstractGraphicsPathItem.h"
#include "domain/shapes/UBEditableGraphicsRegularShapeItem.h"
#include "domain/shapes/UBGraphicsEllipseItem.h"
#include "domain/shapes/UBGraphicsFreehandItem.h"
#include "domain/shapes/UBGraphicsLineItem.h"
#include "domain/shapes/UBGraphicsRectItem.h"

#include "frameworks/UBStringUtils.h"

#include "gui/shapes/UBDrawingStrokePropertiesPalette.h"

const QString SVG_STROKE_DOTLINE = "20 10"; // 1 big dot, 1 little space

void UBSvgShapeAdaptor::registerExtension()
{
    UBSvgSubsetAdaptor::registerAdapterExtension(new UBSvgShapeAdaptor);
}

UBSvgShapeAdaptor::UBSvgShapeAdaptor()
{

}

UBSvgSubsetAdaptor::UBSvgReaderExtension* UBSvgShapeAdaptor::createSvgReaderExtension(QXmlStreamReader& xmlReader)
{
    return new UBSvgShapeReader{xmlReader};
}

UBSvgSubsetAdaptor::UBSvgWriterExtension* UBSvgShapeAdaptor::createSvgWriterExtension(QXmlStreamWriter& xmlWriter)
{
    return new UBSvgShapeWriter{xmlWriter};
}

void UBSvgShapeAdaptor::UBSvgShapeReader::readerExtension(std::shared_ptr<UBGraphicsScene> scene)
{
    QString name = mXmlReader.name().toString();

    if (name == "linearGradient")
    {
        linearGradientFromSvg();
    }
    else if (name == "ellipse") // EV-7 - ALTI/AOU - 20131231
    {
        QStringView isShapeRect = mXmlReader.attributes().value(UBSettings::uniboardDocumentNamespaceUri, "shapeEllipse");

        if(!isShapeRect.isNull() && isShapeRect.toString().toLower() == "true"){
            QStringView isCircle = mXmlReader.attributes().value(UBSettings::uniboardDocumentNamespaceUri, "isCircle");

            UBAbstractGraphicsItem *item = 0;

            Qt::GlobalColor color = scene->isDarkBackground() ? Qt::white : Qt::black;

            if(!isCircle.isNull() && isCircle.toString().toLower() == "true"){
                item = shapeCircleFromSvg(color);
            }else{
                item = shapeEllipseFromSvg(color);
            }

            scene->addItem(item);
        }
    }
    else if (name == "rect") // EV-7 - ALTI/CFA - 20131231
    {
        QStringView isShapeRect = mXmlReader.attributes().value(UBSettings::uniboardDocumentNamespaceUri, "shapeRect");

        if(!isShapeRect.isNull() && isShapeRect.toString().toLower() == "true"){
            QStringView isSquare = mXmlReader.attributes().value(UBSettings::uniboardDocumentNamespaceUri, "isSquare");

            UBAbstractGraphicsItem *item = 0;

            Qt::GlobalColor color = scene->isDarkBackground() ? Qt::white : Qt::black;

            if(!isSquare.isNull() && isSquare.toString().toLower() == "true"){
                item = shapeSquareFromSvg(color);
            }else{
                item = shapeRectFromSvg(color);
            }

            scene->addItem(item);
        }
    }
    else if (mXmlReader.name() == "polyline")
    {
        QStringView s = mXmlReader.attributes().value(UBSettings::uniboardDocumentNamespaceUri, "shapePath"); // EV-7 - ALTI/AOU - 20140102
        if (!s.isNull())
        {
            Qt::GlobalColor color = scene->isDarkBackground() ? Qt::white : Qt::black;
            UBAbstractGraphicsItem *pathItem = 0;

            if(s.toString().toInt() == UBEditableGraphicsRegularShapeItem::Type){
                pathItem = shapeRegularFromSvg(color);
            }else{
                pathItem = shapePathFromSvg(color, s.toString().toInt());
            }

            if (pathItem)
            {
                scene->addItem(pathItem);
            }
        }
    }
}

void UBSvgShapeAdaptor::UBSvgShapeReader::baseAttributesFromSvg(QGraphicsItem* item)
{
    bool hasZValue;
    auto ubZValue = mXmlReader.attributes().value(UBSettings::uniboardDocumentNamespaceUri, "z-value");
    qreal zValue = 0;

    if (!ubZValue.isNull())
    {
        zValue = ubZValue.toFloat(&hasZValue);

        if (zValue < -999999)
        {
            zValue = -999999;
        }
    }
    else
    {
        hasZValue = false;
    }

    if (hasZValue)
    {
        UBGraphicsItem::assignZValue(item, zValue);
    }

    UBItem* ubItem = dynamic_cast<UBItem*>(item);

    if (ubItem)
    {
        auto ubUuid = mXmlReader.attributes().value(UBSettings::uniboardDocumentNamespaceUri, "uuid");

        if (!ubUuid.isNull())
            ubItem->setUuid(QUuid(ubUuid.toString()));
        else
            ubItem->setUuid(QUuid::createUuid());
    }

}

void UBSvgShapeAdaptor::UBSvgShapeReader::linearGradientFromSvg()
{
    const auto id = mXmlReader.attributes().value("id").toString();
    QLinearGradient gradient;

    mXmlReader.readNext();
    mXmlReader.readNext();//<stop>
    QString stop0 = mXmlReader.attributes().value("style").toString();

    QStringList split = stop0.split(":");
    QStringList rgb = split.at(1).split(";");
    QStringList colors = rgb.at(0).split(",");
    int color1R = colors.at(0).split("(").at(1).toInt();
    int color1G = colors.at(1).toInt();
    int color1B = colors.at(2).split(")").at(0).toInt();

    qreal alphaF = split.at(2).toFloat();

    QColor color1(color1R, color1G, color1B);
    color1.setAlphaF(alphaF);
    gradient.setColorAt(0, color1);

    mXmlReader.readNext();
    mXmlReader.readNext();
    mXmlReader.readNext();//<stop>

    QString stop1 =  mXmlReader.attributes().value("style").toString();

    split = stop1.split(":");
    rgb = split.at(1).split(";");
    colors = rgb.at(0).split(",");
    int color2R = colors.at(0).split("(").at(1).toInt();
    int color2G = colors.at(1).toInt();
    int color2B = colors.at(2).split(")").at(0).toInt();

    alphaF = split.at(2).toFloat();

    QColor color2(color2R, color2G, color2B);
    color2.setAlphaF(alphaF);
    gradient.setColorAt(1, color2);

    mGradientMap[id] = gradient;
}

void UBSvgShapeAdaptor::UBSvgShapeReader::getStyleFromSvg(UBAbstractGraphicsItem *item, const QColor &pDefaultPenColor)
{
    QPen p = item->pen();
    // Stroke color
    QStringView svgStroke = mXmlReader.attributes().value("stroke");
    QColor strokeColor = pDefaultPenColor;
    if (!svgStroke.isNull())
    {
        strokeColor = QColor::fromString(svgStroke);
    }

    QStringView svgStrokeOpacity = mXmlReader.attributes().value("stroke-opacity");
    if (!svgStrokeOpacity.isNull())
    {
        strokeColor.setAlphaF(svgStrokeOpacity.toString().toInt());
    }

    p.setColor(strokeColor);

    // Stroke width/thickness
    QStringView svgStrokeWidth = mXmlReader.attributes().value("stroke-width");
    double strokeWidth = 2;
    if (!svgStrokeWidth.isNull())
    {
        strokeWidth = svgStrokeWidth.toString().toDouble();
    }

    p.setWidthF(strokeWidth);

    // Stroke style
    QStringView svgStrokeLineCap = mXmlReader.attributes().value("stroke-linecap");

    if(!svgStrokeLineCap.isNull() && svgStrokeLineCap.toString().toLower() == "round"){
        //Custom dash line style
        p.setCapStyle(Qt::RoundCap);
        QVector<qreal> dashPattern = UBApplication::boardController->shapeFactory().dashPattern();
        p.setDashPattern(dashPattern);
    }else{
        QStringView svgStrokeStyle = mXmlReader.attributes().value("stroke-dasharray");
        if (!svgStrokeStyle.isNull())
        {
            QString strokeStyle = svgStrokeStyle.toString();
            if (strokeStyle == SVG_STROKE_DOTLINE)
            {
                p.setStyle(Qt::DotLine);
            }
        }
    }


    item->setPen(p);

    QBrush b = item->brush();

    // Fill color
    QStringView svgFill1 = mXmlReader.attributes().value("fill");

    // Reference to gradient? url(#id)
    if (svgFill1.startsWith(QString("url(#")) && svgFill1.endsWith(')'))
    {
        const auto id = svgFill1.slice(5, svgFill1.length() - 6).toString();

        if (mGradientMap.contains(id))
        {
            auto gradient = mGradientMap.value(id);
            gradient.setStart(item->boundingRect().topLeft());
            gradient.setFinalStop(item->boundingRect().topRight());
            item->setBrush(gradient);
        }
    }
    else
    {
        QColor brushColor1 = Qt::transparent;
        if (!svgFill1.isNull())
        {
            brushColor1 = QColor::fromString(svgFill1);
        }

        // Fill opacity (transparency)
        QStringView svgOpacity1 = mXmlReader.attributes().value("fill-opacity");
        qreal opacity1 = 1.0; // opaque
        if (!svgOpacity1.isNull())
        {
            opacity1 = svgOpacity1.toString().toFloat();
        }

        if (!item->hasGradient())
        {
            brushColor1.setAlphaF(opacity1);
        }

        QStringView style = mXmlReader.attributes().value(UBSettings::uniboardDocumentNamespaceUri, "fill-style");
        b.setStyle(static_cast<Qt::BrushStyle>(style.toString().toInt()));
        b.setColor(brushColor1);

        item->setBrush(b);
    }

    // Fill pattern
    QStringView svgFillPattern = mXmlReader.attributes().value(UBSettings::uniboardDocumentNamespaceUri, "fill-pattern");
    UBAbstractGraphicsItem::FillPattern fillPattern = UBAbstractGraphicsItem::FillPattern_None;
    if (!svgFillPattern.isNull())
    {
        fillPattern = static_cast<UBAbstractGraphicsItem::FillPattern>(svgFillPattern.toString().toInt());
    }
    item->setFillPattern(fillPattern);

    // Transform matrix
    QStringView svgTransform = mXmlReader.attributes().value("transform");
    QTransform itemMatrix;
    if (!svgTransform.isNull())
    {
        itemMatrix = UBSvgSubsetAdaptor::fromSvgTransform(svgTransform.toString());
        item->setTransform(itemMatrix);
    }

    // ShapeStyle
    UBShapeStyle style;
    QStringView onLight = mXmlReader.attributes().value(UBSettings::uniboardDocumentNamespaceUri, "line-on-light-background");
    QStringView onDark = mXmlReader.attributes().value(UBSettings::uniboardDocumentNamespaceUri, "line-on-dark-background");

    if (!onLight.isNull() && !onDark.isNull())
    {
        style.setLineColor(QColor::fromString(onLight), QColor::fromString(onDark));
    }

    style.setLineWidth(strokeWidth);
    style.setLineStyle(p.style() == Qt::CustomDashLine ? Qt::DashLine : p.style());

    onLight = mXmlReader.attributes().value(UBSettings::uniboardDocumentNamespaceUri, "fill-on-light-background");
    onDark = mXmlReader.attributes().value(UBSettings::uniboardDocumentNamespaceUri, "fill-on-dark-background");

    if (!onLight.isNull() && !onDark.isNull())
    {
        style.setFillColor(QColor::fromString(onLight), QColor::fromString(onDark));
    }

    item->setShapeStyle(style);
}

UB3HEditableGraphicsEllipseItem* UBSvgShapeAdaptor::UBSvgShapeReader::shapeEllipseFromSvg(const QColor& pDefaultPenColor) // EV-7 - ALTI/AOU - 20131231
{
    UB3HEditableGraphicsEllipseItem * ellipse = new UB3HEditableGraphicsEllipseItem();
    baseAttributesFromSvg(ellipse);

    qreal cx=0, cy=0, rx=100, ry=100;
    QStringView svgCx = mXmlReader.attributes().value("cx");
    QStringView svgCy = mXmlReader.attributes().value("cy");
    QStringView svgRx = mXmlReader.attributes().value("rx");
    QStringView svgRy = mXmlReader.attributes().value("ry");

    if ( ! svgCx.isNull()) cx = svgCx.toString().toFloat();
    if ( ! svgCy.isNull()) cy = svgCy.toString().toFloat();
    if ( ! svgRx.isNull()) rx = svgRx.toString().toFloat();
    if ( ! svgRy.isNull()) ry = svgRy.toString().toFloat();

    ellipse->setRect(QRectF(0, 0, 2*rx, 2*ry));

    getStyleFromSvg(ellipse, pDefaultPenColor);

    return ellipse;
}

UB1HEditableGraphicsCircleItem* UBSvgShapeAdaptor::UBSvgShapeReader::shapeCircleFromSvg(const QColor &pDefaultPenColor)
{
    UB1HEditableGraphicsCircleItem * circle = new UB1HEditableGraphicsCircleItem();
    baseAttributesFromSvg(circle);

    qreal cx=0, cy=0, rx=100;
    QStringView svgCx = mXmlReader.attributes().value("cx");
    QStringView svgCy = mXmlReader.attributes().value("cy");
    QStringView svgRx = mXmlReader.attributes().value("rx");

    if ( ! svgCx.isNull()) cx = svgCx.toString().toFloat();
    if ( ! svgCy.isNull()) cy = svgCy.toString().toFloat();
    if ( ! svgRx.isNull()) rx = svgRx.toString().toFloat();


    circle->setRect(QRectF(0, 0, 2*rx, 2*rx));

    getStyleFromSvg(circle, pDefaultPenColor);

    return circle;
}

UB1HEditableGraphicsSquareItem* UBSvgShapeAdaptor::UBSvgShapeReader::shapeSquareFromSvg(const QColor &pDefaultPenColor)
{
    UB1HEditableGraphicsSquareItem* square = new UB1HEditableGraphicsSquareItem();
    baseAttributesFromSvg(square);

    qreal x=0, y=0, side = 0;
    QStringView svgX = mXmlReader.attributes().value("x");
    QStringView svgY = mXmlReader.attributes().value("y");
    QStringView svgS = mXmlReader.attributes().value("width");

    if ( ! svgX.isNull()) x = svgX.toString().toFloat();
    if ( ! svgY.isNull()) y = svgY.toString().toFloat();
    if ( ! svgS.isNull()) side = svgS.toString().toFloat();

    square->setRect(QRectF(0, 0, side, side));

    getStyleFromSvg(square, pDefaultPenColor);

    return square;
}

UB3HEditableGraphicsRectItem* UBSvgShapeAdaptor::UBSvgShapeReader::shapeRectFromSvg(const QColor& pDefaultPenColor) // EV-7 - ALTI/AOU - 20131231
{
    UB3HEditableGraphicsRectItem* rect = new UB3HEditableGraphicsRectItem();
    baseAttributesFromSvg(rect);

    qreal x=0, y=0, w=100, h=100;
    QStringView svgX = mXmlReader.attributes().value("x");
    QStringView svgY = mXmlReader.attributes().value("y");
    QStringView svgW = mXmlReader.attributes().value("width");
    QStringView svgH = mXmlReader.attributes().value("height");

    if ( ! svgX.isNull()) x = svgX.toString().toFloat();
    if ( ! svgY.isNull()) y = svgY.toString().toFloat();
    if ( ! svgW.isNull()) w = svgW.toString().toFloat();
    if ( ! svgH.isNull()) h = svgH.toString().toFloat();

    rect->setRect(QRectF(0, 0, w, h));

    getStyleFromSvg(rect, pDefaultPenColor);

    return rect;
}

UBAbstractGraphicsPathItem* UBSvgShapeAdaptor::UBSvgShapeReader::shapePathFromSvg(const QColor& pDefaultPenColor, int type) // EV-7 - ALTI/AOU - 20140102
{
    UBAbstractGraphicsPathItem *pathItem = 0;

    switch(type){
    case UBGraphicsFreehandItem::Type:
        pathItem = new UBGraphicsFreehandItem();
        break;
    case UBEditableGraphicsPolygonItem::Type:
        pathItem = new UBEditableGraphicsPolygonItem();
        break;
    case UBEditableGraphicsLineItem::Type:
        pathItem = new UBEditableGraphicsLineItem();
        break;
    default:
        break;
    }

    if (pathItem)
    {
        baseAttributesFromSvg(pathItem);
    }

    QStringView svgPoints = mXmlReader.attributes().value("points");

    if (pathItem && !svgPoints.isNull())
    {
        QStringList ts = svgPoints.toString().split(QLatin1Char(' '),
                         Qt::SkipEmptyParts);

        foreach(const QString sPoint, ts)
        {
            QStringList sCoord = sPoint.split(QLatin1Char(','), Qt::SkipEmptyParts);

            if (sCoord.size() == 2)
            {
                QPointF point;
                point.setX(sCoord.at(0).toFloat());
                point.setY(sCoord.at(1).toFloat());
                pathItem->addPoint(point);
            }
            else if (sCoord.size() == 4){
                //This is the case on system were the "," is used to seperate decimal
                QPointF point;
                QString x = sCoord.at(0) + "." + sCoord.at(1);
                QString y = sCoord.at(2) + "." + sCoord.at(3);
                point.setX(x.toFloat());
                point.setY(y.toFloat());
                pathItem->addPoint(point);
            }
            else
            {
                qWarning() << "cannot make sense of a 'point' value" << sCoord;
            }
        }
    }
    else
    {
        qWarning() << "cannot make sense of 'points' value " << svgPoints.toString();
    }

    // Arrows on extremities :
    QStringView startArrowType = mXmlReader.attributes().value(UBSettings::uniboardDocumentNamespaceUri, "startArrowType");
    if (!startArrowType.isNull()) {
        pathItem->setStartArrowType((UBAbstractGraphicsPathItem::ArrowType)startArrowType.toString().toInt());
    }
    QStringView endArrowType = mXmlReader.attributes().value(UBSettings::uniboardDocumentNamespaceUri, "endArrowType");
    if (!endArrowType.isNull()) {
        pathItem->setEndArrowType((UBAbstractGraphicsPathItem::ArrowType)endArrowType.toString().toInt());
    }

    getStyleFromSvg(pathItem, pDefaultPenColor);

    return pathItem;
}

UBEditableGraphicsRegularShapeItem* UBSvgShapeAdaptor::UBSvgShapeReader::shapeRegularFromSvg(const QColor& pDefaultPenColor)
{
    QStringView svgPoints = mXmlReader.attributes().value("points");

    QStringView nVertices = mXmlReader.attributes().value(UBSettings::uniboardDocumentNamespaceUri, "nVertices");
    QStringView startPointX = mXmlReader.attributes().value(UBSettings::uniboardDocumentNamespaceUri, "startPointX");
    QStringView startPointY = mXmlReader.attributes().value(UBSettings::uniboardDocumentNamespaceUri, "startPointY");

    QStringView cCenterX = mXmlReader.attributes().value(UBSettings::uniboardDocumentNamespaceUri, "cCenterX");
    QStringView cCenterY = mXmlReader.attributes().value(UBSettings::uniboardDocumentNamespaceUri, "cCenterY");
    QStringView cRadius = mXmlReader.attributes().value(UBSettings::uniboardDocumentNamespaceUri, "cRadius");

    UBEditableGraphicsRegularShapeItem *pathItem = new UBEditableGraphicsRegularShapeItem(nVertices.toString().toInt(), QPointF(startPointX.toString().toFloat(), startPointY.toString().toFloat()));
    baseAttributesFromSvg(pathItem);

    pathItem->setCircumscribedCenterCircle(QPointF(cCenterX.toString().toFloat(), cCenterY.toString().toFloat()));
    pathItem->setCircumscribedRadiusCircle(cRadius.toString().toFloat());

    if (!svgPoints.isNull())
    {
        QStringList ts = svgPoints.toString().split(QLatin1Char(' '),
                         Qt::SkipEmptyParts);

        foreach(const QString sPoint, ts)
        {
            QStringList sCoord = sPoint.split(QLatin1Char(','), Qt::SkipEmptyParts);

            if (sCoord.size() == 2)
            {
                QPointF point;
                point.setX(sCoord.at(0).toFloat());
                point.setY(sCoord.at(1).toFloat());
                pathItem->addPoint(point);
            }
            else if (sCoord.size() == 4){
                //This is the case on system were the "," is used to seperate decimal
                QPointF point;
                QString x = sCoord.at(0) + "." + sCoord.at(1);
                QString y = sCoord.at(2) + "." + sCoord.at(3);
                point.setX(x.toFloat());
                point.setY(y.toFloat());
                pathItem->addPoint(point);
            }
            else
            {
                qWarning() << "cannot make sense of a 'point' value" << sCoord;
            }
        }
    }
    else
    {
        qWarning() << "cannot make sense of 'points' value " << svgPoints.toString();
    }

    getStyleFromSvg(pathItem, pDefaultPenColor);

    return pathItem;
}


UBSvgShapeAdaptor::UBSvgShapeWriter::UBSvgShapeWriter(QXmlStreamWriter& xmlWriter)
    : mXmlWriter{xmlWriter}
{
}

void UBSvgShapeAdaptor::UBSvgShapeWriter::writerExtension(QGraphicsItem* item)
{
    // Is the item a shape Ellipse ?
    UB3HEditableGraphicsEllipseItem* shapeEllipseItem = dynamic_cast<UB3HEditableGraphicsEllipseItem*>(item);// EV-7 - ALTI/AOU - 20131231
    if (shapeEllipseItem && shapeEllipseItem->isVisible())
    {
        shapeEllipseToSvg(shapeEllipseItem);
    }

    // Is the item a shape Rect ?
    UB3HEditableGraphicsRectItem* shapeRectItem = dynamic_cast<UB3HEditableGraphicsRectItem*>(item);// EV-7 - ALTI/AOU - 20131231
    if (shapeRectItem && shapeRectItem->isVisible())
    {
        shapeRectToSvg(shapeRectItem);
    }

    // Is the item a shape Square ?
    UB1HEditableGraphicsSquareItem* shapeSquareItem = dynamic_cast<UB1HEditableGraphicsSquareItem*>(item);// EV-7 - ALTI/AOU - 20131231
    if (shapeSquareItem && shapeSquareItem->isVisible())
    {
        shapeSquareToSvg(shapeSquareItem);
    }

    // Is the item a shape Circle ?
    UB1HEditableGraphicsCircleItem* shapeCircleItem = dynamic_cast<UB1HEditableGraphicsCircleItem*>(item);// EV-7 - ALTI/AOU - 20131231
    if (shapeCircleItem && shapeCircleItem->isVisible())
    {
        shapeCircleToSvg(shapeCircleItem);
    }


    // Is the item a shape Path ? (closed polygon, opened polygon, freehand drawing)
    UBAbstractGraphicsPathItem * shapepathItem = dynamic_cast<UBAbstractGraphicsPathItem *>(item); // EV-7 - ALTI/AOU - 20140102
    if (shapepathItem && shapepathItem->isVisible())
    {
        shapePathToSvg(shapepathItem);
    }

    // Is the item a shape Path ? (closed polygon, opened polygon, freehand drawing)
    UBEditableGraphicsRegularShapeItem * shapeRegularItem = dynamic_cast<UBEditableGraphicsRegularShapeItem *>(item); // EV-7 - ALTI/AOU - 20140102
    if (shapeRegularItem && shapeRegularItem->isVisible())
    {
        shapeRegularToSvg(shapeRegularItem);
    }
}

void UBSvgShapeAdaptor::UBSvgShapeWriter::shapeEllipseToSvg(UB3HEditableGraphicsEllipseItem *item) // EV-7 - ALTI/AOU - 20131231
{
    writeAbstractGraphicsItemGradient(item);

    mXmlWriter.writeStartElement("ellipse");
    mXmlWriter.writeAttribute(UBSettings::uniboardDocumentNamespaceUri, "shapeEllipse", "true");

    // SVG <ellipse> tag :
    mXmlWriter.writeAttribute("cx", QString("%1").arg(item->center().x())); // The <ellipse> SVG tag need center coordinates. Compute them from boundaries of item.
    mXmlWriter.writeAttribute("cy", QString("%1").arg(item->center().y()));
    mXmlWriter.writeAttribute("rx", QString("%1").arg(item->radiusX()));
    mXmlWriter.writeAttribute("ry", QString("%1").arg(item->radiusY()));

    writeAbstractGraphicsItemStyle(item);

    mXmlWriter.writeEndElement();
}

void UBSvgShapeAdaptor::UBSvgShapeWriter::shapeRectToSvg(UB3HEditableGraphicsRectItem *item) // EV-7 - ALTI/AOU - 20131231
{
    writeAbstractGraphicsItemGradient(item);

    mXmlWriter.writeStartElement("rect");
    mXmlWriter.writeAttribute(UBSettings::uniboardDocumentNamespaceUri, "shapeRect", "true");

    // SVG <shapeRect> tag :
    mXmlWriter.writeAttribute("x", QString::number(item->rect().x()));
    mXmlWriter.writeAttribute("y", QString::number(item->rect().y()));
    mXmlWriter.writeAttribute("width", QString::number(item->rect().width()));
    mXmlWriter.writeAttribute("height", QString::number(item->rect().height()));

    writeAbstractGraphicsItemStyle(item);

    mXmlWriter.writeEndElement();
}

void UBSvgShapeAdaptor::UBSvgShapeWriter::shapeRegularToSvg(UBEditableGraphicsRegularShapeItem *item)
{
    writeAbstractGraphicsItemGradient(item);

    mXmlWriter.writeStartElement("polyline");

    mXmlWriter.writeAttribute(UBSettings::uniboardDocumentNamespaceUri, "shapePath", QString::number(item->type())); // just to know it's a path drawn with the drawingPalette

    mXmlWriter.writeAttribute(UBSettings::uniboardDocumentNamespaceUri, "nVertices", QString::number(item->nVertices()));
    mXmlWriter.writeAttribute(UBSettings::uniboardDocumentNamespaceUri, "startPointX", QString::number(item->startPoint().x()));
    mXmlWriter.writeAttribute(UBSettings::uniboardDocumentNamespaceUri, "startPointY", QString::number(item->startPoint().y()));
    mXmlWriter.writeAttribute(UBSettings::uniboardDocumentNamespaceUri, "cCenterX", QString::number(item->circumscribedCenterCircle().x()));
    mXmlWriter.writeAttribute(UBSettings::uniboardDocumentNamespaceUri, "cCenterY", QString::number(item->circumscribedCenterCircle().y()));
    mXmlWriter.writeAttribute(UBSettings::uniboardDocumentNamespaceUri, "cRadius", QString::number(item->circumscribedRadiusCircle()));

    QString sPoints;
    for(int i=0; i<item->path().elementCount(); ++i)
    {
        QPainterPath::Element element = item->path().elementAt(i);
        sPoints += QString("%1,%2 ").arg(element.x).arg(element.y);
    }

    mXmlWriter.writeAttribute("points", sPoints);

    writeAbstractGraphicsItemStyle(item);

    mXmlWriter.writeEndElement();
}

void UBSvgShapeAdaptor::UBSvgShapeWriter::shapePathToSvg(UBAbstractGraphicsPathItem *item) // EV-7 - ALTI/AOU - 20140102
{
    writeAbstractGraphicsItemGradient(item);

    mXmlWriter.writeStartElement("polyline");

    mXmlWriter.writeAttribute(UBSettings::uniboardDocumentNamespaceUri, "shapePath", QString::number(item->type())); // just to know it's a path drawn with the drawingPalette

    if(dynamic_cast<UBEditableGraphicsLineItem*>(item))
        mXmlWriter.writeAttribute(UBSettings::uniboardDocumentNamespaceUri, "isLine", "true");

    QString sPoints;
    for(int i=0; i<item->path().elementCount(); ++i)
    {
        QPainterPath::Element element = item->path().elementAt(i);
        sPoints += QString("%1,%2 ").arg(element.x).arg(element.y);
    }

    mXmlWriter.writeAttribute("points", sPoints);

    // Arrows :
    UBAbstractGraphicsPathItem::ArrowType startArrowType = item->startArrowType();
    if (startArrowType != UBAbstractGraphicsPathItem::ArrowType_None){
        mXmlWriter.writeAttribute(UBSettings::uniboardDocumentNamespaceUri, "startArrowType", QString::number(startArrowType));
    }

    UBAbstractGraphicsPathItem::ArrowType endArrowType = item->endArrowType();
    if (endArrowType != UBAbstractGraphicsPathItem::ArrowType_None)    {
        mXmlWriter.writeAttribute(UBSettings::uniboardDocumentNamespaceUri, "endArrowType", QString::number(endArrowType));
    }

    writeAbstractGraphicsItemStyle(item);

    mXmlWriter.writeEndElement();

}

void UBSvgShapeAdaptor::UBSvgShapeWriter::shapeSquareToSvg(UB1HEditableGraphicsSquareItem *item)
{
    writeAbstractGraphicsItemGradient(item);

    mXmlWriter.writeStartElement("rect");
    mXmlWriter.writeAttribute(UBSettings::uniboardDocumentNamespaceUri, "shapeRect", "true");

    // SVG <shapeRect> tag :
    mXmlWriter.writeAttribute(UBSettings::uniboardDocumentNamespaceUri, "isSquare", "true");
    mXmlWriter.writeAttribute("x", QString::number(item->rect().x()));
    mXmlWriter.writeAttribute("y", QString::number(item->rect().y()));
    mXmlWriter.writeAttribute("width", QString::number(item->rect().width()));
    mXmlWriter.writeAttribute("height", QString::number(item->rect().height()));

    writeAbstractGraphicsItemStyle(item);

    mXmlWriter.writeEndElement();
}

void UBSvgShapeAdaptor::UBSvgShapeWriter::shapeCircleToSvg(UB1HEditableGraphicsCircleItem *item)
{
    writeAbstractGraphicsItemGradient(item);

    mXmlWriter.writeStartElement("ellipse");
    mXmlWriter.writeAttribute(UBSettings::uniboardDocumentNamespaceUri, "shapeEllipse", "true");

    // SVG <ellipse> tag :
    mXmlWriter.writeAttribute(UBSettings::uniboardDocumentNamespaceUri, "isCircle", "true");
    mXmlWriter.writeAttribute("cx", QString("%1").arg(item->center().x()));
    mXmlWriter.writeAttribute("cy", QString("%1").arg(item->center().y()));
    mXmlWriter.writeAttribute("rx", QString("%1").arg(item->radius()));
    mXmlWriter.writeAttribute("ry", QString("%1").arg(item->radius()));

    writeAbstractGraphicsItemStyle(item);

    mXmlWriter.writeEndElement();
}

void UBSvgShapeAdaptor::UBSvgShapeWriter::writeAbstractGraphicsItemGradient(UBAbstractGraphicsItem *item)
{
    if(item->hasGradient()){
        QColor color1 = item->brush().gradient()->stops().at(0).second;
        QColor color2 = item->brush().gradient()->stops().at(1).second;
        mXmlWriter.writeStartElement("linearGradient");
        mXmlWriter.writeAttribute("id", "id:" + item->uuid().toString(QUuid::WithoutBraces));
        mXmlWriter.writeAttribute("x1", "0%");
        mXmlWriter.writeAttribute("y1", "0%");
        mXmlWriter.writeAttribute("x2", "100%");
        mXmlWriter.writeAttribute("y2", "0%");
        mXmlWriter.writeStartElement("stop");
        mXmlWriter.writeAttribute("offset", "0%");
        mXmlWriter.writeAttribute("style", QString("stop-color:rgb(%1,%2,%3);stop-opacity:%4").arg(QString::number(color1.red()), QString::number(color1.green()), QString::number(color1.blue()), QString::number(color1.alphaF())));
        mXmlWriter.writeEndElement();
        mXmlWriter.writeStartElement("stop");
        mXmlWriter.writeAttribute("offset", "100%");
        mXmlWriter.writeAttribute("style", QString("stop-color:rgb(%1,%2,%3);stop-opacity:%4").arg(QString::number(color2.red()), QString::number(color2.green()), QString::number(color2.blue()), QString::number(color2.alphaF())));
        mXmlWriter.writeEndElement();
        mXmlWriter.writeEndElement();
    }
}

void UBSvgShapeAdaptor::UBSvgShapeWriter::writeAbstractGraphicsItemStyle(UBAbstractGraphicsItem *item)
{
    // Stroke :
    if(item->hasStrokeProperty()){
        mXmlWriter.writeAttribute("stroke", QString("%1").arg(item->pen().color().name()));
        mXmlWriter.writeAttribute("stroke-width", QString("%1").arg(item->pen().widthF()));

        mXmlWriter.writeAttribute(UBSettings::uniboardDocumentNamespaceUri
                                  , "line-on-light-background", item->shapeStyle().lineColor(false).name(QColor::HexArgb));
        mXmlWriter.writeAttribute(UBSettings::uniboardDocumentNamespaceUri
                                  , "line-on-dark-background", item->shapeStyle().lineColor(true).name(QColor::HexArgb));

        if (item->pen().style() == Qt::DotLine){
            mXmlWriter.writeAttribute("stroke-dasharray", SVG_STROKE_DOTLINE);
        }

        if(item->pen().style() == Qt::CustomDashLine || item->pen().style() == Qt::DashLine){
            switch(item->pen().width()){
                case UBDrawingStrokePropertiesPalette::Fine:
                    mXmlWriter.writeAttribute("stroke-dasharray", "1, 10");
                    break;
                case UBDrawingStrokePropertiesPalette::Medium:
                    mXmlWriter.writeAttribute("stroke-dasharray", "1, 15");
                    break;
                case UBDrawingStrokePropertiesPalette::Large:
                    mXmlWriter.writeAttribute("stroke-dasharray", "1, 20");
                    break;
                default:
                    mXmlWriter.writeAttribute("stroke-dasharray", "1, 3");
            }

            mXmlWriter.writeAttribute("stroke-linecap", "round");
        }

        mXmlWriter.writeAttribute("stroke-opacity", QString("%1").arg(item->pen().color().alphaF()));
    }

    // Fill :
    if (item->hasFillingProperty())
    {
        if (!item->hasGradient())
        {
            mXmlWriter.writeAttribute("fill", QString("%1").arg(item->brush().color().name()));
            mXmlWriter.writeAttribute("fill-opacity", QString("%1").arg(item->brush().color().alphaF()));
            mXmlWriter.writeAttribute(UBSettings::uniboardDocumentNamespaceUri, "fill-style", QString("%1").arg(item->brush().style()));

            if (item->brush().style() == Qt::TexturePattern){
                mXmlWriter.writeAttribute(UBSettings::uniboardDocumentNamespaceUri, "fill-pattern", QString("%1").arg(item->fillPattern()));
            }

            mXmlWriter.writeAttribute(UBSettings::uniboardDocumentNamespaceUri
                                      , "fill-on-light-background", item->shapeStyle().fillColor(false).name(QColor::HexArgb));
            mXmlWriter.writeAttribute(UBSettings::uniboardDocumentNamespaceUri
                                      , "fill-on-dark-background", item->shapeStyle().fillColor(true).name(QColor::HexArgb));
        }
        else
        {
            mXmlWriter.writeAttribute("fill", QString("url(#%1)").arg("id:" + item->uuid().toString(QUuid::WithoutBraces)));
        }
    }
    else
        mXmlWriter.writeAttribute("fill", "none");

    //z-value
    mXmlWriter.writeAttribute(UBSettings::uniboardDocumentNamespaceUri, "z-value", QString("%1").arg(item->zValue()));

    //uuid
    mXmlWriter.writeAttribute(UBSettings::uniboardDocumentNamespaceUri, "uuid", UBStringUtils::toCanonicalUuid(item->uuid()));

    //transform matrix
    mXmlWriter.writeAttribute("transform", UBSvgSubsetAdaptor::toSvgTransform(item->sceneTransform()));
}

UBSvgShapeAdaptor::UBSvgShapeReader::UBSvgShapeReader(QXmlStreamReader& xmlReader)
    : mXmlReader{xmlReader}
{
}
