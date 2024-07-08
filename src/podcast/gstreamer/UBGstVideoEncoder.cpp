#include "UBGstVideoEncoder.h"


#include <gst/gst.h>
#include <QImage>
#include <gst/pbutils/encoding-profile.h>

#include "podcast/ffmpeg/UBMicrophoneInput.h"

UBGstVideoEncoder::UBGstVideoEncoder(QObject* parent)
    : UBAbstractVideoEncoder(parent)
{
}

UBGstVideoEncoder::~UBGstVideoEncoder()
{
}

bool UBGstVideoEncoder::start()
{
    if (!gst_is_initialized())
    {
        gst_init(nullptr, nullptr);
    }

    mPipeline = setupPipeline();

    if (mShouldRecordAudio) {
        setupAudio();
        mAudioInput->start();
    }

    gst_element_set_state(mPipeline, GST_STATE_PLAYING);
    return true;
}

bool UBGstVideoEncoder::stop()
{
    // write end-of-stream
    GstElement* appsrc = gst_bin_get_by_name(GST_BIN(mPipeline), "videosrc");
    GstFlowReturn ret;
    g_signal_emit_by_name(appsrc, "end-of-stream", &ret);

    if (mShouldRecordAudio)
    {
        appsrc = gst_bin_get_by_name(GST_BIN(mPipeline), "audiosrc");
        g_signal_emit_by_name(appsrc, "end-of-stream", &ret);
        mAudioInput->stop();
    }

    // wait for the EOS to traverse the pipeline and be reported to the bus
    GstBus* bus = gst_element_get_bus(mPipeline);
    gst_bus_timed_pop_filtered(bus, GST_CLOCK_TIME_NONE, GST_MESSAGE_EOS);
    gst_object_unref(bus);

    gst_element_set_state(mPipeline, GST_STATE_NULL);
    gst_object_unref(mPipeline);
    mPipeline = nullptr;
    return true;
}

void UBGstVideoEncoder::newPixmap(const QImage& pImage, long timestamp)
{
    // create a new buffer from the image
    GstBuffer* buffer = gst_buffer_new_memdup(pImage.bits(), pImage.sizeInBytes());

    // set its timestamp and duration
    GST_BUFFER_TIMESTAMP(buffer) = gst_util_uint64_scale(timestamp, GST_MSECOND, 1);
    GST_BUFFER_DURATION(buffer) = gst_util_uint64_scale(1, GST_SECOND, framesPerSecond());

    /* Push the buffer into the appsrc */
    GstElement* appsrc = gst_bin_get_by_name(GST_BIN(mPipeline), "videosrc");
    GstFlowReturn ret;
    g_signal_emit_by_name(appsrc, "push-buffer", buffer, &ret);

    // free the buffer now that we are done with it
    gst_buffer_unref(buffer);
}

QString UBGstVideoEncoder::videoFileExtension() const
{
    return "ogg";
}

QString UBGstVideoEncoder::lastErrorMessage()
{
    return mLastErrorMessage;
}

void UBGstVideoEncoder::setRecordAudio(bool pRecordAudio)
{
    mShouldRecordAudio = pRecordAudio;
}

void UBGstVideoEncoder::onAudioAvailable(QByteArray data)
{
    static long timestamp = 0;

    int inSamplesCount = data.size() / ((mAudioInput->sampleSize() / 8) * mAudioInput->channelCount());

    // create a new buffer from the audio data
    GstBuffer* buffer = gst_buffer_new_memdup(data.constData(), data.size());

    // set its timestamp and duration
    GST_BUFFER_TIMESTAMP(buffer) = gst_util_uint64_scale(timestamp, GST_MSECOND, 1);
    GST_BUFFER_DURATION(buffer) = gst_util_uint64_scale(inSamplesCount, GST_SECOND, mAudioInput->sampleRate());
    timestamp += GST_BUFFER_DURATION(buffer) / 1000000LL;

    /* Push the buffer into the appsrc */
    GstElement* appsrc = gst_bin_get_by_name(GST_BIN(mPipeline), "audiosrc");
    GstFlowReturn ret;
    g_signal_emit_by_name(appsrc, "push-buffer", buffer, &ret);

    // free the buffer now that we are done with it
    gst_buffer_unref(buffer);
}

