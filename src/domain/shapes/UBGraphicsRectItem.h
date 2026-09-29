/*
 * Copyright (C) 2015-2026 Département de l'Instruction Publique (DIP-SEM)
 * and contributors.
 *
 * This file is part of OpenBoard.
 *
 * OpenBoard is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 3 of the License,
 * with a specific linking exception for the OpenSSL project's
 * "OpenSSL" library (or with modified versions of it that use the
 * same license as the "OpenSSL" library).
 *
 * OpenBoard is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with OpenBoard. If not, see <http://www.gnu.org/licenses/>.
 */


#ifndef UBGRAPHICSRECTITEM_H
#define UBGRAPHICSRECTITEM_H

#include <QGraphicsRectItem>

#include "UB3HandlesEditable.h"

class UB3HEditableGraphicsRectItem : public UB3HEditablesGraphicsBasicShapeItem
{
public:
    UB3HEditableGraphicsRectItem(QGraphicsItem* parent = 0);

    virtual ~UB3HEditableGraphicsRectItem();

    enum { Type = UBGraphicsItemType::GraphicsShapeItemType };
    virtual int type() const { return Type; }

    virtual UBItem* deepCopy() const;

    virtual void copyItemParameters(UBItem *copy) const;

    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

    void updateHandle(UBAbstractHandle *handle);

    virtual QRectF boundingRect() const;

    virtual QPainterPath painterPath() const;

    virtual void onActivateEditionMode();

    void setRect(QRectF rect);

    QRectF rect() const;

private:
    qreal mWidth;
    qreal mHeight;
};

#endif // UBGRAPHICSRECTITEM_H
