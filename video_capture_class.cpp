//////////////////////////////////////////////////
//                                              //
// Emu64                                        //
// von Thorsten Kattanek                        //
//                                              //
// #file: video_capture_class.cpp               //
//                                              //
// Dieser Sourcecode ist Copyright geschützt!   //
// Geistiges Eigentum von Th.Kattanek           //
//                                              //
// Letzte Änderung am 24.09.2017                //
// www.emu64.de                                 //
//                                              //
//////////////////////////////////////////////////

#include "video_capture_class.h"
#include "video_capture_class.h"

VideoCaptureClass::VideoCaptureClass()
{
    CaptureIsActive = false;
    CaptureIsPause = false;

    FormatCtx = NULL;
    AudioCodec = VideoCodec = NULL;
    VideoStream = AudioStream = {0};

    SourceAudioData = new unsigned short[SOURCE_SAMPLE_BUFFER_LEN];

    AudioBitrate = 64000;
    VideoBitrate = 400000;

    RecordedFrames = 0;

    cout << "FFMPEG Version: " << GetAVVersion() << endl;
}

VideoCaptureClass::~VideoCaptureClass()
{
    if(CaptureIsActive)
    {
        cout << "VideoCaptureClass: " << "Stoppen erzwingen!" << endl;
        StopCapture();

        delete[] SourceAudioData;
    }
}

const char* VideoCaptureClass::GetAVVersion()
{
    return av_version_info();
}

void VideoCaptureClass::SetVideoBitrate(int video_bitrate)
{
    VideoBitrate = video_bitrate;
}

void VideoCaptureClass::SetAudioBitrate(int audio_bitrate)
{
    AudioBitrate = audio_bitrate;
}

bool VideoCaptureClass::StartCapture(const char *filename, const char *codec_name, int xw, int yw)
{
    if(CaptureIsActive) return false;

    RecordedFrames = 0;

    VideoXW = xw;
    VideoYW = yw;

    HaveVideo = HaveAudio = false;
    EncodeVideo = EncodeAudio = 0;

    AvailableAudioData = false;
    SourceAudioDataLength = 0;
    FrameSamplesPt = 0;

    Options = NULL;

    int ret;

    av_register_all();

    //
    avformat_alloc_output_context2(&FormatCtx, NULL, NULL, filename);
    if (!FormatCtx)
    {
        cerr << "Es konnte das Ausgabe Format nicht anhand des Dateinamens ermittelt werden. Es wird versucht MPEG zu benutzten." << endl;
        avformat_alloc_output_context2(&FormatCtx, NULL, "mpeg", filename);
    }
    if (!FormatCtx)
    {
        cerr << "Es konnte kein FormatContext erstellt werden." << endl;
        return false;
    }

    OutputFormat = FormatCtx->oformat;

    // Hinzufügen des Audio und Videostreams mit den Default Format Codecs
    // Und der Initialisierung der Codecs
    if (OutputFormat->video_codec != AV_CODEC_ID_NONE)
    {
        AddStream(&VideoStream, FormatCtx, &VideoCodec, OutputFormat->video_codec);
        HaveVideo = true;
        EncodeVideo = 1;
    }
    if (OutputFormat->audio_codec != AV_CODEC_ID_NONE)
    {
        AddStream(&AudioStream, FormatCtx, &AudioCodec, OutputFormat->audio_codec);
        HaveAudio = true;
        EncodeAudio = 1;
    }


    /* Now that all the parameters are set, we can open the audio and
     * video codecs and allocate the necessary encode buffers. */
    if (HaveVideo)
        OpenVideo(FormatCtx, VideoCodec, &VideoStream, Options);
    if (HaveAudio)
        OpenAudio(FormatCtx, AudioCodec, &AudioStream, Options);

    av_dump_format(FormatCtx, 0, filename, 1);

    // Öffnen der Ausgabedatei
    if (!(OutputFormat->flags & AVFMT_NOFILE))
    {
        ret = avio_open(&FormatCtx->pb, filename, AVIO_FLAG_WRITE);
        if (ret < 0)
        {
            char err_msg[AV_ERROR_MAX_STRING_SIZE];
            cerr << "Ausgabedatei kann nicht geöffnet werden: [" << filename << "[  -- " << av_make_error_string(err_msg,AV_ERROR_MAX_STRING_SIZE,ret) << endl;
            StopCapture();
            return false;
        }
    }

    // Schreiben des Stream Headers
    ret = avformat_write_header(FormatCtx, &Options);
    if (ret < 0)
    {
        char err_msg[AV_ERROR_MAX_STRING_SIZE];
        cerr << "Beim öffnen der Ausgabedatei ist ein Fehler aufgetreten.  -- " << av_make_error_string(err_msg,AV_ERROR_MAX_STRING_SIZE,ret) << endl;
        StopCapture();
        return false;
    }

    // Ausgabe von aktuellen Aufnahme Parametern
    cout << "Video Auflösung: " << VideoXW << " x " << VideoYW << endl;
    cout << "VideoCodec: " << VideoCodec->name << endl;
    cout << "AudioCodec: " << AudioCodec->name << endl;
    cout << "AudioFrameSize: " << AudioStream.enc->frame_size << endl;

    FrameAudioDataL = new unsigned short[AudioStream.enc->frame_size];
    FrameAudioDataR = new unsigned short[AudioStream.enc->frame_size];

    CaptureIsActive = true;
    return true;
}

