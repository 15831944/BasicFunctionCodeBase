#include <stdio.h>

//Linux...
#ifdef __cplusplus
extern "C" {
#endif

#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>

#include "SDL/SDL.h"

#include "log/LogC.h"
#include "LogPlayer.h"
#include "Player.h"

#include "OutPutAVInfo.h"

#ifdef __cplusplus
}
#endif






int output_AVFormatContext(AVFormatContext *pAVFormatCtx)
{
    printf("Now output AVFormatContext Structure:\n");
    printf("================================start===========================\n");

    printf(" av_class [%p]\n", pAVFormatCtx->av_class );
    printf(" iformat [%p]\n", pAVFormatCtx->iformat );
    printf(" pb [%p]\n", pAVFormatCtx->pb );
    printf(" nb_streams [%u]\n", pAVFormatCtx->nb_streams );

    printf(" filename [%s]\n", pAVFormatCtx->filename  );
    printf(" timestamp [%lld]\n", pAVFormatCtx->timestamp );
    printf(" ctx_flags [%d]\n", pAVFormatCtx->ctx_flags );
    printf(" packet_buffer [%p]\n", pAVFormatCtx->packet_buffer );


    printf(" start_time [%lld]\n", pAVFormatCtx->start_time );
    printf(" duration [%lld]\n", pAVFormatCtx->duration  );
    printf(" file_size [%lld]\n", pAVFormatCtx->file_size );
    printf(" bit_rate [%d]\n", pAVFormatCtx->bit_rate  );
    printf(" cur_st [%p]\n", pAVFormatCtx->cur_st  );
    printf(" data_offset [%lld]\n", pAVFormatCtx->data_offset  );
    printf(" mux_rate [%d]\n", pAVFormatCtx->mux_rate  );
    printf(" packet_size [%u]\n", pAVFormatCtx->packet_size  );
    printf(" preload [%d]\n", pAVFormatCtx->preload  );
    printf(" max_delay [%d]\n", pAVFormatCtx->max_delay  );

    printf(" loop_output [%d]\n", pAVFormatCtx->loop_output  );
    printf(" loop_input [%d]\n", pAVFormatCtx->loop_input  );

    printf(" probesize [%u]\n", pAVFormatCtx->probesize  );
    printf(" max_analyze_duration [%d]\n", pAVFormatCtx->max_analyze_duration  );
    printf(" keylen [%d]\n", pAVFormatCtx->keylen  );



    printf(" video_codec_id [%d]\n", pAVFormatCtx->video_codec_id  );
    printf(" audio_codec_id [%d]\n", pAVFormatCtx->audio_codec_id  );
    printf(" subtitle_codec_id [%d]\n", pAVFormatCtx->subtitle_codec_id  );
    printf(" max_index_size [%u]\n", pAVFormatCtx->max_index_size  );
    printf(" max_picture_buffer [%u]\n", pAVFormatCtx->max_picture_buffer  );

    printf(" raw_packet_buffer_remaining_size [%d]\n", pAVFormatCtx->raw_packet_buffer_remaining_size  );
    printf(" start_time_realtime [%lld]\n", pAVFormatCtx->start_time_realtime  );
    printf(" fps_probe_size [%d]\n", pAVFormatCtx->fps_probe_size  );
    printf(" ts_id [%d]\n", pAVFormatCtx->ts_id  );
    printf("===================================end========================\n");


    return 0;
}


