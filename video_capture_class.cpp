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
// Letzte Änderung am 18.05.2014                //
// www.emu64.de                                 //
//                                              //
//////////////////////////////////////////////////

#include "video_capture_class.h"

unsigned char RGB_COLORS[16*4]={
30,30,30,0,254,254,254,0,157,96,96,0,179,243,239,0,171,113,170,0,145,197,137,0,90,92,161,0,250,254,189,0,162,119,85,0,
104,87,27,0,221,158,159,0,110,108,108,0,166,165,165,0,221,246,204,0,170,170,255,0,224,224,230,0};

VideoCaptureClass::VideoCaptureClass()
{
    CaptureIsRunning = false;
    codec_context = NULL;
    outbuf = NULL;
    AddFrameIsActive = false;

    for(int i=0; i<16; i++)
    {
        unsigned char R = RGB_COLORS[i*4+0];
        unsigned char G = RGB_COLORS[i*4+1];
        unsigned char B = RGB_COLORS[i*4+2];

        unsigned char Y = 0.299*R+0.587*G+0.114*B;  // Y
        unsigned char U = -0.147*R-0.289*G+0.436*B+128; // U
        unsigned char V = 0.615*R-0.515*G-0.100*B+128;  // V

        YUV_COLORS[i*4+0] = Y;
        YUV_COLORS[i*4+1] = U;
        YUV_COLORS[i*4+2] = V;
    }

    avcodec_register_all();
}

VideoCaptureClass::~VideoCaptureClass()
{
    // Wenn eine Aufzeichnung noch läuft dann erst beenden
    if(CaptureIsRunning)
    {
        Stop();
    }
}

int VideoCaptureClass::Start(char *filename, int video_bit_rate, int xw, int yw, int fps)
{
    if(CaptureIsRunning) return -1;     // Es läuft gerade eine Aufzeichnung
    if(filename == NULL) return -2;     // Es wurde kein Dateiname übergeben
    if(xw & 1 || yw & 1) return -3;     // Auflösung muss ein vielfaches von 2 sein

    cout << "CapureFilename: " << filename;

    // MPPEG1 Videoencoder finden
    codec = avcodec_find_encoder(CODEC_ID_MPEG1VIDEO);
    if (!codec) return -4;              // Es wurde kein MPEG1 Videoencoder gefunden

    vic = new unsigned char[xw * yw];

    codec_context = avcodec_alloc_context3(codec);
    picture = avcodec_alloc_frame();

    codec_context->bit_rate = video_bit_rate;
    codec_context->width = xw;
    codec_context->height = yw;
    codec_context->time_base= (AVRational){1,fps};
    codec_context->gop_size = 10; /* emit one intra frame every ten frames */
    codec_context->max_b_frames=1;

    codec_context->pix_fmt = PIX_FMT_YUV420P;

    //if(codec_id == CODEC_ID_H264) av_opt_set(c->priv_data, "preset", "slow", 0);

    //Codec öffnen
    if (avcodec_open2(codec_context, codec, NULL) < 0)
    {
        av_free(codec_context);
        av_free(picture->data[0]);
        av_free(picture);
        return -5;                      // Der Codec konnte nicht geöffnet werden
    }

    /* alloc image and output buffer */
    outbuf_size = 100000;
    outbuf = new unsigned char(outbuf_size);

    // Picture Daten reservieren
    av_image_alloc(picture->data, picture->linesize,codec_context->width, codec_context->height, codec_context->pix_fmt, 1);

    //MPEG Datei erstellen
    file = fopen(filename, "wb");
    if (!file)
    {
        if(outbuf != NULL) delete outbuf;
        av_free(codec_context);
        av_free(picture->data[0]);
        av_free(picture);
        return -6;                      // Die Datei konnte nicht geöffnet werden
    }

    CaptureIsRunning = true;
    return 0;
}

void VideoCaptureClass::Stop()
{
    if(!CaptureIsRunning) return;
    CaptureIsRunning = false;

    while(AddFrameIsActive){};          // Solange warten falls gerade ein Frame hinzugefuegt wird

    /* get the delayed frames */
    for(;out_size;)
    {
        fflush(stdout);

        out_size = avcodec_encode_video(codec_context, outbuf, outbuf_size, NULL);
        fwrite(outbuf, 1, out_size, file);
    }

    /* add sequence end code to have a real mpeg file */
    outbuf[0] = 0x00;
    outbuf[1] = 0x00;
    outbuf[2] = 0x01;
    outbuf[3] = 0xb7;
    fwrite(outbuf, 1, 4, file);

    fclose(file);
    if(outbuf != NULL) delete outbuf;
    avcodec_close(codec_context);
    av_free(codec_context);
    av_free(picture->data[0]);
    av_free(picture);

    delete vic;
}

void VideoCaptureClass::AddFrame(void *puffer, int xw, int yw)
{
    if(!CaptureIsRunning) return;

    AddFrameIsActive = true;
    unsigned char *vic_src = (unsigned char*)puffer;

    for(int i=0; i<(xw * yw);i++)
    {
        vic[i] = vic_src[i];
    }

    /*
    fflush(stdout);
      // prepare a dummy image //
         int i=0;
      // Y //
      for(int y=0;y<codec_context->height;y++) {
          for(int x=0;x<codec_context->width;x++) {
              picture->data[0][y * picture->linesize[0] + x] = x + y + i * 3;
              i++;
          }
      }

      // Cb and Cr //
      for(int y=0;y<codec_context->height/2;y++) {
          for(int x=0;x<codec_context->width/2;x++) {
              picture->data[1][y * picture->linesize[1] + x] = 128 + y + i * 2;
              picture->data[2][y * picture->linesize[2] + x] = 64 + x + i * 5;
              i++;
          }
      }
    */

    fflush(stdout);
    int i=0;
    for(int y=0;y<codec_context->height;y++)
    {
        for(int x=0;x<codec_context->width;x++)
        {
            picture->data[0][y * picture->linesize[0] + x] = YUV_COLORS[(vic[y*xw+x]&0xf)*4+0];
            i++;
        }
    }

    for(int y=0;y<codec_context->height;y++)
    {
        for(int x=0;x<codec_context->width;x++)
        {   
           picture->data[1][y * picture->linesize[1] + x] = YUV_COLORS[(vic[y*xw+x]&0xf)*4+1];
           picture->data[2][y * picture->linesize[2] + x] = YUV_COLORS[(vic[y*xw+x]&0xf)*4+2];
        }
    }

  /* encode the image */
  out_size = avcodec_encode_video(codec_context, outbuf, outbuf_size, picture);
  fwrite(outbuf, 1, out_size, file);

      AddFrameIsActive = false;
}
