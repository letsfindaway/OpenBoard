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



#include "UBScreenMirror.h"

#include "core/UBSettings.h"
#include "core/UBSetting.h"
#include "core/UBApplication.h"
#include "core/UBDisplayManager.h"
#include "board/UBBoardController.h"
#include "frameworks/UBDesktopPortal.h"
#include "gui/UBGstPlayer.h"

#if defined(Q_OS_OSX)
#include <ApplicationServices/ApplicationServices.h>
#endif

#include "core/memcheck.h"


UBScreenMirror::UBScreenMirror(QWidget* parent)
    : QWidget(parent)
    , mSourceWidget(0)
    , mTimerID(0)
{
    // NOOP
}


UBScreenMirror::~UBScreenMirror()
{
    // NOOP
}


void UBScreenMirror::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QPainter painter(this);

    painter.fillRect(0, 0, width(), height(), QBrush(Qt::black));

    if (!mLastPixmap.isNull())
    {
        int x = (width() - mLastPixmap.width()) / 2;
        int y = (height() - mLastPixmap.height()) / 2;

        painter.drawPixmap(x, y, width(), height(), mLastPixmap);
    }
}


void UBScreenMirror::timerEvent(QTimerEvent *event)
{
    Q_UNUSED(event);

    grabPixmap();

    update();
}

void UBScreenMirror::grabPixmap()
{
    if (mSourceWidget)
    {
        mLastPixmap = mSourceWidget->grab();

        if (!mLastPixmap.isNull())
            mLastPixmap = mLastPixmap.scaled(width(), height(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
    }
    else
    {
        UBApplication::displayManager->grab(ScreenRole::Control, [this](QPixmap pixmap){
            if (!pixmap.isNull())
                mLastPixmap = pixmap.scaled(width(), height(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
        });
    }
}

void UBScreenMirror::startScreenCast()
{
    // use UBDesktopPortal and UBGstPlayer
    if (!mPortal)
    {
        mPortal = new UBDesktopPortal(this);

        connect(mPortal, &UBDesktopPortal::streamStarted, this, &UBScreenMirror::playStream);
        connect(mPortal, &UBDesktopPortal::screenCastAborted, this, [](){
            UBApplication::applicationController->mirroringEnabled(false);
        });
    }

    mPortal->startScreenCast(true);
}

void UBScreenMirror::playStream(qint64 fd, const QString& path)
{
    qDebug() << "Start stream player";

    if (!mPlayer)
    {
        mPlayer = new UBGstPlayer(this);
    }

    mPlayer->setStream(fd, path);
    mPlayer->move(0, 0);
    mPlayer->resize(this->size());
    mPlayer->play();
    mPlayer->show();
}


void UBScreenMirror::setSourceWidget(QWidget *sourceWidget)
{
    mSourceWidget = sourceWidget;

    grabPixmap();

    update();
}


void UBScreenMirror::start()
{
    qDebug() << "mirroring START";
    UBApplication::boardController->freezeW3CWidgets(true);

    QString sessionType = QProcessEnvironment::systemEnvironment().value("XDG_SESSION_TYPE", "");

    if (mSourceWidget == nullptr && sessionType == "wayland")
    {
        // use desktop portal to start a screencast
        mLastPixmap = {};
        startScreenCast();
        return;
    }

    if (mTimerID == 0)
    {
        int ms = 125;

        bool success;
        int fps = UBSettings::settings()->mirroringRefreshRateInFps->get().toInt(&success);

        if (success && fps > 0)
        {
            ms = 1000 / fps;
        }

        mTimerID = startTimer(ms);
    }
    else
    {
        qDebug() << "UBScreenMirror::start() : Timer already running ...";
    }
}


void UBScreenMirror::stop()
{
    qDebug() << "mirroring STOP";
    UBApplication::boardController->freezeW3CWidgets(false);
    if (mTimerID != 0)
    {
        killTimer(mTimerID);
        mTimerID = 0;
    }

    if (mPortal)
    {
        mPortal->stopScreenCast();
    }

    if (mPlayer)
    {
        mPlayer->stop();
        mPlayer->hide();
        mPlayer->resize(1024, 768);
    }
}
