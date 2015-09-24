#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <stdio.h>



static int audioindex = -1;
static int videoindex = -1;
static int isaaccodec = -1;


int main(int argc ,char **argv)
{
    av_register_all();
    FILE *f = NULL;
    FILE *g = NULL;
    f = fopen("audio","wb");
    g = fopen("video","wb");
    if(!f || !g)
    {
        printf("open write file errorn");
        return 0;
    }
    AVFormatContext *fmtctx = NULL;

    AVPacket audiopack;
    if(avformat_open_input(&fmtctx,argv[1],NULL,NULL) < 0)
    {
        printf("open fmtctx errorn");
        return 0;
    }

    if(avformat_find_stream_info(fmtctx,NULL) < 0)
    {
        printf("find stream info n");
        return 0;
    }
    int streamnum = fmtctx->nb_streams;
    printf("stream num is %dn",streamnum);
    int i=0;
    for(;i<streamnum;i++)
    {
        if(fmtctx->streams[i]->codec->codec_type == AVMEDIA_TYPE_AUDIO && audioindex == -1)
        {
            audioindex = i; 
        }
        else if(fmtctx->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO && videoindex == -1)
        {
            videoindex = i;
        }
    }
    printf("audioindex is %dn",audioindex);

    printf("videoindex is %dn",videoindex);

    AVCodecContext *codecctx = fmtctx->streams[videoindex]->codec;
    AVCodec *decode = avcodec_find_decoder(codecctx->codec_id);

    AVCodecContext *audioCodecCtx = fmtctx->streams[audioindex]->codec;
    AVCodec *audiodecode = avcodec_find_decoder(audioCodecCtx->codec_id);
    if(audiodecode->id == AV_CODEC_ID_AAC)
    {
        isaaccodec = 1;
    }

    if(avcodec_open2(codecctx,decode,NULL) < 0)
    {
        return -1;
    }
    if(avcodec_open2(audioCodecCtx,audiodecode,NULL) < 0)
    {
        return -1;
    }
    printf("extradata size is %dn",audioCodecCtx->extradata_size);

    AVBitStreamFilterContext* bsfc = av_bitstream_filter_init("h264_mp4toannexb");
    AVBitStreamFilterContext* aacbsfc = av_bitstream_filter_init("aac_adtstoasc");
    if(!bsfc || !aacbsfc)
    {
        return 0;
    }
    AVFrame picture;
    while(!(av_read_frame(fmtctx,&audiopack)))
    {
        if(audiopack.stream_index == 1)
        {
            if(isaaccodec == 1)
            {
                char bits[7] = {0};
                int sample_index = 0 , channel = 0;
                char temp = 0;
                int length = 7 + audiopack.size;
                sample_index = (audioCodecCtx->extradata[0] & 0x07) << 1;
                temp = (audioCodecCtx->extradata[1]&0x80);
                switch(audioCodecCtx->sample_rate)
                {
                    case 44100:
                        {
                            sample_index = 0x7;
                        }break;
                    default:
                        {
                            sample_index = sample_index + (temp>>7);
                        }break;
                }
                channel = ((audioCodecCtx->extradata[1] - temp) & 0xff) >> 3;
                bits[0] = 0xff;
                bits[1] = 0xf1;
                bits[2] = 0x40 | (sample_index<<2) | (channel>>2);
                bits[3] = ((channel&0x3)<<6) | (length >>11);
                bits[4] = (length>>3) & 0xff;
                bits[5] = ((length<<5) & 0xff) | 0x1f;
                bits[6] = 0xfc;

                fwrite(bits,1,7,f);
            }
            fwrite(audiopack.data,1,audiopack.size,f);
            printf("audio pts is %fn",audiopack.pts*av_q2d(fmtctx->streams[audioindex]->time_base));
        }
        else if(audiopack.stream_index == videoindex){
            AVPacket pkt = audiopack;
            
            int a = av_bitstream_filter_filter(bsfc, codecctx, NULL, &pkt.data, &pkt.size, audiopack.data, audiopack.size, audiopack.flags & AV_PKT_FLAG_KEY);
            if(a > 0)
            {
                audiopack = pkt;
            }
            fwrite(audiopack.data,1,audiopack.size,g);
            int gotfinished = 0;
            if(avcodec_decode_video2(codecctx,&picture,&gotfinished,&audiopack) < 0)
            {
                printf("decode video errorn");
            }
            if(picture.key_frame)
            {
                //printf("key framen");
            }
            else{
                //printf("frame num is %dn",picture.pict_type);
            }

            //printf("video pts is %f, %dn",picture.pkt_dts * av_q2d(fmtctx->streams[videoindex]->codec->time_base),picture.pts );
        }
        av_free_packet(&audiopack);
    }
    fclose(f);
}