void VideoCaptureClass::StopCapture()
{
    cout << "VideoCapture wird gestoppt" << endl;

    CaptureIsActive = false;

    // Trailer schreiben
    av_write_trailer(FormatCtx);

    // VideoStream schließen
    if (HaveVideo)
    {
        CloseStream(FormatCtx, &VideoStream);
        VideoStream = {0};
    }

    // AudioStream schließen
    if (HaveAudio)
    {
        CloseStream(FormatCtx, &AudioStream);
        AudioStream = {0};
    }

    // Ausgabedatei schließen
    if (!(OutputFormat->flags & AVFMT_NOFILE))
        /* Close the output file. */
        avio_closep(&FormatCtx->pb);

    avformat_free_context(FormatCtx);
    FormatCtx = NULL;

    delete[] FrameAudioDataL;
    delete[] FrameAudioDataR;
}

void VideoCaptureClass::SetCapturePause(bool cpt_pause)
{
    CaptureIsPause = cpt_pause;
}

void VideoCaptureClass::AddFrame(uint8_t *data, int linesize)
{
    if(!CaptureIsActive || CaptureIsPause) return;

    SourceVideoData = data;
    SourceVideoLineSize = linesize;
    EncodeVideo = !WriteVideoFrame(FormatCtx, &VideoStream);

    int n_sample = AudioStream.enc->frame_size;

    if(AvailableAudioData)
    {
        unsigned short* src = SourceAudioData;

        if(SourceAudioDataLength/2 < n_sample)
        {
           for(int i=0; i<SourceAudioDataLength/2; i++)
           {
                FrameAudioDataL[FrameSamplesPt] = *src++;
                FrameAudioDataR[FrameSamplesPt++] = *src++;
                if(FrameSamplesPt == n_sample)
                {
                    FrameSamplesPt = 0;
                    EncodeAudio = !WriteAudioFrame(FormatCtx, &AudioStream);
                }
           }
        }
        AvailableAudioData = false;
    }

    RecordedFrames++;
}

void VideoCaptureClass::FillSourceAudioBuffer(uint16_t *data, int len)
{
    if(!CaptureIsActive) return;

    if(AvailableAudioData)
        cout << "Error: Audio Buffer Overflow !!" << endl;

    if(len > SOURCE_SAMPLE_BUFFER_LEN)
    {
        cerr << "Error: Sampledaten größer als Buffer !!" << endl;
        return;
    }

    for(int i=0; i<len; i++)
        SourceAudioData[i] = data[i];

    SourceAudioDataLength = len;

    AvailableAudioData = true;
}

