/*
 * Copyright (C) 2015-2024 Département de l'Instruction Publique (DIP-SEM)
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


#include "UBDesktopPortal.h"

#include <QDBusInterface>
#include <QDBusReply>
#include <QDBusUnixFileDescriptor>
#include <QDebug>

#include "board/UBBoardView.h"
#include "core/UBApplication.h"
#include "core/UBApplicationController.h"
#include "desktop/UBDesktopAnnotationController.h"

Q_DECLARE_METATYPE(UBDesktopPortal::Stream)
Q_DECLARE_METATYPE(UBDesktopPortal::Streams)

const QDBusArgument &operator >> (const QDBusArgument &arg, UBDesktopPortal::Stream &stream)
{
    arg.beginStructure();
    arg >> stream.node_id;

    arg.beginMap();

    while (!arg.atEnd())
    {
        QString key;
        QVariant value;
        arg.beginMapEntry();
        arg >> key >> value;
        arg.endMapEntry();
        stream.map.insert(key, value);
    }

    arg.endMap();
    arg.endStructure();

    return arg;
}

UBDesktopPortal::UBDesktopPortal(QObject* parent)
    : QObject{parent}
{
}

UBDesktopPortal::~UBDesktopPortal()
{
    if (mScreencastPortal)
    {
        delete mScreencastPortal;
    }
}

void UBDesktopPortal::startScreenCast(bool withCursor)
{
    mWithCursor = withCursor;

    QDBusInterface* portal = screencastPortal();

    if (!portal)
    {
        return;
    }

    // Create ScreenCast session
    QMap<QString, QVariant> options;
    options["session_handle_token"] = createSessionToken();
    options["handle_token"] = createRequestToken();

    const QDBusReply<QDBusObjectPath> reply = portal->call("CreateSession", options);

    if (!reply.isValid())
    {
        qWarning() << "Couldn't get reply to ScreenCast/CreateSession";
        qWarning() << "Error: " << reply.error().message();
        emit screenCastAborted();
        return;
    }

    const QDBusObjectPath objectPath = reply.value();
    const QString path = objectPath.path();
    qDebug() << "CreateSession response" << path;
    QDBusConnection::sessionBus().connect("", path, "org.freedesktop.portal.Request", "Response", "ua{sv}", this,
                                          SLOT(handleCreateSessionResponse(uint,QMap<QString,QVariant>)));
}

void UBDesktopPortal::stopScreenCast()
{
    if (mSession.isEmpty())
    {
        return;
    }

    QDBusInterface portal("org.freedesktop.portal.Desktop", mSession, "org.freedesktop.portal.Session");

    if (portal.isValid())
    {
        const QDBusReply<void> reply = portal.call("Close");

        if (!reply.isValid())
        {
            qWarning() << "Couldn't get reply to ScreenCast/Close";
            qWarning() << "Error: " << reply.error().message();
            return;
        }

        mSession.clear();
    }
}

void UBDesktopPortal::handleCreateSessionResponse(uint response, const QVariantMap& results)
{
    if (response != 0)
    {
        qWarning() << "Failed to create session: " << response << results;
        emit screenCastAborted();
        return;
    }

    mSession = results.value("session_handle").toString();

    QDBusInterface* portal = screencastPortal();

    if (!portal)
    {
        return;
    }

    // Select sources
    QMap<QString, QVariant> options;
    options["multiple"] = false;
    options["types"] = uint(1);
    options["cursor_mode"] = uint(mWithCursor ? 2 : 1);
    options["handle_token"] = createRequestToken();
    options["persist_mode"] = uint(2);

    if (!mRestoreToken.isEmpty())
    {
        options["restore_token"] = mRestoreToken;
    }

    const QDBusReply<QDBusObjectPath> reply = portal->call("SelectSources", QDBusObjectPath(mSession), options);

    if (!reply.isValid())
    {
        qWarning() << "Couldn't get reply";
        qWarning() << "Error: " << reply.error().message();
        emit screenCastAborted();
        return;
    }

    const QDBusObjectPath objectPath = reply.value();
    const QString path = objectPath.path();
    qDebug() << "SelectSources response" << path;
    QDBusConnection::sessionBus().connect("", path, "org.freedesktop.portal.Request", "Response", "ua{sv}", this,
                                          SLOT(handleSelectSourcesResponse(uint,QMap<QString,QVariant>)));
}

void UBDesktopPortal::handleSelectSourcesResponse(uint response, const QVariantMap& results)
{
    Q_UNUSED(results);

    if (response != 0)
    {
        qWarning() << "Failed to select sources: " << response;
        emit screenCastAborted();
        return;
    }

    QDBusInterface* portal = screencastPortal();

    if (!portal)
    {
        return;
    }

    // Start ScreenCast
    QMap<QString, QVariant> options;
    options["handle_token"] = createRequestToken();
    const QDBusReply<QDBusObjectPath> reply = portal->call("Start", QDBusObjectPath(mSession), "", options);

    if (!reply.isValid())
    {
        qWarning() << "Couldn't get reply";
        qWarning() << "Error: " << reply.error().message();
        emit screenCastAborted();
        return;
    }

    const QDBusObjectPath objectPath = reply.value();
    const QString path = objectPath.path();
    qDebug() << "Start response" << path;

    // Hide annotation drawing view in desktop mode so that portal dialog is topmost
    showGlassPane(false);

    QDBusConnection::sessionBus().connect("", path, "org.freedesktop.portal.Request", "Response", "ua{sv}", this,
                                          SLOT(handleStartResponse(uint,QMap<QString,QVariant>)));
}

void UBDesktopPortal::handleStartResponse(uint response, const QVariantMap& results)
{
    Q_UNUSED(results);

    // Show annotation drawing view in desktop mode after portal dialog was closed
    showGlassPane(true);

    if (response != 0)
    {
        // The system Desktop dialog was canceled
        qDebug() << "Failed to start or cancel dialog: " << response;
        emit screenCastAborted();
        return;
    }

    // save restore token
    mRestoreToken = results.value("restore_token").toString();

    const Streams streams = qdbus_cast<Streams>(results.value("streams"));
    const Stream stream = streams.last();

    QDBusInterface* portal = screencastPortal();

    if (!portal)
    {
        return;
    }

    // Open PipeWire Remote
    QMap<QString, QVariant> options;
    const QDBusReply<QDBusUnixFileDescriptor> reply = portal->call("OpenPipeWireRemote", QDBusObjectPath(mSession), options);

    if (!reply.isValid())
    {
        qWarning() << "Couldn't get reply";
        qWarning() << "Error: " << reply.error().message();
        emit screenCastAborted();
        return;
    }

    const quint64 fd = reply.value().fileDescriptor();
    const QString path = QString::number(stream.node_id);

    emit streamStarted(fd, path);
}

QDBusInterface* UBDesktopPortal::screencastPortal()
{
    if (!mScreencastPortal)
    {
        mScreencastPortal = new QDBusInterface("org.freedesktop.portal.Desktop", "/org/freedesktop/portal/desktop",
                                               "org.freedesktop.portal.ScreenCast");
        mScreencastPortal->setParent(this);
    }

    if (mScreencastPortal->isValid())
    {
        return mScreencastPortal;
    }

    emit screenCastAborted();
    return nullptr;
}

QString UBDesktopPortal::createSessionToken() const
{
    static int sessionTokenCounter = 0;

    sessionTokenCounter += 1;
    return QString("obsess%1").arg(sessionTokenCounter);
}

QString UBDesktopPortal::createRequestToken() const
{
    static int requestTokenCounter = 0;

    requestTokenCounter += 1;
    return QString("obreq%1").arg(requestTokenCounter);
}

void UBDesktopPortal::showGlassPane(bool show) const
{
    if (UBApplication::applicationController->isShowingDesktop())
    {
        UBApplication::applicationController->uninotesController()->drawingView()->setVisible(show);
    }
}
