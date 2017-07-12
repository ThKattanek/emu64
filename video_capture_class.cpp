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
// Letzte Änderung am 12.07.2017                //
// www.emu64.de                                 //
//                                              //
//////////////////////////////////////////////////

#include "video_capture_class.h"

VideoCaptureClass::VideoCaptureClass()
{
    EndCode[0] = 0;
    EndCode[1] = 0;
    EndCode[2] = 1;
    EndCode[3] = 0xb7 ;

    CaptureIsActive = false;
    VideoCodec = NULL;

    // Alle Formate registrieren
    av_register_all();

    cout << "FFMPEG Version: " << GetAVVersion() << endl;
}

VideoCaptureClass::~VideoCaptureClass()
{
    if(CaptureIsActive)
    {
        cout << "VideoCaptureClass: " << "Stoppen erzwingen!" << endl;
        StopCapture();
    }
}

const char* VideoCaptureClass::GetAVVersion()
{
    return av_version_info();
}

bool VideoCaptureClass::StartCapture(const char *filename, const char *codec_name, int xw, int yw)
{
    /// TEST ///
    /*
    AVOutputFormat *oformat, *onextformat;

    // VideoCodecs
    oformat = onextformat = NULL;
    do
    {
        oformat = av_oformat_next(onextformat);
        if(oformat != NULL)
        {
            cout << "FormatName: " << oformat->name << endl;
            //cout << "Extensions: " << oformat->extensions << endl;
        }
        onextformat = oformat;
    }
    while(oformat != NULL);
    */

    VideoXW = xw;
    VideoYW = yw;

    // Prüfen ob der VideoCodec vorhanden ist
    VideoCodec = avcodec_find_encoder_by_name(codec_name);
    if (!VideoCodec)
    {
        cerr << "Codec " << codec_name << " nicht gefunden." << endl;
        return false;
    }

    // Video Codec Context zuordnen
    VideoCodecContext = avcodec_alloc_context3(VideoCodec);
    if (!VideoCodecContext)
    {
        cerr << "Video Codec Context konnet nicht reserviert werden." << endl;
        return false;
    }

    // Video Packet reservieren
    VideoPacket = av_packet_alloc();
    if (!VideoPacket)
    {
        cerr << "Video Packet konnte nicht reserviert werden." << endl;
        return false;
    }

    // Video Codec configurieren
    VideoCodecContext->bit_rate = 40000000;                 // BitRate
    VideoCodecContext->width = VideoXW;                     // Breite
    VideoCodecContext->height = VideoYW;                    // Höhe
    VideoCodecContext->time_base = AVRational{1, 50};       // Frametime
    VideoCodecContext->framerate = AVRational{50, 1};       // Framerate
    VideoCodecContext->gop_size = 10;
    VideoCodecContext->max_b_frames = 1;
    VideoCodecContext->pix_fmt = AV_PIX_FMT_YUV422P;        // YUV420P, YUV422P

    if (VideoCodec->id == AV_CODEC_ID_H264)
        av_opt_set(VideoCodecContext->priv_data, "preset", "slow", 0);

    // Video Codec öffnen
    int ret = avcodec_open2(VideoCodecContext, VideoCodec, NULL);
    if (ret < 0)
    {
        cerr << "Video Codec konnte nicht geöffnet werden: " << av_make_error_string(NULL,0,ret) << endl;
        return false;
    }

    // Video Stream erstellen / öffnen
    VideoOutFile = fopen(filename, "wb");
    if (!VideoOutFile)
    {
        cerr << "Video Datei " << filename << " kann nicht erstellt/geöffnet werden." << endl;
        return false;
    }

    // Video Frame reservieren
    VideoFrame = av_frame_alloc();
    if (!VideoFrame)
    {
        cerr << "Video Frame konnte nicht reserviert werden." << endl;
        return false;
    }

    // Frame Configurieren
    VideoFrame->format = VideoCodecContext->pix_fmt;
    VideoFrame->width  = VideoCodecContext->width;
    VideoFrame->height = VideoCodecContext->height;

    // Speicher für FrameDaten reservieren
    ret = av_frame_get_buffer(VideoFrame, 32);
    if (ret < 0)
    {
        cerr << "Es konnte keine Video Frame Daten reserviert werden." << endl;
        return false;
    }

    CaptureIsActive = true;
    cout << "VideoCapture wird gestartet" << endl;
    cout << "Width=" << VideoXW << endl;
    cout << "Heigh=" << VideoYW << endl;
    return true;
}

void VideoCaptureClass::StopCapture()
{
    cout << "VideoCapture wird gestoppt" << endl;

    /* flush the encoder */
    VideoEncode(VideoCodecContext, NULL, VideoPacket, VideoOutFile);

    /* add sequence end code to have a real MPEG file */
    fwrite(EndCode, 1, sizeof(EndCode), VideoOutFile);
    fclose(VideoOutFile);

    // Video Codec Context wieder freigeben
    avcodec_free_context(&VideoCodecContext);
    av_frame_free(&VideoFrame);
    av_packet_free(&VideoPacket);
}

