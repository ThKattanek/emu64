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
// Letzte Änderung am 17.09.2019                //
// www.emu64.de                                 //
//                                              //
//////////////////////////////////////////////////

#include "video_capture_class.h"
#include "video_capture_class.h"

int SDLThreadAddCaptureFrame(void *userdat);

VideoCaptureClass::VideoCaptureClass()
{
    mutex_01 = false;

    is_capture_cctive = false;
    is_capture_pause = false;

    format_ctx = nullptr;
    video_codec = audio_codec = nullptr;
    video_stream = {};
    audio_stream = {};

    source_audio_data = new unsigned short[SOURCE_SAMPLE_BUFFER_LEN];

    audio_bitrate = 128000;
    video_bitrate = 4000000;

    video_package_counter = 0;
    audio_package_counter = 0;
}

VideoCaptureClass::~VideoCaptureClass()
{
    if(is_capture_cctive)
    {
        cout << "VideoCaptureClass: " << "Stoppen erzwingen!" << endl;
        StopCapture();

        delete[] source_audio_data;
    }
}

const char* VideoCaptureClass::GetAVVersion()
{
    return av_version_info();
}

void VideoCaptureClass::SetVideoBitrate(int video_bitrate)
{
    this->video_bitrate = video_bitrate;
}

void VideoCaptureClass::SetAudioBitrate(int audio_bitrate)
{
    this->audio_bitrate = audio_bitrate;
}

bool VideoCaptureClass::StartCapture(const char *filename, const char *codec_name, int xw, int yw)
{
    if(is_capture_cctive) return false;

    while(mutex_01){
        SDL_Delay(1);
    }   // Warten bis Mutex1 Unlocked (false)
    mutex_01 = true;      // Mutex1 Locken (true)

    video_xw = xw;
    video_yw = yw;

    have_video = have_audio = false;
    encode_video = encode_audio = 0;

    frame_samples_pt = 0;

    audio_package_counter = 0;
    video_package_counter = 0;

    options = nullptr;

    int ret;

    av_register_all();

    //
    avformat_alloc_output_context2(&format_ctx, nullptr, nullptr, filename);
    if (!format_ctx)
    {
        cerr << "Es konnte das Ausgabe Format nicht anhand des Dateinamens ermittelt werden. Es wird versucht MPEG zu benutzten." << endl;
        avformat_alloc_output_context2(&format_ctx, nullptr, codec_name, filename);
    }
    if (!format_ctx)
    {
        cerr << "Es konnte kein FormatContext erstellt werden." << endl;
        mutex_01 = false;      // Mutex1 Unlocken (false)
        return false;
    }

    output_format = format_ctx->oformat;

    // Hinzufügen des Audio und Videostreams mit den Default Format Codecs
    // Und der Initialisierung der Codecs
    if (output_format->video_codec != AV_CODEC_ID_NONE)
    {
        AddStream(&video_stream, format_ctx, &video_codec, output_format->video_codec);
        have_video = true;
        encode_video = 1;
    }
    if (output_format->audio_codec != AV_CODEC_ID_NONE)
    {
        AddStream(&audio_stream, format_ctx, &audio_codec, output_format->audio_codec);
        have_audio = true;
        encode_audio = 1;
    }


    /* Now that all the parameters are set, we can open the audio and
     * video codecs and allocate the necessary encode buffers. */
    if (have_video)
        OpenVideo(video_codec, &video_stream, options);
    if (have_audio)
        OpenAudio(audio_codec, &audio_stream, options);

    av_dump_format(format_ctx, 0, filename, 1);

    // Öffnen der Ausgabedatei
    if (!(output_format->flags & AVFMT_NOFILE))
    {
        ret = avio_open(&format_ctx->pb, filename, AVIO_FLAG_WRITE);
        if (ret < 0)
        {
            char err_msg[AV_ERROR_MAX_STRING_SIZE];
            cerr << "Ausgabedatei kann nicht geöffnet werden: [" << filename << "[  -- " << av_make_error_string(err_msg,AV_ERROR_MAX_STRING_SIZE,ret) << endl;
            mutex_01 = false;      // Mutex1 Unlocken (false)
            StopCapture();
            return false;
        }
    }

    // Schreiben des Stream Headers
    ret = avformat_write_header(format_ctx, &options);
    if (ret < 0)
    {
        char err_msg[AV_ERROR_MAX_STRING_SIZE];
        cerr << "Beim öffnen der Ausgabedatei ist ein Fehler aufgetreten.  -- " << av_make_error_string(err_msg,AV_ERROR_MAX_STRING_SIZE,ret) << endl;
        mutex_01 = false;      // Mutex1 Unlocken (false)
        StopCapture();
        return false;
    }

    // Ausgabe von aktuellen Aufnahme Parametern
    cout << "Video Auflösung: " << video_xw << " x " << video_yw << endl;
    cout << "VideoCodec: " << video_codec->name << endl;
    cout << "AudioCodec: " << audio_codec->name << endl;
    cout << "AudioFrameSize: " << audio_stream.enc->frame_size << endl;

    frame_audio_data_left = new int16_t[audio_stream.enc->frame_size];
    frame_audio_data_right = new int16_t[audio_stream.enc->frame_size];

    is_capture_cctive = true;

    mutex_01 = false;      // Mutex1 Unlocken (false)
    return true;
}