int VideoCaptureClass::GetRecordedFrameCount()
{
    return RecordedFrames;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void VideoCaptureClass::AddStream(OutputStream *ost, AVFormatContext *oc, AVCodec **codec, enum AVCodecID codec_id)
{
    AVCodecContext *c;
    int i;
    /* find the encoder */
    *codec = avcodec_find_encoder(codec_id);
    if (!(*codec)) {
        fprintf(stderr, "Could not find encoder for '%s'\n",
                avcodec_get_name(codec_id));
        exit(1);
    }


    ost->st = avformat_new_stream(oc, NULL);
    if (!ost->st) {
        fprintf(stderr, "Could not allocate stream\n");
        exit(1);
    }

    ost->st->id = oc->nb_streams-1;
    c = avcodec_alloc_context3(*codec);
    if (!c) {
        fprintf(stderr, "Could not alloc an encoding context\n");
        exit(1);
    }
    ost->enc = c;
    switch ((*codec)->type) {
    case AVMEDIA_TYPE_AUDIO:
        c->sample_fmt  = (*codec)->sample_fmts ?
            (*codec)->sample_fmts[0] : AV_SAMPLE_FMT_FLTP;
        c->bit_rate    = AudioBitrate;
        c->sample_rate = 44100;
        if ((*codec)->supported_samplerates) {
            c->sample_rate = (*codec)->supported_samplerates[0];
            for (i = 0; (*codec)->supported_samplerates[i]; i++) {
                if ((*codec)->supported_samplerates[i] == 44100)
                    c->sample_rate = 44100;
            }
        }
        c->channels        = av_get_channel_layout_nb_channels(c->channel_layout);
        c->channel_layout = AV_CH_LAYOUT_STEREO;
        if ((*codec)->channel_layouts) {
            c->channel_layout = (*codec)->channel_layouts[0];
            for (i = 0; (*codec)->channel_layouts[i]; i++) {
                if ((*codec)->channel_layouts[i] == AV_CH_LAYOUT_STEREO)
                    c->channel_layout = AV_CH_LAYOUT_STEREO;
            }
        }
        c->channels        = av_get_channel_layout_nb_channels(c->channel_layout);
        ost->st->time_base = (AVRational){ 1, c->sample_rate };
        break;

    //////////////////////////////////////////////////////////////////////////////////////////////////////////

    case AVMEDIA_TYPE_VIDEO:
        c->codec_id = codec_id;
        c->bit_rate = VideoBitrate;
        /* Resolution must be a multiple of two. */
        c->width    = VideoXW;
        c->height   = VideoYW;
        /* timebase: This is the fundamental unit of time (in seconds) in terms
         * of which frame timestamps are represented. For fixed-fps content,
         * timebase should be 1/framerate and timestamp increments should be
         * identical to 1. */
        ost->st->time_base = (AVRational){ 1, STREAM_FRAME_RATE };
        c->time_base       = ost->st->time_base;
        c->gop_size      = 12; /* emit one intra frame every twelve frames at most */
        c->pix_fmt       = STREAM_PIX_FMT;
        if (c->codec_id == AV_CODEC_ID_MPEG2VIDEO) {
            /* just for testing, we also add B-frames */
            c->max_b_frames = 2;
        }
        if (c->codec_id == AV_CODEC_ID_MPEG1VIDEO) {
            /* Needed to avoid using macroblocks in which some coeffs overflow.
             * This does not happen with normal video, it just happens here as
             * the motion of the chroma plane does not match the luma plane. */
            c->mb_decision = 2;
        }
    break;
    default:
        break;
    }
    /* Some formats want stream headers to be separate. */
    if (oc->oformat->flags & AVFMT_GLOBALHEADER)
        c->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;
}

void VideoCaptureClass::CloseStream(AVFormatContext *oc, OutputStream *ost)
{
    avcodec_free_context(&ost->enc);
    av_frame_free(&ost->frame);
    av_frame_free(&ost->tmp_frame);
    sws_freeContext(ost->sws_ctx);
    swr_free(&ost->swr_ctx);
}

bool VideoCaptureClass::OpenVideo(AVFormatContext *oc, AVCodec *codec, OutputStream *ost, AVDictionary *opt_arg)
{
    int ret;
    AVCodecContext *c = ost->enc;
    AVDictionary *opt = NULL;
    av_dict_copy(&opt, opt_arg, 0);
    /* open the codec */
    ret = avcodec_open2(c, codec, &opt);
    av_dict_free(&opt);
    if (ret < 0)
    {
        char err_msg[AV_ERROR_MAX_STRING_SIZE];
        cerr << "Could not open video codec: " << av_make_error_string(err_msg,AV_ERROR_MAX_STRING_SIZE,ret) << endl;
        return false;
    }

    /* allocate and init a re-usable frame */
    ost->frame = AllocPicture(c->pix_fmt, c->width, c->height);
    if (!ost->frame)
    {
        cerr << "Could not allocate video frame." << endl;
        return false;
    }
    /* If the output format is not YUV420P, then a temporary YUV420P
     * picture is needed too. It is then converted to the required
     * output format. */
    ost->tmp_frame = NULL;
    if (c->pix_fmt != AV_PIX_FMT_YUV420P)
    {
        ost->tmp_frame = AllocPicture(AV_PIX_FMT_YUV420P, c->width, c->height);
        if (!ost->tmp_frame)
        {
            cerr << "Could not allocate temporary picture." << endl;
            return false;
        }
    }

    /* copy the stream parameters to the muxer */
    ret = avcodec_parameters_from_context(ost->st->codecpar, c);
    if (ret < 0)
    {
        cerr << "Could not copy the stream parameters." << endl;
        return false;
    }

    return true;
}

AVFrame* VideoCaptureClass::AllocPicture(enum AVPixelFormat pix_fmt, int width, int height)
{
    AVFrame *picture;
    int ret;
    picture = av_frame_alloc();
    if (!picture)
        return NULL;
    picture->format = pix_fmt;
    picture->width  = width;
    picture->height = height;
    /* allocate the buffers for the frame data */
    ret = av_frame_get_buffer(picture, 32);
    if (ret < 0)
    {
        cerr << "Could not allocate frame data." << endl;
        return NULL;
    }
    return picture;
}

bool VideoCaptureClass::OpenAudio(AVFormatContext *oc, AVCodec *codec, OutputStream *ost, AVDictionary *opt_arg)
{
    AVCodecContext *c;
    int nb_samples;
    int ret;
    AVDictionary *opt = NULL;
    c = ost->enc;
    /* open it */
    av_dict_copy(&opt, opt_arg, 0);
    ret = avcodec_open2(c, codec, &opt);
    av_dict_free(&opt);
    if (ret < 0)
    {
        char err_msg[AV_ERROR_MAX_STRING_SIZE];
        cerr << "Could not open audio codec: " << av_make_error_string(err_msg,AV_ERROR_MAX_STRING_SIZE,ret) << endl;
        return false;
    }

    /* init signal generator */
    ost->t     = 0;
    ost->tincr = 2 * M_PI * 110.0 / c->sample_rate;
    /* increment frequency by 110 Hz per second */
    ost->tincr2 = 2 * M_PI * 110.0 / c->sample_rate / c->sample_rate;

    if (c->codec->capabilities & AV_CODEC_CAP_VARIABLE_FRAME_SIZE)
    {
        nb_samples = 10000;
    }
    else
    {
        nb_samples = c->frame_size;
    }

    ost->frame     = AllocAudioFrame(c->sample_fmt, c->channel_layout, c->sample_rate, nb_samples);
    ost->tmp_frame = AllocAudioFrame(AV_SAMPLE_FMT_S16, c->channel_layout, c->sample_rate, nb_samples);

    /* copy the stream parameters to the muxer */
    ret = avcodec_parameters_from_context(ost->st->codecpar, c);
    if (ret < 0)
    {
        cerr << "Could not copy the stream parameters." << endl;
        return false;
    }

    /* create resampler context */
    ost->swr_ctx = swr_alloc();
    if (!ost->swr_ctx)
    {
        cerr << "Could not allocate resampler context" << endl;
        return false;
    }

    /* set options */
    av_opt_set_int       (ost->swr_ctx, "in_channel_count",   c->channels,       0);
    av_opt_set_int       (ost->swr_ctx, "in_sample_rate",     c->sample_rate,    0);
    av_opt_set_sample_fmt(ost->swr_ctx, "in_sample_fmt",      AV_SAMPLE_FMT_S16, 0);
    av_opt_set_int       (ost->swr_ctx, "out_channel_count",  c->channels,       0);
    av_opt_set_int       (ost->swr_ctx, "out_sample_rate",    c->sample_rate,    0);
    av_opt_set_sample_fmt(ost->swr_ctx, "out_sample_fmt",     c->sample_fmt,     0);
    /* initialize the resampling context */
    if ((ret = swr_init(ost->swr_ctx)) < 0)
    {
        cerr << "Failed to initialize the resampling context" << endl;
        return false;
    }

    return true;
}

AVFrame* VideoCaptureClass::AllocAudioFrame(enum AVSampleFormat sample_fmt, uint64_t channel_layout, int sample_rate, int nb_samples)
{
    AVFrame *frame = av_frame_alloc();
    int ret;
    if (!frame)
    {
        cerr << "Error allocating an audio frame." << endl;
        return NULL;
    }

    frame->format = sample_fmt;
    frame->channel_layout = channel_layout;
    frame->sample_rate = sample_rate;
    frame->nb_samples = nb_samples;
    if (nb_samples)
    {
        ret = av_frame_get_buffer(frame, 0);
        if (ret < 0)
        {
            cerr << "Error allocating an audio buffer." << endl;
            return NULL;
        }
    }
    return frame;
}

int VideoCaptureClass::WriteVideoFrame(AVFormatContext *oc, OutputStream *ost)
{
    int ret;
    AVCodecContext *c;
    AVFrame *frame;
    int got_packet = 0;
    AVPacket pkt = { 0 };
    c = ost->enc;
    frame = GetVideoFrame(ost);
    av_init_packet(&pkt);

    /* encode the image */
    ret = avcodec_encode_video2(c, &pkt, frame, &got_packet);
    if (ret < 0)
    {
        char err_msg[AV_ERROR_MAX_STRING_SIZE];
        cerr << "Error encoding video frame: " << av_make_error_string(err_msg,AV_ERROR_MAX_STRING_SIZE,ret) << endl;
    }

    if (got_packet)
    {
        ret = WriteFrame(oc, &c->time_base, ost->st, &pkt);
    } else
        ret = 0;

    if (ret < 0)
    {
        char err_msg[AV_ERROR_MAX_STRING_SIZE];
        cerr << "Error while writing video frame: " << av_make_error_string(err_msg,AV_ERROR_MAX_STRING_SIZE,ret) << endl;
    }

    return (frame || got_packet) ? 0 : 1;
}

int VideoCaptureClass::WriteAudioFrame(AVFormatContext *oc, OutputStream *ost)
{
    AVCodecContext *c;
    AVPacket pkt = { 0 }; // data and size must be 0;
    AVFrame *frame;
    int ret;
    int got_packet;
    int dst_nb_samples;
    av_init_packet(&pkt);
    c = ost->enc;

    frame = GetAudioFrame(ost);

    if(frame == NULL) return 0;

    if (frame)
    {
        /* convert samples from native format to destination codec format, using the resampler */
            /* compute destination number of samples */
            dst_nb_samples = av_rescale_rnd(swr_get_delay(ost->swr_ctx, c->sample_rate) + frame->nb_samples, c->sample_rate, c->sample_rate, AV_ROUND_UP);
            av_assert0(dst_nb_samples == frame->nb_samples);
        /* when we pass a frame to the encoder, it may keep a reference to it
         * internally;
         * make sure we do not overwrite it here
         */
        ret = av_frame_make_writable(ost->frame);
        if (ret < 0)
            exit(1);
        /* convert to destination format */
        ret = swr_convert(ost->swr_ctx, ost->frame->data, dst_nb_samples, (const uint8_t **)frame->data, frame->nb_samples);
        if (ret < 0)
        {
            fprintf(stderr, "Error while converting\n");
            exit(1);
        }
        frame = ost->frame;
        frame->pts = av_rescale_q(ost->samples_count, (AVRational){1, c->sample_rate}, c->time_base);
        ost->samples_count += dst_nb_samples;
    }
    ret = avcodec_encode_audio2(c, &pkt, frame, &got_packet);

    if (ret < 0)
    {
        char err_msg[AV_ERROR_MAX_STRING_SIZE];
        cerr << "Error encoding audio frame: " << av_make_error_string(err_msg,AV_ERROR_MAX_STRING_SIZE,ret) << endl;
    }

    if (got_packet)
    {
        ret = WriteFrame(oc, &c->time_base, ost->st, &pkt);
        if (ret < 0)
        {
            char err_msg[AV_ERROR_MAX_STRING_SIZE];
            cerr << "Error while writing audio frame: " << av_make_error_string(err_msg,AV_ERROR_MAX_STRING_SIZE,ret) << endl;
        }
    }
    return (frame || got_packet) ? 0 : 1;
}

int VideoCaptureClass::WriteFrame(AVFormatContext *fmt_ctx, const AVRational *time_base, AVStream *st, AVPacket *pkt)
{
    /* rescale output packet timestamp values from codec to stream timebase */
    av_packet_rescale_ts(pkt, *time_base, st->time_base);
    pkt->stream_index = st->index;
    /* Write the compressed frame to the media file. */
    LogPacket(fmt_ctx, pkt);
    return av_interleaved_write_frame(fmt_ctx, pkt);
}

void VideoCaptureClass::LogPacket(const AVFormatContext *fmt_ctx, const AVPacket *pkt)
{
    AVRational *time_base = &fmt_ctx->streams[pkt->stream_index]->time_base;

    /*
    printf("pts:%s pts_time:%s dts:%s dts_time:%s duration:%s duration_time:%s stream_index:%d\n",
           av_ts2str(pkt->pts), av_ts2timestr(pkt->pts, time_base),
           av_ts2str(pkt->dts), av_ts2timestr(pkt->dts, time_base),
           av_ts2str(pkt->duration), av_ts2timestr(pkt->duration, time_base),
           pkt->stream_index);
    */
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

AVFrame* VideoCaptureClass::GetVideoFrame(OutputStream *ost)
{
    AVCodecContext *c = ost->enc;
    /* check if we want to generate more frames */

    /*
    if (av_compare_ts(ost->next_pts, c->time_base,
                      STREAM_DURATION, (AVRational){ 1, 1 }) >= 0)
        return NULL;
    */


    /* when we pass a frame to the encoder, it may keep a reference to it
     * internally; make sure we do not overwrite it here */

    if (av_frame_make_writable(ost->frame) < 0)
        exit(1);


    if (c->pix_fmt != AV_PIX_FMT_YUV420P) {
        /* as we only generate a YUV420P picture, we must convert it
         * to the codec pixel format if needed */
        if (!ost->sws_ctx) {
            ost->sws_ctx = sws_getContext(c->width, c->height,
                                          AV_PIX_FMT_YUV420P,
                                          c->width, c->height,
                                          c->pix_fmt,
                                          SCALE_FLAGS, NULL, NULL, NULL);
            if (!ost->sws_ctx) {
                fprintf(stderr,
                        "Could not initialize the conversion context\n");
                exit(1);
            }
        }
        FillyuvImage(ost->tmp_frame, ost->next_pts, c->width, c->height);
        sws_scale(ost->sws_ctx,
                  (const uint8_t * const *)ost->tmp_frame->data, ost->tmp_frame->linesize,
                  0, c->height, ost->frame->data, ost->frame->linesize);
    } else {
        FillyuvImage(ost->frame, ost->next_pts, c->width, c->height);
    }
    ost->frame->pts = ost->next_pts++;
    return ost->frame;
}

void VideoCaptureClass::FillyuvImage(AVFrame *pict, int frame_index, int width, int height)
{
    uint8_t *src_pixels;
    uint8_t Y;
    uint8_t Cb,Cr;
    uint8_t r,g,b;

    // Y
    for (int y = 0; y < height; y++)
    {
        src_pixels = SourceVideoData + y*SourceVideoLineSize;
        for (int x = 0; x < width; x++)
        {
            Y =  0.257*src_pixels[2] + 0.504*src_pixels[1] + 0.098*src_pixels[0] + 16;
            src_pixels += 4;
            pict->data[0][y * pict->linesize[0] + x] = Y;
        }
    }

    // Cb and Cr
    for (int y = 0; y < height; y+=2)
    {
        src_pixels = SourceVideoData + y*SourceVideoLineSize;
        for (int x = 0; x < width/2; x++)
        {
            r = (src_pixels[2]+src_pixels[6]+src_pixels[2+SourceVideoLineSize]+src_pixels[6+SourceVideoLineSize]) / 4;
            g = (src_pixels[1]+src_pixels[5]+src_pixels[1+SourceVideoLineSize]+src_pixels[5+SourceVideoLineSize]) / 4;
            b = (src_pixels[0]+src_pixels[4]+src_pixels[0+SourceVideoLineSize]+src_pixels[4+SourceVideoLineSize]) / 4;

            src_pixels += 8;

            Cb = -0.148*r - 0.291*g + 0.439*b + 128;
            Cr = 0.439*r - 0.368*g - 0.071*b + 128;
            pict->data[1][y/2 * pict->linesize[1] + x] = Cr;
            pict->data[2][y/2 * pict->linesize[2] + x] = Cb;
        }
    }
}

AVFrame* VideoCaptureClass::GetAudioFrame(OutputStream *ost)
{
    AVFrame *frame = ost->tmp_frame;
    int j, i, v;
    int16_t *q = (int16_t*)frame->data[0];

    for (j = 0; j <frame->nb_samples; j++)
    {
        *q++ = FrameAudioDataL[j];
        *q++ = FrameAudioDataR[j];
        ost->t     += ost->tincr;
        ost->tincr += ost->tincr2;
    }

    frame->pts = ost->next_pts;
    ost->next_pts  += frame->nb_samples;

    return frame;
}
