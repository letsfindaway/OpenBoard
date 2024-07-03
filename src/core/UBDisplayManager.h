/*
 * Copyright (C) 2015-2022 Département de l'Instruction Publique (DIP-SEM)
 *
 * Copyright (C) 2013 Open Education Foundation
 *
 * Copyright (C) 2010-2013 Groupement d'Intérêt Public pour
 * l'Education Numérique en Afrique (GIP ENA)
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




#ifndef UBDISPLAYMANAGER_H_
#define UBDISPLAYMANAGER_H_

#include <QtGui>
#include <QMap>

class UBBlackoutWidget;
class UBBoardView;

namespace Ui
{
    class BlackoutWidget;
}

enum class ScreenRole : int
{
    None = 0, Control, Display, Desktop, Previous1, Previous2, Previous3, Previous4, Previous5
};

// prefix increment
ScreenRole& operator++(ScreenRole& role);

// postfix increment
ScreenRole operator++(ScreenRole& role, int);

class UBDisplayManager : public QObject
{
    Q_OBJECT;

    public:
        UBDisplayManager(QObject *parent = 0);
        virtual ~UBDisplayManager();

        int numScreens();

        int numPreviousViews();

        void setControlWidget(QWidget* pControlWidget);

        void setDisplayWidget(QWidget* pDisplayWidget);

        void setDesktopWidget(QWidget* pDesktopWidget);

        void setPreviousDisplaysWidgets(QList<UBBoardView*> pPreviousViews);

        QWidget* widget(ScreenRole role) const;
        QScreen* screen(ScreenRole role) const;

        QList<QScreen*> availableScreens() const;

        bool hasControl()
        {
            return mScreensByRole.value(ScreenRole::Control);
        }

        bool hasDisplay()
        {
            return mScreensByRole.value(ScreenRole::Display);
        }

        bool hasPrevious()
        {
            return mScreensByRole.value(ScreenRole::Previous1);
        }

        bool useMultiScreen()
        {
            return mUseMultiScreen;
        }

        void setUseMultiScreen(bool pUse);

        QSize screenSize(ScreenRole role) const;
        QSize availableScreenSize(ScreenRole role) const;
        QRect screenGeometry(ScreenRole role) const;
        qreal physicalDpi(ScreenRole role) const;
        qreal logicalDpi(ScreenRole role) const;

        void grab(ScreenRole role, std::function<void(QPixmap)> callback, QRect rect = QRect(0, 0, -1, -1)) const;
        QPixmap grabGlobal(QRect rect) const;

        void initScreensByRole();
        void assignRoles();
        void positionScreens();
        void adjustScreens();

   signals:
        void screenRolesAssigned();
        void screenLayoutChanged();
        void availableScreenCountChanged(int screenCount);

   public slots:

        void blackout();

        void unBlackout();

    private slots:

        void addOrRemoveScreen(QScreen* screen);

    private:

        void initScreenIndexes();

        QList<UBBlackoutWidget*> mBlackoutWidgets;
        QList<Ui::BlackoutWidget*> mBlackoutUiList;

        QList<QScreen*> mAvailableScreens;
        QMap<ScreenRole, QPointer<QScreen>> mScreensByRole;
        QMap<ScreenRole, QPointer<QWidget>> mWidgetsByRole;

        bool mUseMultiScreen;
};

#endif /* UBDISPLAYMANAGER_H_ */
