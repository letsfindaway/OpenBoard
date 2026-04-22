#pragma once

#include "UBSvgSubsetAdaptor.h"

// forward
class UB1HEditableGraphicsCircleItem;
class UB1HEditableGraphicsSquareItem;
class UB3HEditableGraphicsEllipseItem;
class UB3HEditableGraphicsRectItem;
class UBAbstractGraphicsItem;
class UBAbstractGraphicsPathItem;
class UBEditableGraphicsRegularShapeItem;

class UBSvgShapeAdaptor : public UBSvgSubsetAdaptor::UBSvgAdaptorExtension
{
public:
    static void registerExtension();

    UBSvgShapeAdaptor();

    virtual UBSvgSubsetAdaptor::UBSvgReaderExtension* createSvgReaderExtension(QXmlStreamReader& xmlReader) override;
    virtual UBSvgSubsetAdaptor::UBSvgWriterExtension* createSvgWriterExtension(QXmlStreamWriter& xmlWriter) override;

    class UBSvgShapeReader : public UBSvgSubsetAdaptor::UBSvgReaderExtension
    {
    public:
        UBSvgShapeReader(QXmlStreamReader& xmlReader);

        virtual void readerExtension(std::shared_ptr<UBGraphicsScene> scene) override;

    private:
        void baseAttributesFromSvg(QGraphicsItem* item);
        void linearGradientFromSvg();
        void getStyleFromSvg(UBAbstractGraphicsItem *item, const QColor &pDefaultPenColor);
        UB3HEditableGraphicsEllipseItem* shapeEllipseFromSvg(const QColor& pDefaultPenColor);
        UB1HEditableGraphicsCircleItem* shapeCircleFromSvg(const QColor &pDefaultPenColor);
        UB1HEditableGraphicsSquareItem* shapeSquareFromSvg(const QColor &pDefaultPenColor);
        UB3HEditableGraphicsRectItem* shapeRectFromSvg(const QColor& pDefaultPenColor);
        UBAbstractGraphicsPathItem* shapePathFromSvg(const QColor& pDefaultPenColor, int type);
        UBEditableGraphicsRegularShapeItem* shapeRegularFromSvg(const QColor& pDefaultPenColor);

    private:
        QXmlStreamReader& mXmlReader;
        QMap<QString,QLinearGradient> mGradientMap;
    };

    class UBSvgShapeWriter : public UBSvgSubsetAdaptor::UBSvgWriterExtension
    {
    public:
        UBSvgShapeWriter(QXmlStreamWriter& xmlWriter);

        virtual void writerExtension(QGraphicsItem* item) override;

    private:
        void shapeEllipseToSvg(UB3HEditableGraphicsEllipseItem *item);
        void shapeRectToSvg(UB3HEditableGraphicsRectItem *item);
        void shapeRegularToSvg(UBEditableGraphicsRegularShapeItem *item);
        void shapePathToSvg(UBAbstractGraphicsPathItem *item);
        void shapeSquareToSvg(UB1HEditableGraphicsSquareItem *item);
        void shapeCircleToSvg(UB1HEditableGraphicsCircleItem *item);
        void writeAbstractGraphicsItemGradient(UBAbstractGraphicsItem *item);
        void writeAbstractGraphicsItemStyle(UBAbstractGraphicsItem *item);

    private:
        QXmlStreamWriter& mXmlWriter;
    };
};

