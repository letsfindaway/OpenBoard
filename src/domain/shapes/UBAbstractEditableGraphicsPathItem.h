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


#ifndef UBABSTRACTEDITABLEGRAPHICSPATHITEM_H
#define UBABSTRACTEDITABLEGRAPHICSPATHITEM_H

#include "UBEditable.h"
#include "UBAbstractGraphicsPathItem.h"

class UBAbstractEditableGraphicsPathItem : public UBAbstractEditable, public UBAbstractGraphicsPathItem
{
public:
    UBAbstractEditableGraphicsPathItem(QGraphicsItem *parent = 0);

    virtual QRectF boundingRect() const;

    virtual QPainterPath shape() const;

protected:
    virtual void mousePressEvent(QGraphicsSceneMouseEvent *event);
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
    virtual void focusOutEvent(QFocusEvent *event);
    virtual void focusHandle(UBAbstractHandle *handle);
    virtual void deactivateEditionMode();
    virtual void onActivateEditionMode();

    bool mHasMoved;
};

#endif // UBABSTRACTEDITABLEGRAPHICSPATHITEM_H