void VideoCaptureClass::StopCapture()
{
    if(!is_capture_cctive) return;

    cout << "VideoCapture wird gestoppt" << endl;

    is_capture_cctive = false;

    while(mutex_01){
        SDL_Delay(1);
    }   // Warten bis Mutex1 Unlocked (false)
    mutex_01 = true;      // Mutex1 Locken (true)


    // Trailer schreiben
    av_write_trailer(format_ctx);

    // VideoStream schließen
    if (have_video)
    {
        CloseStream(&video_stream);
        video_stream = {};
    }

    // AudioStream schließen
    if (have_audio)
    {
        CloseStream(&audio_stream);
        audio_stream = {};
    }

    // Ausgabedatei schließen
    if (!(output_format->flags & AVFMT_NOFILE))
        /* Close the output file. */
        avio_closep(&format_ctx->pb);

    avformat_free_context(format_ctx);
    format_ctx = nullptr;

    delete[] frame_audio_data_left;
    delete[] frame_audio_data_right;

    mutex_01 = false;      // Mutex1 Unlocken (false)

    cout << "VideoCapture wurde gestoppt" << endl;
    cout << "VideoFrames: " << video_package_counter << endl;
    cout << "AudioPackages: " << audio_package_counter << endl;
}

void VideoCaptureClass::SetCapturePause(bool cpt_pause)
{
    is_capture_pause = cpt_pause;
}

void VideoCaptureClass::AddFrame(uint8_t *data, int linesize)
{
    if(!is_capture_cctive || is_capture_pause) return;

    source_video_data = data;
    source_video_line_size = linesize;

    thread_01 = SDL_CreateThread(SDLThreadAddCaptureFrame ,"Emu64CaptureThread",this);
    SDL_DetachThread(thread_01);
}

int SDLThreadAddCaptureFrame(void *userdat){
    VideoCaptureClass *cc = static_cast<VideoCaptureClass*>(userdat);

    while(cc->mutex_01){
    }

    cc->mutex_01 = true;      // Mutex1 Locken (true)
    cc->encode_video = !cc->WriteVideoFrame(cc->format_ctx, &cc->video_stream);
    cc->mutex_01 = false;      // Mutex1 Unlocken (false)
}

void VideoCaptureClass::FillSourceAudioBuffer(int16_t *data, int len)
{
    if(!is_capture_cctive || is_capture_pause) return;

    if(len > SOURCE_SAMPLE_BUFFER_LEN)
    {
        cerr << "Error: Sampledaten größer als Buffer !!" << endl;
        return;
    }

    int n_sample = audio_stream.enc->frame_size;

    for(int i=0; i<len/2; i++)
    {
        frame_audio_data_left[frame_samples_pt] = *data++;
        frame_audio_data_right[frame_samples_pt++] = *data++;
        if(frame_samples_pt == n_sample)
        {
            frame_samples_pt = 0;
            encode_audio = !WriteAudioFrame(format_ctx, &audio_stream);
        }
    }
}

