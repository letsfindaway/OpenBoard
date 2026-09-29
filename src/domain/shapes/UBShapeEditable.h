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


#ifndef UBSHAPEEDITABLE_H
#define UBSHAPEEDITABLE_H

#include "UBAbstractGraphicsItem.h"
#include "UBEditable.h"

class UBAbstractEditableGraphicsShapeItem : public UBAbstractGraphicsItem, public UBAbstractEditable
{
public:
    UBAbstractEditableGraphicsShapeItem(QGraphicsItem *parent = 0);

    virtual ~UBAbstractEditableGraphicsShapeItem(){ }
protected:
    virtual QPainterPath shape() const override;

    virtual void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;
    virtual void focusOutEvent(QFocusEvent *event) override;
    virtual void paintCenterMark(QPainter *painter);

    virtual void onActivateEditionMode();

    virtual void deactivateEditionMode() override;

    bool mHasMoved;
};

#endif // UBSHAPEEDITABLE_H
