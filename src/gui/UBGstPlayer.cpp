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


#include "UBGstPlayer.h"

#include <QQuickItem>
#include <QRunnable>

#include <gst/gst.h>

// Private helper class to set the Playing state when the widget is rendered
class SetPlaying : public QRunnable
{
public:
    SetPlaying(GstElement*);
    ~SetPlaying();

    void run();

private:
    GstElement* mPipeline;
};

SetPlaying::SetPlaying(GstElement* pipeline)
{
    mPipeline = pipeline ? static_cast<GstElement*>(gst_object_ref(pipeline)) : nullptr;
}

SetPlaying::~SetPlaying()
{
    if (mPipeline)
    {
        gst_object_unref(mPipeline);
    }
}

void SetPlaying::run()
{
    if (mPipeline)
    {
        gst_element_set_state(mPipeline, GST_STATE_PLAYING);
    }
}

UBGstPlayer::UBGstPlayer(QWidget* parent)
    : QQuickWidget(parent)
{
    setResizeMode(QQuickWidget::SizeRootObjectToView);
}

UBGstPlayer::~UBGstPlayer()
{
    if (mPipeline)
    {
        gst_element_set_state(mPipeline, GST_STATE_NULL);
        gst_object_unref(mPipeline);
    }
}

void UBGstPlayer::setStream(qint64 fd, const QString& path)
{
    mFd = fd;
    mPath = path;
}

void UBGstPlayer::play()
{
    if (!mFd || mPath.isEmpty())
    {
        qWarning() << "Stream not set";
        return;
    }

    qDebug() << "create pipeline" << mFd << mPath;
    mPipeline = createPipeline(mFd, mPath);
    GstElement* sink = gst_bin_get_by_name(GST_BIN(mPipeline), "sink");

    // load the qml
    setSource(QUrl(QStringLiteral("qrc:/gstvideo.qml")));

    // find and set the videoItem on the sink
    QQuickItem* videoItem = rootObject()->findChild<QQuickItem*>("videoItem");

    g_assert(videoItem && sink);
    g_object_set(sink, "widget", videoItem, nullptr);

    // trigger play state
    quickWindow()->scheduleRenderJob(new SetPlaying(mPipeline), QQuickWindow::BeforeSynchronizingStage);
}

void UBGstPlayer::stop()
{
    if (mPipeline)
    {
        gst_element_set_state(mPipeline, GST_STATE_NULL);
    }
}

void UBGstPlayer::resizeEvent(QResizeEvent* event)
{
    QQuickItem* videoItem = rootObject()->findChild<QQuickItem*>("videoItem");

    if (videoItem && !event->size().isNull())
    {
        videoItem->setSize(event->size());
    }
}

GstElement* UBGstPlayer::createPipeline(qint64 fd, const QString& path) const
{
    if (!gst_is_initialized())
    {
        gst_init(nullptr, nullptr);
    }

    // create the pipeline
    // use a qmlglsink to play the pipeline in a QML item
    GstElement* pipeline = gst_pipeline_new(nullptr);

    GstElement* src = gst_element_factory_make("pipewiresrc", nullptr);
    g_object_set(src, "fd", fd, nullptr);
    g_object_set(src, "path", path.toStdString().c_str(), nullptr);

    GstElement* convert = gst_element_factory_make("videoconvert", nullptr);
    GstElement* rate = gst_element_factory_make("videorate", nullptr);
    GstElement* glupload = gst_element_factory_make("glupload", nullptr);

    // the plugin must be loaded before loading the qml file to register the
    // GstGLVideoItem qml item
#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
    GstElement* sink = gst_element_factory_make("qml6glsink", "sink");
#else
    GstElement* sink = gst_element_factory_make("qmlglsink", "sink");
#endif

    g_assert(src && glupload && sink);

    gst_bin_add_many(GST_BIN(pipeline), src, convert, rate, glupload, sink, nullptr);
    gst_element_link_many(src, convert, rate, glupload, sink, nullptr);

    return pipeline;
}