int VideoCaptureClass::GetRecordedFrameCount()
{
    return video_package_counter;
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


    ost->st = avformat_new_stream(oc, nullptr);
    if (!ost->st) {
        fprintf(stderr, "Could not allocate stream\n");
        exit(1);
    }

    ost->st->id = static_cast<int>(oc->nb_streams - 1);
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
        c->bit_rate    = audio_bitrate;
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
        ost->st->time_base = AVRational{ 1, c->sample_rate };
        break;

    //////////////////////////////////////////////////////////////////////////////////////////////////////////

    case AVMEDIA_TYPE_VIDEO:
        c->codec_id = codec_id;
        c->bit_rate = video_bitrate;
        /* Resolution must be a multiple of two. */
        c->width    = video_xw;
        c->height   = video_yw;
        /* timebase: This is the fundamental unit of time (in seconds) in terms
         * of which frame timestamps are represented. For fixed-fps content,
         * timebase should be 1/framerate and timestamp increments should be
         * identical to 1. */
        ost->st->time_base = AVRational{ 1, STREAM_FRAME_RATE };
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

void VideoCaptureClass::CloseStream(OutputStream *ost)
{
    avcodec_free_context(&ost->enc);
    av_frame_free(&ost->frame);
    av_frame_free(&ost->tmp_frame);
    sws_freeContext(ost->sws_ctx);
    swr_free(&ost->swr_ctx);
}

bool VideoCaptureClass::OpenVideo(AVCodec *codec, OutputStream *ost, AVDictionary *opt_arg)
{
    int ret;
    AVCodecContext *c = ost->enc;
    AVDictionary *opt = nullptr;
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
    ost->tmp_frame = nullptr;
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
        return nullptr;
    picture->format = pix_fmt;
    picture->width  = width;
    picture->height = height;
    /* allocate the buffers for the frame data */
    ret = av_frame_get_buffer(picture, 32);
    if (ret < 0)
    {
        cerr << "Could not allocate frame data." << endl;
        return nullptr;
    }
    return picture;
}

bool VideoCaptureClass::OpenAudio(AVCodec *codec, OutputStream *ost, AVDictionary *opt_arg)
{
    AVCodecContext *c;
    int nb_samples;
    int ret;
    AVDictionary *opt = nullptr;
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
        return nullptr;
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
            return nullptr;
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
    AVPacket pkt = {};
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
    else
        video_package_counter++;

    return (frame || got_packet) ? 0 : 1;
}

int VideoCaptureClass::WriteAudioFrame(AVFormatContext *oc, OutputStream *ost)
{
    AVCodecContext *c;
    AVPacket pkt = {}; // data and size must be 0;
    AVFrame *frame;
    int64_t ret;
    int got_packet;
    int64_t dst_nb_samples;
    av_init_packet(&pkt);
    c = ost->enc;

    frame = GetAudioFrame(ost);

    if(frame == nullptr) return 0;

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
        ret = swr_convert(ost->swr_ctx, ost->frame->data, static_cast<int>(dst_nb_samples), const_cast<const uint8_t**>(frame->data), frame->nb_samples);
        if (ret < 0)
        {
            fprintf(stderr, "Error while converting\n");
            exit(1);
        }
        frame = ost->frame;
        frame->pts = av_rescale_q(ost->samples_count, AVRational{1, c->sample_rate}, c->time_base);
        ost->samples_count += dst_nb_samples;
    }
    ret = avcodec_encode_audio2(c, &pkt, frame, &got_packet);

    if (ret < 0)
    {
        char err_msg[AV_ERROR_MAX_STRING_SIZE];
        cerr << "Error encoding audio frame: " << av_make_error_string(err_msg, AV_ERROR_MAX_STRING_SIZE, static_cast<int>(ret)) << endl;
    }

    if (got_packet)
    {
        ret = WriteFrame(oc, &c->time_base, ost->st, &pkt);
        if (ret < 0)
        {
            char err_msg[AV_ERROR_MAX_STRING_SIZE];
            cerr << "Error while writing audio frame: " << av_make_error_string(err_msg, AV_ERROR_MAX_STRING_SIZE, static_cast<int>(ret)) << endl;
        }
        else
        {
            audio_package_counter++;
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
    // LogPacket(fmt_ctx, pkt);
    return av_interleaved_write_frame(fmt_ctx, pkt);
}

/*
void VideoCaptureClass::LogPacket(const AVFormatContext *fmt_ctx, const AVPacket *pkt)
{
    // AVRational *time_base = &fmt_ctx->streams[pkt->stream_index]->time_base;

    printf("pts:%s pts_time:%s dts:%s dts_time:%s duration:%s duration_time:%s stream_index:%d\n",
           av_ts2str(pkt->pts), av_ts2timestr(pkt->pts, time_base),
           av_ts2str(pkt->dts), av_ts2timestr(pkt->dts, time_base),
           av_ts2str(pkt->duration), av_ts2timestr(pkt->duration, time_base),
           pkt->stream_index);
}
*/

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
                                          SCALE_FLAGS, nullptr, nullptr, nullptr);
            if (!ost->sws_ctx) {
                fprintf(stderr,
                        "Could not initialize the conversion context\n");
                exit(1);
            }
        }
        FillyuvImage(ost->tmp_frame, c->width, c->height);
        sws_scale(ost->sws_ctx,
                  const_cast<const uint8_t * const *>(ost->tmp_frame->data), ost->tmp_frame->linesize,
                  0, c->height, ost->frame->data, ost->frame->linesize);
    } else {
        FillyuvImage(ost->frame, c->width, c->height);
    }
    ost->frame->pts = ost->next_pts++;
    return ost->frame;
}

