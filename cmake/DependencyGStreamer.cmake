# Find GStreamer
#
# GStreamer only supports PkgConfig

find_package(PkgConfig REQUIRED)
pkg_check_modules(GStreamer REQUIRED IMPORTED_TARGET gstreamer-1.0 gstreamer-video-1.0)

if (GStreamer_FOUND)
    target_link_libraries(${PROJECT_NAME} 
        PkgConfig::GStreamer
    )
endif()