int output_AVCodecContext(AVCodecContext *pAVCodecCtx)
{
    printf("Now output AVCodecContext Structure:\n");
    printf("===========================AVCodecContext=====start===========================\n");


    printf(" bit_rate [%d]\n", pAVCodecCtx->bit_rate  );
    printf(" bit_rate_tolerance [%d]\n", pAVCodecCtx->bit_rate_tolerance  );
    printf(" flags [%d]\n", pAVCodecCtx->flags  );
    printf(" sub_id [%d]\n", pAVCodecCtx->sub_id  );


    printf(" width [%d]\n", pAVCodecCtx->width  );
    printf(" height [%d]\n", pAVCodecCtx->height  );
    printf(" pix_fmt [%d]\n", pAVCodecCtx->pix_fmt  );

    printf(" sample_rate [%d]\n", pAVCodecCtx->sample_rate  );
    printf(" channels [%d]\n", pAVCodecCtx->channels  );
    printf(" sample_fmt [%d]\n", pAVCodecCtx->sample_fmt  );

    printf(" frame_size [%d]\n", pAVCodecCtx->frame_size  );
    printf(" frame_number [%d]\n", pAVCodecCtx->frame_number  );
    printf(" delay [%d]\n", pAVCodecCtx->delay  );
    printf(" qcompress [%f]\n", pAVCodecCtx->qcompress  );
    printf(" qblur [%f]\n", pAVCodecCtx->qblur  );
    printf(" qmin [%d]\n", pAVCodecCtx->qmin  );
    printf(" qmax [%d]\n", pAVCodecCtx->qmax  );
    printf(" max_qdiff [%d]\n", pAVCodecCtx->max_qdiff  );
    printf(" max_b_frames [%d]\n", pAVCodecCtx->max_b_frames  );
    printf(" b_quant_factor [%f]\n", pAVCodecCtx->b_quant_factor  );
    printf(" rc_strategy [%d]\n", pAVCodecCtx->rc_strategy  );
    printf(" b_frame_strategy [%d]\n", pAVCodecCtx->b_frame_strategy  );


    printf(" rtp_payload_size [%d]\n", pAVCodecCtx->rtp_payload_size  );
    printf(" mv_bits [%d]\n", pAVCodecCtx->mv_bits  );
    printf(" header_bits [%d]\n", pAVCodecCtx->header_bits  );
    printf(" i_tex_bits [%d]\n", pAVCodecCtx->i_tex_bits  );
    printf(" p_tex_bits [%d]\n", pAVCodecCtx->p_tex_bits  );
    printf(" i_count [%d]\n", pAVCodecCtx->i_count  );
    printf(" p_count [%d]\n", pAVCodecCtx->p_count  );
    printf(" skip_count [%d]\n", pAVCodecCtx->skip_count  );
    printf(" misc_bits [%d]\n", pAVCodecCtx->misc_bits  );


    printf(" frame_bits [%d]\n", pAVCodecCtx->frame_bits  );
    printf(" codec_type [%d]\n", pAVCodecCtx->codec_type  );
    printf(" codec_id [%d]\n", pAVCodecCtx->codec_id  );
    printf(" codec_tag [%u]\n", pAVCodecCtx->codec_tag  );
    printf(" workaround_bugs [%d]\n", pAVCodecCtx->workaround_bugs  );
    printf(" luma_elim_threshold [%d]\n", pAVCodecCtx->luma_elim_threshold  );
    printf(" chroma_elim_threshold [%d]\n", pAVCodecCtx->chroma_elim_threshold  );
    printf(" strict_std_compliance [%d]\n", pAVCodecCtx->strict_std_compliance  );
    printf(" b_quant_offset [%d]\n", pAVCodecCtx->b_quant_offset  );
    printf(" error_recognition [%d]\n", pAVCodecCtx->error_recognition  );


    printf(" has_b_frames [%d]\n", pAVCodecCtx->has_b_frames  );
    printf(" block_align [%d]\n", pAVCodecCtx->block_align  );
    printf(" parse_only [%d]\n", pAVCodecCtx->parse_only  );
    printf(" mpeg_quant [%d]\n", pAVCodecCtx->mpeg_quant  );
    printf(" stats_out [%s]\n", pAVCodecCtx->stats_out  );
    printf(" stats_in [%s]\n", pAVCodecCtx->stats_in  );


    printf(" rc_qsquish [%f]\n", pAVCodecCtx->rc_qsquish  );
    printf(" rc_qmod_amp [%f]\n", pAVCodecCtx->rc_qmod_amp  );
    printf(" rc_qmod_freq [%d]\n", pAVCodecCtx->rc_qmod_freq  );

    printf(" rc_override_count [%d]\n", pAVCodecCtx->rc_override_count  );
    printf(" rc_eq [%s]\n", pAVCodecCtx->rc_eq  );
    printf(" rc_max_rate [%d]\n", pAVCodecCtx->rc_max_rate  );
    printf(" rc_min_rate [%d]\n", pAVCodecCtx->rc_min_rate  );
    printf(" rc_buffer_size [%d]\n", pAVCodecCtx->rc_buffer_size  );


    printf(" rc_buffer_aggressivity [%f]\n", pAVCodecCtx->rc_buffer_aggressivity  );
    printf(" i_quant_factor [%f]\n", pAVCodecCtx->i_quant_factor  );
    printf(" i_quant_offset [%f]\n", pAVCodecCtx->i_quant_offset  );
    printf(" dct_algo [%f]\n", pAVCodecCtx->dct_algo  );
    printf(" lumi_masking [%f]\n", pAVCodecCtx->lumi_masking  );
    printf(" temporal_cplx_masking [%f]\n", pAVCodecCtx->temporal_cplx_masking  );
    printf(" spatial_cplx_masking [%f]\n", pAVCodecCtx->spatial_cplx_masking  );
    printf(" p_masking [%f]\n", pAVCodecCtx->p_masking  );
    printf(" dark_masking [%f]\n", pAVCodecCtx->dark_masking  );


    printf(" idct_algo [%d]\n", pAVCodecCtx->idct_algo  );
    printf(" slice_count [%d]\n", pAVCodecCtx->slice_count  );

    printf(" error_concealment [%d]\n", pAVCodecCtx->error_concealment  );
    printf(" dsp_mask [%u]\n", pAVCodecCtx->dsp_mask  );
    printf(" bits_per_coded_sample [%d]\n", pAVCodecCtx->bits_per_coded_sample  );
    printf(" prediction_method [%d]\n", pAVCodecCtx->prediction_method  );
    printf(" sample_aspect_ratio.num [%d]\n", pAVCodecCtx->sample_aspect_ratio.num  );
    printf(" sample_aspect_ratio.den [%d]\n", pAVCodecCtx->sample_aspect_ratio.den  );


    printf(" debug [%d]\n", pAVCodecCtx->debug  );
    printf(" debug_mv [%d]\n", pAVCodecCtx->debug_mv  );
    printf(" error[0] [%lld]\n", pAVCodecCtx->error[0]  );
    printf(" error[1] [%lld]\n", pAVCodecCtx->error[1]  );
    printf(" error[2] [%lld]\n", pAVCodecCtx->error[2]  );
    printf(" error[3] [%lld]\n", pAVCodecCtx->error[3]  );


    printf(" me_cmp [%d]\n", pAVCodecCtx->me_cmp  );
    printf(" me_sub_cmp [%d]\n", pAVCodecCtx->me_sub_cmp  );
    printf(" mb_cmp [%d]\n", pAVCodecCtx->mb_cmp  );
    printf(" ildct_cmp [%d]\n", pAVCodecCtx->ildct_cmp  );
    printf(" dia_size [%d]\n", pAVCodecCtx->dia_size  );
    printf(" last_predictor_count [%d]\n", pAVCodecCtx->last_predictor_count  );
    printf(" pre_me [%d]\n", pAVCodecCtx->pre_me  );
    printf(" me_pre_cmp [%d]\n", pAVCodecCtx->me_pre_cmp  );
    printf(" pre_dia_size [%d]\n", pAVCodecCtx->pre_dia_size  );
    printf(" me_subpel_quality [%d]\n", pAVCodecCtx->me_subpel_quality  );


    printf(" dtg_active_format [%d]\n", pAVCodecCtx->dtg_active_format  );
    printf(" me_range [%d]\n", pAVCodecCtx->me_range  );
    printf(" intra_quant_bias [%d]\n", pAVCodecCtx->intra_quant_bias  );
    printf(" inter_quant_bias [%d]\n", pAVCodecCtx->inter_quant_bias  );
    printf(" color_table_id [%d]\n", pAVCodecCtx->color_table_id  );
    printf(" internal_buffer_count [%d]\n", pAVCodecCtx->internal_buffer_count  );
    printf(" global_quality [%d]\n", pAVCodecCtx->global_quality  );
    printf(" coder_type [%d]\n", pAVCodecCtx->coder_type  );
    printf(" context_model [%d]\n", pAVCodecCtx->context_model  );
    printf(" slice_flags [%d]\n", pAVCodecCtx->slice_flags  );
    printf(" xvmc_acceleration [%d]\n", pAVCodecCtx->xvmc_acceleration  );
    printf(" mb_decision [%d]\n", pAVCodecCtx->mb_decision  );


    printf(" stream_codec_tag [%u]\n", pAVCodecCtx->stream_codec_tag  );
    printf(" scenechange_threshold [%d]\n", pAVCodecCtx->scenechange_threshold  );
    printf(" lmin [%d]\n", pAVCodecCtx->lmin  );
    printf(" lmax [%d]\n", pAVCodecCtx->lmax  );
    printf(" noise_reduction [%d]\n", pAVCodecCtx->noise_reduction  );
    printf(" rc_initial_buffer_occupancy [%d]\n", pAVCodecCtx->rc_initial_buffer_occupancy  );
    printf(" inter_threshold [%d]\n", pAVCodecCtx->inter_threshold  );
    printf(" flags2 [%d]\n", pAVCodecCtx->flags2  );
    printf(" error_rate [%d]\n", pAVCodecCtx->error_rate  );


    printf(" coded_width [%d]\n", pAVCodecCtx->coded_width  );
    printf(" coded_height [%d]\n", pAVCodecCtx->coded_height  );
    printf(" frame_skip_threshold [%d]\n", pAVCodecCtx->frame_skip_threshold  );
    printf(" frame_skip_factor [%d]\n", pAVCodecCtx->frame_skip_factor  );
    printf(" frame_skip_exp [%d]\n", pAVCodecCtx->frame_skip_exp  );
    printf(" frame_skip_cmp [%d]\n", pAVCodecCtx->frame_skip_cmp  );


    printf(" reordered_opaque [%lld]\n", pAVCodecCtx->reordered_opaque  );
    printf(" bits_per_raw_sample [%d]\n", pAVCodecCtx->bits_per_raw_sample  );
    printf(" channel_layout [%lld]\n", pAVCodecCtx->channel_layout  );
    printf(" request_channel_layout [%lld]\n", pAVCodecCtx->request_channel_layout  );
    printf(" ticks_per_frame [%d]\n", pAVCodecCtx->ticks_per_frame  );
    printf(" color_primaries [%d]\n", pAVCodecCtx->color_primaries  );
    printf(" color_trc [%d]\n", pAVCodecCtx->color_trc  );
    printf(" colorspace [%d]\n", pAVCodecCtx->colorspace  );
    printf(" color_range [%d]\n", pAVCodecCtx->color_range  );
    printf(" chroma_sample_location [%d]\n", pAVCodecCtx->chroma_sample_location  );
    printf(" weighted_p_pred [%d]\n", pAVCodecCtx->weighted_p_pred  );


    printf(" aq_mode [%d]\n", pAVCodecCtx->aq_mode  );
    printf(" aq_strength [%f]\n", pAVCodecCtx->aq_strength  );
    printf(" psy_rd [%f]\n", pAVCodecCtx->psy_rd  );
    printf(" psy_trellis [%f]\n", pAVCodecCtx->psy_trellis  );
    printf(" rc_lookahead [%d]\n", pAVCodecCtx->rc_lookahead  );



    printf(" slices [%d]\n", pAVCodecCtx->slices  );
    printf(" subtitle_header_size [%d]\n", pAVCodecCtx->subtitle_header_size  );
    printf(" thread_type [%d]\n", pAVCodecCtx->thread_type  );
    printf(" active_thread_type [%d]\n", pAVCodecCtx->active_thread_type  );
    printf(" thread_safe_callbacks [%d]\n", pAVCodecCtx->thread_safe_callbacks  );
    printf(" vbv_delay [%d]\n", pAVCodecCtx->vbv_delay  );

    printf(" audio_service_type [%d]\n", pAVCodecCtx->audio_service_type  );
    printf(" request_sample_fmt [%d]\n", pAVCodecCtx->request_sample_fmt  );
    printf(" pts_correction_num_faulty_pts [%lld]\n", pAVCodecCtx->pts_correction_num_faulty_pts  );
    printf(" pts_correction_num_faulty_dts [%lld]\n", pAVCodecCtx->pts_correction_num_faulty_dts  );
    printf(" pts_correction_last_pts [%lld]\n", pAVCodecCtx->pts_correction_last_pts  );
    printf(" pts_correction_last_dts [%lld]\n", pAVCodecCtx->pts_correction_last_dts  );


    printf("==============================AVCodecContext=====end========================\n");
    return 0;
}



int output_AVPacket(AVPacket *pAVPacket)
{
    printf("Now output AVPacket Structure:\n");
    printf("=============================AVPacket===start===========================\n");

    printf(" pts [%lld]\n", pAVPacket->pts  );
    printf(" dts [%lld]\n", pAVPacket->dts  );
    printf(" size [%d]\n", pAVPacket->size  );
    printf(" stream_index [%d]\n", pAVPacket->stream_index  );
    printf(" flags [%d]\n", pAVPacket->flags  );
    printf(" side_data_elems [%d]\n", pAVPacket->side_data_elems  );
    printf(" duration [%d]\n", pAVPacket->duration  );
    printf(" pos [%lld]\n", pAVPacket->pos  );
    printf(" convergence_duration [%d]\n", pAVPacket->convergence_duration  );


    printf("==============================AVPacket=====end========================\n");
    return 0;
}




























