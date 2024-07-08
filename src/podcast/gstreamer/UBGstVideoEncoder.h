#pragma once

#include "podcast/UBAbstractVideoEncoder.h"

// forward
typedef struct _GstElement GstElement;
typedef struct _GstEncodingProfile GstEncodingProfile;
class UBMicrophoneInput;

class UBGstVideoEncoder : public UBAbstractVideoEncoder
{
    Q_OBJECT

public:
    UBGstVideoEncoder(QObject* parent = nullptr);
    virtual ~UBGstVideoEncoder();

    virtual bool start() override;
    virtual bool stop() override;
    virtual void newPixmap(const QImage& pImage, long timestamp) override;
    virtual QString videoFileExtension() const override;
    virtual QString lastErrorMessage() override;
    virtual void setRecordAudio(bool pRecordAudio) override;

private slots:
    void onAudioAvailable(QByteArray data);

private:
    GstElement* setupPipeline();
    GstEncodingProfile* createProfile();
    void setupAudio();

private:
    QString mLastErrorMessage;
    bool mShouldRecordAudio{false};
    UBMicrophoneInput* mAudioInput{nullptr};
    GstElement* mPipeline{nullptr};
};