void VideoCaptureClass::WriteRGBAFrame(uint8_t *data, int linesize)
{
    if(!CaptureIsActive) return;

    int ret;
    static int i=0;

    /* encode 1 second of video */

         fflush(stdout);

         /* make sure the frame data is writable */
         ret = av_frame_make_writable(VideoFrame);
         if (ret < 0)
             StopCapture();


         /* prepare a dummy image */
         /* Y */
         /*
         for (int y = 0; y < VideoCodecContext->height; y++) {
             for (int x = 0; x < VideoCodecContext->width; x++) {
                 VideoFrame->data[0][y * VideoFrame->linesize[0] + x] = x + y + i * 3;
             }
         }

         /* Cb and Cr */
         /*
         for (int y = 0; y < VideoCodecContext->height/2; y++) {
             for (int x = 0; x < VideoCodecContext->width/2; x++) {
                 VideoFrame->data[1][y * VideoFrame->linesize[1] + x] = 128 + y + i * 2;
                 VideoFrame->data[2][y * VideoFrame->linesize[2] + x] = 64 + x + i * 5;
             }
         }
            */

         // YUV420
         /* Y */

         /*
         Y' = 0.257*R' + 0.504*G' + 0.098*B' + 16
         Cb' = -0.148*R' - 0.291*G' + 0.439*B' + 128
         Cr' = 0.439*R' - 0.368*G' - 0.071*B' + 128
         */

         uint8_t *src_pixels;
         uint8_t Y;
         uint8_t Cb,Cr;
         uint8_t r,g,b;

         for (int y = 0; y < VideoCodecContext->height; y++)
         {
             src_pixels = data + y*linesize;
             for (int x = 0; x < VideoCodecContext->width; x++)
             {
                 Y =  0.257*src_pixels[2] + 0.504*src_pixels[1] + 0.098*src_pixels[0] + 16;
                 src_pixels += 4;
                 VideoFrame->data[0][y * VideoFrame->linesize[0] + x] = Y;
             }
         }

         /* Cb and Cr */
         switch(VideoCodecContext->pix_fmt)
         {
         case AV_PIX_FMT_YUV420P:
             for (int y = 0; y < VideoCodecContext->height; y+=2)
             {
                 src_pixels = data + y*linesize;
                 for (int x = 0; x < VideoCodecContext->width/2; x++)
                 {
                     r = (src_pixels[2]+src_pixels[6]+src_pixels[2+linesize]+src_pixels[6+linesize]) / 4;
                     g = (src_pixels[1]+src_pixels[5]+src_pixels[1+linesize]+src_pixels[5+linesize]) / 4;
                     b = (src_pixels[0]+src_pixels[4]+src_pixels[0+linesize]+src_pixels[4+linesize]) / 4;

                     src_pixels += 8;

                     Cb = -0.148*r - 0.291*g + 0.439*b + 128;
                     Cr = 0.439*r - 0.368*g - 0.071*b + 128;
                     VideoFrame->data[1][y/2 * VideoFrame->linesize[1] + x] = Cr;
                     VideoFrame->data[2][y/2 * VideoFrame->linesize[2] + x] = Cb;
                 }
             }
             break;
         case AV_PIX_FMT_YUV422P:
             for (int y = 0; y < VideoCodecContext->height; y++)
             {
                 src_pixels = data + y*linesize;
                 for (int x = 0; x < VideoCodecContext->width/2; x++)
                 {
                     r = (src_pixels[2]+src_pixels[6]) / 2;
                     g = (src_pixels[1]+src_pixels[5]) / 2;
                     b = (src_pixels[0]+src_pixels[4]) / 2;

                     src_pixels += 8;

                     Cb = -0.148*r - 0.291*g + 0.439*b + 128;
                     Cr = 0.439*r - 0.368*g - 0.071*b + 128;
                     VideoFrame->data[1][y * VideoFrame->linesize[1] + x] = Cr;
                     VideoFrame->data[2][y * VideoFrame->linesize[2] + x] = Cb;
                 }
             }
             break;
         default:
             break;
         }

         VideoFrame->pts = i;

         /* encode the image */
         VideoEncode(VideoCodecContext, VideoFrame, VideoPacket, VideoOutFile);

         i++;


    ////////////////////////////////////////////////////////////////////////////
}

void VideoCaptureClass::VideoEncode(AVCodecContext *enc_ctx, AVFrame *frame, AVPacket *pkt, FILE *outfile)
{
    int ret = 0;

    ret = avcodec_send_frame(enc_ctx, frame);
    if (ret < 0)
    {
        fprintf(stderr, "Error sending a frame for encoding\n");
        StopCapture();
    }

    while (ret >= 0)
    {
        ret = avcodec_receive_packet(enc_ctx, pkt);
        if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
            return;
        else if (ret < 0)
        {
            fprintf(stderr, "Error during encoding\n");
            StopCapture();
        }

        fwrite(pkt->data, 1, pkt->size, outfile);
        av_packet_unref(pkt);
    }
}
