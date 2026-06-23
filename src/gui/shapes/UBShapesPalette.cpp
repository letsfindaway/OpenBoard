/*
 * Copyright (C) 2010-2013 Groupement d'Intérêt Public pour l'Education Numérique en Afrique (GIP ENA)
 *
 * This file is part of Open-Sankoré.
 *
 * Open-Sankoré is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 3 of the License,
 * with a specific linking exception for the OpenSSL project's
 * "OpenSSL" library (or with modified versions of it that use the
 * same license as the "OpenSSL" library).
 *
 * Open-Sankoré is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Open-Sankoré.  If not, see <http://www.gnu.org/licenses/>.
 */



#include "UBShapesPalette.h"

#include <QtGui>

#include "board/UBBoardController.h"
#include "core/UBApplication.h"
#include "domain/shapes/UBShapeFactory.h"

#include "core/memcheck.h"

UBShapesPalette::UBShapesPalette(Qt::Orientation orient, QWidget *parent )
    : UBAbstractSubPalette(parent, orient)
{

    hide();

    auto shapeActions = UBApplication::boardController->shapeFactory().shapeActions();

    QList<QAction*> actions;

    actions << shapeActions->actionSmartLine;
    actions << shapeActions->actionPolygon;
    actions << shapeActions->actionEllipse;
    actions << shapeActions->actionCircle;
    actions << shapeActions->actionRectangle;
    actions << shapeActions->actionSquare;
    actions << shapeActions->actionRegularPentagone;

    // assign shape types
    shapeActions->actionSmartLine->setProperty("ShapeType", UBShapeFactory::ShapeType::Line);
    shapeActions->actionPolygon->setProperty("ShapeType", UBShapeFactory::ShapeType::Polygon);
    shapeActions->actionEllipse->setProperty("ShapeType", UBShapeFactory::ShapeType::Ellipse);
    shapeActions->actionCircle->setProperty("ShapeType", UBShapeFactory::ShapeType::Circle);
    shapeActions->actionRectangle->setProperty("ShapeType", UBShapeFactory::ShapeType::Rectangle);
    shapeActions->actionSquare->setProperty("ShapeType", UBShapeFactory::ShapeType::Square);
    shapeActions->actionRegularPentagone->setProperty("ShapeType", UBShapeFactory::ShapeType::RegularPolygon);

    setActions(actions);

    layout()->setSpacing(0);

    adjustSizeAndPosition();

    for (const auto action : actions)
    {
        connect(action, &QAction::triggered, this, [this, action](){
            actionActivated(action);
        });
    }
}

UBShapesPalette::~UBShapesPalette()
{

}

void UBShapesPalette::actionActivated(QAction* action)
{
    auto& shapeFactory = UBApplication::boardController->shapeFactory();
    const auto shapeType = action->property("ShapeType").value<UBShapeFactory::ShapeType>();

    switch (shapeType)
    {
    case UBShapeFactory::Ellipse:
        shapeFactory.createEllipse(true);
        break;

    case UBShapeFactory::Circle:
        shapeFactory.createCircle(true);
        break;

    case UBShapeFactory::Rectangle:
        shapeFactory.createRectangle(true);
        break;

    case UBShapeFactory::Square:
        shapeFactory.createSquare(true);
        break;

    case UBShapeFactory::Line:
        shapeFactory.createLine(true);
        break;

    case UBShapeFactory::RegularPolygon:
        shapeFactory.createRegularPolygon(5);
        break;

    case UBShapeFactory::Polygon:
        shapeFactory.createPolygon(true);
        break;

    default:
        break;
    }

    if (actionPaletteButtonParent()->defaultAction() != action)
    {
        if (!actionPaletteButtonParent()->actions().isEmpty())
        {
            // Change the action shown in the stylus palette :
            for (const auto a : actionPaletteButtonParent()->actions())
            {
                 // Remove all previois actions
                actionPaletteButtonParent()->removeAction(a);
            }
        }

        // Associate the new action to the button.
        actionPaletteButtonParent()->setDefaultAction(action);
    }
}
