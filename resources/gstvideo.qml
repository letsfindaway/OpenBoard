import QtQuick 2.15
import org.freedesktop.gstreamer.GLVideoItem 1.0

Item {
    Rectangle {
        color: "black"
        width : video.width
        height: video.height
    }

    GstGLVideoItem {
        id: video
        objectName: "videoItem"
        width: 1024
        height: 640
    }
}