GstElement* UBGstVideoEncoder::setupPipeline()
{
    // We have two options for video input:
    // - Images coming via appsrc plugin
    // - and a stream from ScreenCast via pipewiresrc plugin
    // And one optional audio input using the appsrc plugin
    // We setup all elements and dynamically link and unlink the video sources
    // https://gstreamer.freedesktop.org/documentation/application-development/advanced/pipeline-manipulation.html#dynamically-changing-the-pipeline
    // Note that threads can be created by inserting a queue element in the pipeline
    // https://gstreamer.freedesktop.org/documentation/tutorials/basic/multithreading-and-pad-availability.html?gi-language=c
    // see also https://gstreamer.freedesktop.org/documentation/encoding/encodebin.html?gi-language=c
    // Profile could be video/quicktime,variant=iso:video/x-h264:audio/mpeg,mpegversion=1,layer=3 represents mp4
    GstElement* pipeline = gst_pipeline_new(nullptr);

    // create video source
    //    GstElement* videosrc = gst_element_factory_make("videotestsrc", "videosrc");
    GstElement* videosrc = gst_element_factory_make("appsrc", "videosrc");
    // TODO the size may change between frames
    QSize size = videoSize();
    GstCaps* caps = gst_caps_new_simple("video/x-raw",
                                        "format", G_TYPE_STRING, "BGRx",
                                        "framerate", GST_TYPE_FRACTION, framesPerSecond(), 1,
                                        "pixel-aspect-ratio", GST_TYPE_FRACTION, 1, 1,
                                        "width", G_TYPE_INT, size.width(),
                                        "height", G_TYPE_INT, size.height(),
                                        nullptr);
    g_object_set(videosrc, "caps", caps, "format", GST_FORMAT_TIME, nullptr);
    gst_caps_unref(caps);

    GstElement* videoqueue = gst_element_factory_make("queue", "videoqueue");
    GstElement* videoconvert = gst_element_factory_make("videoconvert", "videoconvert");
    GstElement* videoscale = gst_element_factory_make("videoscale", "videoscale");

    // link video
    gst_bin_add_many(GST_BIN(pipeline), videosrc, videoqueue, videoconvert, videoscale, nullptr);
    gst_element_link_many(videosrc, videoqueue, videoconvert, videoscale, nullptr);

    // create encodebin
    GstElement* encodebin = gst_element_factory_make("encodebin", "encodebin");
    GstEncodingProfile* profile = createProfile();
    g_object_set(encodebin, "profile", profile, nullptr);

    // create filesink
    GstElement* filesink = gst_element_factory_make("filesink", "filesink");
    g_object_set(filesink, "location", videoFileName().toStdString().c_str(), nullptr);

    // link filesink to encodebin
    gst_bin_add_many(GST_BIN(pipeline), encodebin, filesink, nullptr);
    gst_element_link_many(encodebin, filesink, nullptr);

    // link video to encodebin
    GstPad* videopad = gst_element_get_static_pad(videoscale, "src");
    GstPad* videosink = gst_element_request_pad_simple(encodebin, "video_%u");
    g_assert(videopad && videosink);
    gst_pad_link(videopad, videosink);

    return pipeline;
}

GstEncodingProfile* UBGstVideoEncoder::createProfile()
{
    // https://gstreamer.freedesktop.org/documentation/pbutils/encoding-profile.html?gi-language=c
    GstEncodingContainerProfile* prof;
    GstCaps* caps;

    caps = gst_caps_from_string("application/ogg");
    prof = gst_encoding_container_profile_new("Ogg audio/video", "Standard OGG/THEORA/VORBIS", caps, nullptr);
    gst_caps_unref(caps);

    caps = gst_caps_from_string("video/x-theora");
    gst_encoding_container_profile_add_profile(
        prof, (GstEncodingProfile*)gst_encoding_video_profile_new(caps, nullptr, nullptr, 0));
    gst_caps_unref(caps);

    caps = gst_caps_from_string("audio/x-vorbis");
    gst_encoding_container_profile_add_profile(
        prof, (GstEncodingProfile*)gst_encoding_audio_profile_new(caps, nullptr, nullptr, 0));
    gst_caps_unref(caps);

    return (GstEncodingProfile*)prof;
}

void UBGstVideoEncoder::setupAudio()
{
    mAudioInput = new UBMicrophoneInput();

    connect(mAudioInput, &UBMicrophoneInput::audioLevelChanged,
            this, &UBAbstractVideoEncoder::audioLevelChanged);

    connect(mAudioInput, &UBMicrophoneInput::dataAvailable,
            this, &UBGstVideoEncoder::onAudioAvailable);

    mAudioInput->setInputDevice(audioRecordingDevice());

    if (!mAudioInput->init()) {
        mLastErrorMessage = "Couldn't initialize audio input";
        return;
    }

    int inChannelCount = mAudioInput->channelCount();
    int inSampleRate = mAudioInput->sampleRate();
    int inSampleFormat = mAudioInput->sampleFormat();

    static const char* sampleFormat[] = { "UNKNOWN", "U8", "S16LE", "S32LE", "F32LE"};

    if (inSampleFormat > 3)
    {
        qWarning() << "Unsupported sample format" << inSampleFormat;
        return;
    }

    // create audio source
    GstElement* audiosrc = gst_element_factory_make("appsrc", "audiosrc");
    // https://gstreamer.freedesktop.org/documentation/additional/design/mediatype-audio-raw.html?gi-language=c
    GstCaps* caps = gst_caps_new_simple("audio/x-raw",
                                        "format", G_TYPE_STRING, sampleFormat[inSampleFormat + 1],
                                        "channels", G_TYPE_INT, inChannelCount,
                                        "rate", G_TYPE_INT, inSampleRate,
                                        "layout", G_TYPE_STRING, "interleaved",
                                        nullptr);
    g_object_set(audiosrc, "caps", caps, "format", GST_FORMAT_TIME, nullptr);
    gst_caps_unref(caps);
    GstElement* audioqueue = gst_element_factory_make("queue", "audioqueue");
    GstElement* audioconvert = gst_element_factory_make("audioconvert", "audioconvert");
    GstElement* audioresample = gst_element_factory_make("audioresample", "audioresample");

    // link audio
    gst_bin_add_many(GST_BIN(mPipeline), audiosrc, audioqueue, audioconvert, audioresample, nullptr);
    gst_element_link_many(audiosrc, audioqueue, audioconvert, audioresample, nullptr);

    // link audio to encodebin
    GstElement* encodebin = gst_bin_get_by_name(GST_BIN(mPipeline), "encodebin");
    GstPad* audiopad = gst_element_get_static_pad(audioresample, "src");
    GstPad* audiosink = gst_element_request_pad_simple(encodebin, "audio_%u");
    g_assert(audiopad && audiosink);
    gst_pad_link(audiopad, audiosink);

}
