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


#ifndef UBABSTRACTSUBPALETTE_H
#define UBABSTRACTSUBPALETTE_H

#include "gui/UBActionPalette.h"
#include <QAction>

class UBAbstractSubPalette : public UBActionPalette
{
    public :
        UBAbstractSubPalette(QWidget *parent = 0, Qt::Orientation orient = Qt::Vertical);
        UBAbstractSubPalette(Qt::Orientation orient, QWidget *parent = 0 );

        virtual void togglePalette();
        QAction* mainAction() { return actions().at(mMainAction); }

        UBActionPaletteButton * actionPaletteButtonParent() const {return mActionPaletteButtonParent;}
        void setActionPaletteButtonParent(UBActionPaletteButton * button){mActionPaletteButtonParent = button;}

        virtual void triggerAction(QAction * action);

    protected :
        int mMainAction;

        UBActionPaletteButton * mActionPaletteButtonParent; // button that opened this subPalette.

        // QWidget interface
protected:
        virtual void focusOutEvent(QFocusEvent *);
        virtual void mouseMoveEvent(QMouseEvent *);

        // EV-7 - CFA - 20140127 : ...
        //unable to use Uniboard.css for custom classes, even with overiding paintEvent as Qt recommands...
        //use .css and setObjectName should be preferable, but no more time, and not enough knowledge...
        static const QString styleSheetLeftGroupedButton;
        static const QString styleSheetCenterGroupedButton;
        static const QString styleSheetRightGroupedButton;
};

class UBColorPickerButton : public QToolButton
{
    public:
        UBColorPickerButton(QWidget * parent = 0);

        QColor color() const {return mColor;}
        void setColor(QColor color) {mColor = color;}

        static const int iconSize = 32;

    // QWidget interface
    protected:
        virtual void paintEvent(QPaintEvent * pe);

    private:
        QColor mColor;

        static const int margin_left = 2;
        static const int margin_top = 5;
        static const int width = 20;
        static const int height = 16;
};


#endif // UBABSTRACTSUBPALETTE_H