void VideoCaptureClass::FillyuvImage(AVFrame *pict, int width, int height)
{
    uint8_t *src_pixels;
    uint8_t Y;
    uint8_t Cb,Cr;
    uint8_t r,g,b;

    // Y
    for (int y = 0; y < height; y++)
    {
        src_pixels = source_video_data + y*source_video_line_size;
        for (int x = 0; x < width; x++)
        {
            Y =  static_cast<uint8_t>(0.257*src_pixels[2] + 0.504*src_pixels[1] + 0.098*src_pixels[0] + 16);
            src_pixels += 4;
            pict->data[0][y * pict->linesize[0] + x] = Y;
        }
    }

    // Cb and Cr
    for (int y = 0; y < height; y+=2)
    {
        src_pixels = source_video_data + y*source_video_line_size;
        for (int x = 0; x < width/2; x++)
        {
            r = (src_pixels[2]+src_pixels[6]+src_pixels[2+source_video_line_size]+src_pixels[6+source_video_line_size]) / 4;
            g = (src_pixels[1]+src_pixels[5]+src_pixels[1+source_video_line_size]+src_pixels[5+source_video_line_size]) / 4;
            b = (src_pixels[0]+src_pixels[4]+src_pixels[0+source_video_line_size]+src_pixels[4+source_video_line_size]) / 4;

            src_pixels += 8;

            Cb = static_cast<uint8_t>(-0.148*r - 0.291*g + 0.439*b + 128);
            Cr = static_cast<uint8_t>(0.439*r - 0.368*g - 0.071*b + 128);
            pict->data[1][y/2 * pict->linesize[1] + x] = Cr;
            pict->data[2][y/2 * pict->linesize[2] + x] = Cb;
        }
    }
}

AVFrame* VideoCaptureClass::GetAudioFrame(OutputStream *ost)
{
    AVFrame *frame = ost->tmp_frame;

    int16_t *q = reinterpret_cast<int16_t*>(frame->data[0]);

    for (int i = 0; i <frame->nb_samples; i++)
    {
        *q++ = frame_audio_data_left[i];
        *q++ = frame_audio_data_right[i];
        ost->t     += ost->tincr;
        ost->tincr += ost->tincr2;
    }

    frame->pts = ost->next_pts;
    ost->next_pts  += frame->nb_samples;

    return frame;
}
