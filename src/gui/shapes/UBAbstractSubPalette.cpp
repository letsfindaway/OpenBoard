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


#include "UBAbstractSubPalette.h"


const QString UBAbstractSubPalette::styleSheetLeftGroupedButton =   "QToolButton{background: qlineargradient(x1: 0, y1: 0.49, x2: 0, y2: 0.5,"
                                                                                "stop: 0 #d3d3d3, stop: 1  #c4c4c4);margin-top: 1px;margin-right: 0px;"
                                                                                "border: 1px solid #444444;border-right: 1px solid transparent;"
                                                                                "border-top-left-radius : 3px;border-bottom-left-radius : 3px;height: 24px;}"
                                                                                "QToolButton:checked { background: qlineargradient(x1: 0, y1: 0.49, x2: 0, y2: 0.5, stop: 0 #c3c3c3, stop: 1  #b4b4b4);"
                                                                                "border-right: 1px solid #444444;"
                                                                                " }";

const QString UBAbstractSubPalette::styleSheetCenterGroupedButton =     "QToolButton{background: qlineargradient(x1: 0, y1: 0.49, x2: 0, y2: 0.5, stop: 0 #d3d3d3, stop: 1  #c4c4c4);"
                                                                                    "margin-top: 1px;"
                                                                                    "margin-right: 0px;"
                                                                                    "margin-left: 0px;"
                                                                                    "border: 1px solid #444444;"
                                                                                    "border-right: 1px solid transparent;"
                                                                                    "border-left: 1px solid transparent;"
                                                                                    "height: 24px;}"
                                                                                    "QToolButton:checked { background: qlineargradient(x1: 0, y1: 0.49, x2: 0, y2: 0.5, stop: 0 #c3c3c3, stop: 1  #b4b4b4);"
                                                                                    "border-left: 1px solid #444444;"
                                                                                    "border-right: 1px solid #444444;"
                                                                                    "}";

const QString UBAbstractSubPalette::styleSheetRightGroupedButton =  "QToolButton{background: qlineargradient(x1: 0, y1: 0.49, x2: 0, y2: 0.5, stop: 0 #d3d3d3, stop: 1  #c4c4c4);"
                                                                                "margin-top: 1px;"
                                                                                "margin-left: 0px;"
                                                                                "border: 1px solid #444444;"
                                                                                "border-left: 1px solid transparent;"
                                                                                "border-top-right-radius : 3px;"
                                                                                "border-bottom-right-radius : 3px;"
                                                                                "height: 24px;}"
                                                                                "QToolButton:checked { background: qlineargradient(x1: 0, y1: 0.49, x2: 0, y2: 0.5, stop: 0 #c3c3c3, stop: 1  #b4b4b4);"
                                                                                "border-left: 1px solid #444444;"
                                                                                "}";

UBAbstractSubPalette::UBAbstractSubPalette(QWidget *parent, Qt::Orientation orient) :
    UBActionPalette(Qt::TopLeftCorner, parent, orient)
    , mMainAction(0)
{
    mCustomPosition = true;
    setFocusPolicy(Qt::StrongFocus); // in order to detect focus loss.
}

UBAbstractSubPalette::UBAbstractSubPalette(Qt::Orientation orient, QWidget *parent) :
    UBActionPalette(orient, parent)
    , mMainAction(0)
{
    mCustomPosition = true;
}

void UBAbstractSubPalette::togglePalette()
{
    show();
    setFocus(); // explicitely give focus to the subPalette, to detect when it loses focus, and then hide it .
}

void UBAbstractSubPalette::triggerAction(QAction *action)
{
    Q_UNUSED(action)
    // NOOP
}

void UBAbstractSubPalette::focusOutEvent(QFocusEvent *)
{
    hide(); // hide subPalette when it loses focus (for exemple : clic anywhere else)
}

void UBAbstractSubPalette::mouseMoveEvent(QMouseEvent *)
{
    // User can't move subPalette with the mouse.
}

UBColorPickerButton::UBColorPickerButton(QWidget *parent)
    :QToolButton(parent)
{
}

void UBColorPickerButton::paintEvent(QPaintEvent * pe)
{
    Q_UNUSED(pe);
    QPainter painter(this);
    painter.setBrush(color());    
    painter.drawRect(margin_left, margin_top, width, height);

    // If transparent color, draw a cross in the rect :
    if (color() == Qt::transparent)
    {
        painter.drawLine(margin_left,margin_top, width+margin_left,height+margin_top);
        painter.drawLine(width+margin_left,margin_top, margin_left,height+margin_top);
    }
}
