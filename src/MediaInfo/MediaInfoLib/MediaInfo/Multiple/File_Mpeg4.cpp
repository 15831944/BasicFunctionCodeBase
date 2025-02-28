// File_Mpeg4 - Info for MPEG-4 files
// Copyright (C) 2005-2012 MediaArea.net SARL, Info@MediaArea.net
//
// This library is free software: you can redistribute it and/or modify it
// under the terms of the GNU Library General Public License as published by
// the Free Software Foundation, either version 2 of the License, or
// any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU Library General Public License for more details.
//
// You should have received a copy of the GNU Library General Public License
// along with this library. If not, see <http://www.gnu.org/licenses/>.
//
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
// Main part
//
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//---------------------------------------------------------------------------
// Pre-compilation
#include "MediaInfo/PreComp.h"
#ifdef __BORLANDC__
    #pragma hdrstop
#endif
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
#include "MediaInfo/Setup.h"
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
#ifdef MEDIAINFO_MPEG4_YES
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
#include "MediaInfo/Multiple/File_Mpeg4.h"
#include "MediaInfo/Multiple/File_Mpeg4_Descriptors.h"
#if defined(MEDIAINFO_MPEGPS_YES)
    #include "MediaInfo/Multiple/File_MpegPs.h"
#endif
#include "ZenLib/FileName.h"
#include "MediaInfo/MediaInfo_Internal.h"
#if MEDIAINFO_EVENTS
    #include "MediaInfo/MediaInfo_Events.h"
#endif //MEDIAINFO_EVENTS
#ifdef MEDIAINFO_REFERENCES_YES
    #include "MediaInfo/Multiple/File__ReferenceFilesHelper.h"
#endif //MEDIAINFO_REFERENCES_YES
#include "ZenLib/Format/Http/Http_Utils.h"
//---------------------------------------------------------------------------

namespace MediaInfoLib
{

//***************************************************************************
// Const
//***************************************************************************

namespace Elements
{
    const int64u free=0x66726565;
    const int64u mdat=0x6D646174;
    const int64u moov_meta______=0x2D2D2D2D;
    const int64u moov_meta___ART=0xA9415254;
    const int64u moov_meta___alb=0xA9616C62;
    const int64u moov_meta___aut=0xA9617574;
    const int64u moov_meta___cmt=0xA9636D74;
    const int64u moov_meta___cpy=0xA9637079;
    const int64u moov_meta___day=0xA9646179;
    const int64u moov_meta___des=0xA9646573;
    const int64u moov_meta___dir=0xA9646972;
    const int64u moov_meta___dis=0xA9646973;
    const int64u moov_meta___edl=0xA965646C;
    const int64u moov_meta___enc=0xA9656E63;
    const int64u moov_meta___fmt=0xA9666D74;
    const int64u moov_meta___gen=0xA967656E;
    const int64u moov_meta___grp=0xA9677270;
    const int64u moov_meta___hos=0xA9686F73;
    const int64u moov_meta___inf=0xA9696E66;
    const int64u moov_meta___key=0xA96B6579;
    const int64u moov_meta___mak=0xA96D616B;
    const int64u moov_meta___mod=0xA96D6F64;
    const int64u moov_meta___nam=0xA96E616D;
    const int64u moov_meta___prd=0xA9707264;
    const int64u moov_meta___PRD=0xA9505244;
    const int64u moov_meta___prf=0xA9707266;
    const int64u moov_meta___req=0xA9726571;
    const int64u moov_meta___src=0xA9737263;
    const int64u moov_meta___swr=0xA9737772;
    const int64u moov_meta___too=0xA9746F6F;
    const int64u moov_meta___wrn=0xA977726E;
    const int64u moov_meta___wrt=0xA9777274;
    const int64u moov_meta__aART=0x61415254;
    const int64u moov_meta__albm=0x616C626D;
    const int64u moov_meta__auth=0x61757468;
    const int64u moov_meta__cpil=0x6370696C;
    const int64u moov_meta__cprt=0x63707274;
    const int64u moov_meta__covr=0x636F7672;
    const int64u moov_meta__disk=0x6469736B;
    const int64u moov_meta__dscp=0x64736370;
    const int64u moov_meta__gnre=0x676E7265;
    const int64u moov_meta__name=0x6E616D65;
    const int64u moov_meta__perf=0x70657266;
    const int64u moov_meta__pgap=0x70676170;
    const int64u moov_meta__titl=0x7469746C;
    const int64u moov_meta__tool=0x746F6F6C;
    const int64u moov_meta__trkn=0x74726B6E;
    const int64u moov_meta__tmpo=0x746D706F;
    const int64u moov_meta__year=0x79656172;
    const int64u skip=0x736B6970;
    const int64u wide=0x77696465;
}

//---------------------------------------------------------------------------
Ztring Mpeg4_Encoded_Library(int32u Vendor)
{
    switch (Vendor)
    {
        case 0x33495658 : return __T("3ivX");                //3IVX
        case 0x6170706C : return __T("Apple QuickTime");     //appl
        case 0x6E696B6F : return __T("Nikon");               //niko
        case 0x6F6C796D : return __T("Olympus");             //olym
        case 0x6F6D6E65 : return __T("Omneon");              //omne
        default: return Ztring().From_CC4(Vendor);
    }
}

//---------------------------------------------------------------------------
Ztring Mpeg4_Language_Apple(int16u Language)
{
    switch (Language)
    {
        case  0 : return __T("en");
        case  1 : return __T("fr");
        case  2 : return __T("de");
        case  6 : return __T("es");
        default: return Ztring::ToZtring(Language);
    }
}

//---------------------------------------------------------------------------
extern const char* Mpeg4_chan(int16u Ordering);
extern const char* Mpeg4_chan_Layout(int16u Ordering);

//***************************************************************************
// Constructor/Destructor
//***************************************************************************

//---------------------------------------------------------------------------
File_Mpeg4::File_Mpeg4()
:File__Analyze()
{
    //Configuration
    #if MEDIAINFO_EVENTS
        ParserIDs[0]=MediaInfo_Parser_Mpeg4;
        StreamIDs_Width[0]=8;
    #endif //MEDIAINFO_EVENTS
    #if MEDIAINFO_DEMUX
        Demux_Level=2; //Container
    #endif //MEDIAINFO_DEMUX
    #if MEDIAINFO_TRACE
        Trace_Layers_Update(0); //Container1
    #endif //MEDIAINFO_TRACE

    DataMustAlwaysBeComplete=false;

    //Temp
    mdat_MustParse=false;
    TimeScale=1;
    Vendor=0x00000000;
    FirstMdatPos=(int64u)-1;
    LastMdatPos=0;
    FirstMoovPos=(int64u)-1;
    MajorBrand=0x00000000;
    IsSecondPass=false;
    IsParsing_mdat=false;
    IsFragmented=false;
    moov_trak_tkhd_TrackID=(int32u)-1;
    ReferenceFiles=NULL;
    mdat_Pos_NormalParsing=false;
    moof_traf_base_data_offset=(int64u)-1;
    data_offset_present=true;
    #if MEDIAINFO_NEXTPACKET
        ReferenceFiles_IsParsing=false;
    #endif //MEDIAINFO_NEXTPACKET
    #if MEDIAINFO_DEMUX
        TimeCode_FrameOffset=0;
        TimeCode_DtsOffset=0;
    #endif //MEDIAINFO_DEMUX
}

//---------------------------------------------------------------------------
File_Mpeg4::~File_Mpeg4()
{
}

//***************************************************************************
// Streams management
//***************************************************************************

//---------------------------------------------------------------------------
void File_Mpeg4::Streams_Accept()
{
    if (!IsSub && MajorBrand==0x6A703220) //"jp2 "
    {
        Streams_Accept_TestContinuousFileNames();

        Stream_Prepare((Config->File_Names.size()>1 || Config->File_IsReferenced_Get())?Stream_Video:Stream_Image);
        if (StreamKind_Last==Stream_Video)
            Fill(Stream_Video, StreamPos_Last, Video_FrameCount, Config->File_Names.size());
    }

    //Configuration
    Buffer_MaximumSize=64*1024*1024; //Some big frames are possible (e.g YUV 4:2:2 10 bits 1080p)
}

//---------------------------------------------------------------------------
void File_Mpeg4::Streams_Finish()
{
    #if defined(MEDIAINFO_REFERENCES_YES) && MEDIAINFO_NEXTPACKET
        //Locators only
        if (ReferenceFiles_IsParsing)
        {
            ReferenceFiles->ParseReferences();
            #if MEDIAINFO_DEMUX
                if (Config->Demux_EventWasSent)
                    return;
            #endif //MEDIAINFO_DEMUX

            Streams_Finish_CommercialNames();
            return;
        }
    #endif //defined(MEDIAINFO_REFERENCES_YES) && MEDIAINFO_NEXTPACKET

    //Final Cut EIA-608 format
    if (Retrieve(Stream_General, 0, General_Format)==__T("Final Cut EIA-608"))
    {
        for (streams::iterator Stream=Streams.begin(); Stream!=Streams.end(); ++Stream)
        {
            Stream->second.Parser->Finish();
            if (Stream->second.Parser->Count_Get(Stream_Text))
            {
                Stream_Prepare(Stream_Text);
                Fill(Stream_Text, StreamPos_Last, Text_ID, Stream->first==1?"608-1":"608-2");
                Fill(Stream_Text, StreamPos_Last, "MuxingMode", __T("Final Cut"), Unlimited);
                Merge(*Stream->second.Parser, Stream_Text, 0, StreamPos_Last);
            }
        }

        return;
    }

    Fill_Flush();
    int64u File_Size_Total=File_Size;

    //TimeCode
    for (streams::iterator Temp=Streams.begin(); Temp!=Streams.end(); ++Temp)
        if (Temp->second.TimeCode)
            TimeCode_Associate(Temp->first);

    //For each stream
    streams::iterator Temp=Streams.begin();
    while (Temp!=Streams.end())
    {
        //Preparing
        StreamKind_Last=Temp->second.StreamKind;
        StreamPos_Last=Temp->second.StreamPos;

        //if (Temp->second.stsz_StreamSize)
        //    Fill(StreamKind_Last, StreamPos_Last, Fill_Parameter(StreamKind_Last, Generic_StreamSize), Temp->second.stsz_StreamSize);

        //Edit lists coherencies
        if (Temp->second.edts.size()>1 && Temp->second.edts[0].Duration==Temp->second.tkhd_Duration)
        {
            bool Duplicates=true;
            for (size_t Pos=1; Pos<Temp->second.edts.size(); Pos++)
                if (Temp->second.edts[Pos-1].Delay!=Temp->second.edts[Pos].Delay || Temp->second.edts[Pos-1].Duration!=Temp->second.edts[Pos].Duration || Temp->second.edts[Pos-1].Rate!=Temp->second.edts[Pos].Rate)
                    Duplicates=false;
            if (Duplicates)
                Temp->second.edts.resize(1);
        }

        //Edit Lists
        float64 Delay=0;
        switch (Temp->second.edts.size())
        {
            case 0 :
                    break;
            case 1 :
                    if (Temp->second.edts[0].Duration==Temp->second.tkhd_Duration && Temp->second.edts[0].Rate==0x00010000)
                    {
                        Delay=Temp->second.edts[0].Delay;
                        Delay=-Delay;
                        Delay/=Temp->second.mdhd_TimeScale; //In seconds
                    }
                    break;
            case 2 :
                    if (Temp->second.edts[0].Delay==(int32u)-1 && Temp->second.edts[0].Duration+Temp->second.edts[1].Duration==Temp->second.tkhd_Duration && Temp->second.edts[0].Rate==0x00010000 && Temp->second.edts[1].Rate==0x00010000)
                    {
                        Delay=Temp->second.edts[0].Duration;
                        Temp->second.tkhd_Duration-=Delay;
                        Delay/=TimeScale; //In seconds
                    }
                    break;
            default:
                    break; //TODO: handle more complex Edit Lists
        }
        Delay+=Retrieve(StreamKind_Last, StreamPos_Last, Fill_Parameter(StreamKind_Last, Generic_Delay)).To_float64()/1000; //TODO: use TimeCode value directly instead of the rounded value
        if (!Retrieve(StreamKind_Last, StreamPos_Last, Fill_Parameter(StreamKind_Last, Generic_Delay_Source)).empty() && Retrieve(StreamKind_Last, StreamPos_Last, Fill_Parameter(StreamKind_Last, Generic_Delay_Source))!=__T("Container"))
        {
            Fill(StreamKind_Last, StreamPos_Last, Fill_Parameter(StreamKind_Last, Generic_Delay_Original), Retrieve(StreamKind_Last, StreamPos_Last, Fill_Parameter(StreamKind_Last, Generic_Delay)));
            Fill(StreamKind_Last, StreamPos_Last, Fill_Parameter(StreamKind_Last, Generic_Delay_Original_Source), Retrieve(StreamKind_Last, StreamPos_Last, Fill_Parameter(StreamKind_Last, Generic_Delay_Source)));
        }
        Fill(StreamKind_Last, StreamPos_Last, Fill_Parameter(StreamKind_Last, Generic_Delay), Delay*1000, 0, true);
        Fill(StreamKind_Last, StreamPos_Last, Fill_Parameter(StreamKind_Last, Generic_Delay_Source), "Container", Unlimited, true, true);

        //Fragments
        if (IsFragmented)
        {
            Fill(StreamKind_Last, StreamPos_Last, Fill_Parameter(StreamKind_Last, Generic_Duration), Temp->second.stts_Duration/((float)Temp->second.mdhd_TimeScale)*1000, 0, true);
            Fill(StreamKind_Last, StreamPos_Last, Fill_Parameter(StreamKind_Last, Generic_FrameCount), Temp->second.stts_FrameCount, 10, true);
        }

        //Duration/StreamSize
        if (TimeScale && Temp->second.TimeCode==NULL && Temp->second.mdhd_TimeScale)
        {
            Ztring Duration_stts_FirstFrame, Duration_stts_LastFrame;
            if (Temp->second.stts_Duration_FirstFrame)
                Duration_stts_FirstFrame.From_Number(((float32)(((int32s)Temp->second.stts_Duration_FirstFrame)-((int32s)Temp->second.stts[1].SampleDuration)))*1000/Temp->second.mdhd_TimeScale, 0); //The duration of the frame minus 1 normal frame duration
            if (Temp->second.stts_Duration_LastFrame)
                Duration_stts_LastFrame.From_Number(((float32)(((int32s)Temp->second.stts_Duration_LastFrame)-((int32s)Temp->second.stts[Temp->second.stts_Duration_FirstFrame?1:0].SampleDuration)))*1000/Temp->second.mdhd_TimeScale, 0); //The duration of the frame minus 1 normal frame duration

            float32 Duration_tkhd_H=((float32)(Temp->second.tkhd_Duration+1))/TimeScale;
            float32 Duration_tkhd_L=((float32)(Temp->second.tkhd_Duration-1))/TimeScale;
            float32 Duration_stts=((float32)Temp->second.stts_Duration)/Temp->second.mdhd_TimeScale;
            if (!IsFragmented && Duration_stts && !(Duration_stts>=Duration_tkhd_L && Duration_stts<=Duration_tkhd_H))
            {
                //There is a difference between media/stts atom and track atom
                Fill(StreamKind_Last, StreamPos_Last, "Source_Duration", Duration_stts*1000, 0);
                Fill(StreamKind_Last, StreamPos_Last, "Source_Duration_FirstFrame", Duration_stts_FirstFrame);
                Fill(StreamKind_Last, StreamPos_Last, "Source_Duration_LastFrame", Duration_stts_LastFrame);
                if (Temp->second.stts.size()!=1 || Temp->second.mdhd_TimeScale<100 || Temp->second.stts[0].SampleDuration!=1) //TODO: test PCM
                    if (Temp->second.stts_FrameCount)
                        Fill(StreamKind_Last, StreamPos_Last, "Source_FrameCount", Temp->second.stts_FrameCount);
                if (Temp->second.stsz_StreamSize)
                    Fill(StreamKind_Last, StreamPos_Last, "Source_StreamSize", Temp->second.stsz_StreamSize);

                //Calculating new properties based on track duration
                Fill(StreamKind_Last, StreamPos_Last, Fill_Parameter(StreamKind_Last, Generic_Duration), ((float32)Temp->second.tkhd_Duration)/TimeScale*1000, 0, true);
                //Fill(StreamKind_Last, StreamPos_Last, "Duration_FirstFrame", Duration_stts_FirstFrame);
                Clear(StreamKind_Last, StreamPos_Last, "Duration_LastFrame"); //TODO

                int64u FrameCount;
                if (Temp->second.stts_Min && Temp->second.stts_Min==Temp->second.stts_Max)
                    FrameCount=float64_int64s(((float64)Temp->second.tkhd_Duration)/TimeScale*Temp->second.mdhd_TimeScale/Temp->second.stts_Min);
                else
                {
                    FrameCount=0;
                    int64u Ticks_Max=float64_int64s(((float64)Temp->second.tkhd_Duration)/TimeScale*Temp->second.mdhd_TimeScale);
                    int64u Ticks=0;
                    for (size_t stts_Pos=0; stts_Pos<Temp->second.stts.size(); stts_Pos++)
                    {
                        int64u Ticks_Complete=Temp->second.stts[stts_Pos].SampleCount*Temp->second.stts[stts_Pos].SampleDuration;
                        if (Ticks+Ticks_Complete>=Ticks_Max)
                        {
                            if (Temp->second.stts[stts_Pos].SampleDuration)
                                FrameCount+=float64_int64s(((float64)(Ticks_Max-Ticks))/Temp->second.stts[stts_Pos].SampleDuration);
                            break;
                        }
                        Ticks+=Ticks_Complete;
                        FrameCount+=Temp->second.stts[stts_Pos].SampleCount;
                    }
                }
                if (Temp->second.stts.size()!=1 || Temp->second.mdhd_TimeScale<100 || Temp->second.stts[0].SampleDuration!=1) //TODO: test PCM
                    Fill(StreamKind_Last, StreamPos_Last, "FrameCount", FrameCount, 10, true);

                if (Temp->second.stsz_Total.empty())
                    Fill(StreamKind_Last, StreamPos_Last, "StreamSize", FrameCount*Temp->second.stsz_Sample_Size*Temp->second.stsz_Sample_Multiplier);
                else if (FrameCount<=Temp->second.stsz_Total.size())
                {
                    int64u StreamSize=0;
                    for (size_t stsz_Pos=0; stsz_Pos<FrameCount; stsz_Pos++)
                        StreamSize+=Temp->second.stsz_Total[stsz_Pos];
                    bool HasEncodedBitRate=!Retrieve(StreamKind_Last, StreamPos_Last, "BitRate_Encoded").empty();
                    Fill(StreamKind_Last, StreamPos_Last, HasEncodedBitRate?"StreamSize_Encoded":"StreamSize", StreamSize);
                }
            }
            else
            {
                //Normal
                if (Retrieve(StreamKind_Last, StreamPos_Last, Fill_Parameter(StreamKind_Last, Generic_Duration)).empty())
                    Fill(StreamKind_Last, StreamPos_Last, Fill_Parameter(StreamKind_Last, Generic_Duration), Duration_stts*1000, 0);
                Fill(StreamKind_Last, StreamPos_Last, "Duration_FirstFrame", Duration_stts_FirstFrame);
                Fill(StreamKind_Last, StreamPos_Last, "Duration_LastFrame", Duration_stts_LastFrame);
                if (Temp->second.stts.size()!=1 || Temp->second.mdhd_TimeScale<100 || Temp->second.stts[0].SampleDuration!=1) //TODO: test PCM
                    if (Retrieve(StreamKind_Last, StreamPos_Last, Fill_Parameter(StreamKind_Last, Generic_FrameCount)).empty() && Temp->second.stts_FrameCount)
                        Fill(StreamKind_Last, StreamPos_Last, Fill_Parameter(StreamKind_Last, Generic_FrameCount), Temp->second.stts_FrameCount);
                bool HasPadding=(Temp->second.Parser && !Temp->second.Parser->Retrieve(StreamKind_Last, StreamPos_Last, "BitRate_Encoded").empty()) || (Temp->second.Parser && Temp->second.Parser->Buffer_TotalBytes && ((float32)Temp->second.Parser->Buffer_PaddingBytes)/Temp->second.Parser->Buffer_TotalBytes>0.02);
                if (Temp->second.stsz_StreamSize)
                    Fill(StreamKind_Last, StreamPos_Last, Fill_Parameter(StreamKind_Last, HasPadding?Generic_StreamSize_Encoded:Generic_StreamSize), Temp->second.stsz_StreamSize);
            }
        }

        if (StreamKind_Last==Stream_Video && Temp->second.TimeCode==NULL)
        {
            if (Temp->second.mdhd_TimeScale && Temp->second.stts_Min && Temp->second.stts_Max)
            {
                if (Temp->second.stts_Min==0 || Temp->second.stts_Max==0 || (Temp->second.stts_Min!=Temp->second.stts_Max && ((float)Temp->second.mdhd_TimeScale)/Temp->second.stts_Min-((float)Temp->second.mdhd_TimeScale)/Temp->second.stts_Max>=0.001))
                {
                    if (Temp->second.stts_Max)
                        Fill(Stream_Video, StreamPos_Last, Video_FrameRate_Minimum, ((float)Temp->second.mdhd_TimeScale)/Temp->second.stts_Max, 3, true);
                    if (Temp->second.stts_Min)
                        Fill(Stream_Video, StreamPos_Last, Video_FrameRate_Maximum, ((float)Temp->second.mdhd_TimeScale)/Temp->second.stts_Min, 3, true);
                    if (Temp->second.stts_Duration)
                        Fill(Stream_Video, StreamPos_Last, Video_FrameRate,         ((float)Temp->second.stts_FrameCount)/Temp->second.stts_Duration*Temp->second.mdhd_TimeScale, 3, true);
                    Fill(Stream_Video, StreamPos_Last, Video_FrameRate_Mode,    "VFR", Unlimited, true, true);
                }
                else
                {
                    Fill(Stream_Video, StreamPos_Last, Video_FrameRate,         ((float)Temp->second.mdhd_TimeScale)/Temp->second.stts_Max, 3, true);
                    Fill(Stream_Video, StreamPos_Last, Video_FrameRate_Mode,    "CFR", Unlimited, true, true);
                }
            }
        }

        //Coherency
        if (!IsFragmented && Temp->second.stts_Duration!=Temp->second.mdhd_Duration && Temp->second.mdhd_TimeScale)
        {
            //There is a difference between media/mdhd atom and track atom
            Fill(StreamKind_Last, StreamPos_Last, "mdhd_Duration", ((float32)Temp->second.mdhd_Duration)/Temp->second.mdhd_TimeScale*1000, 0);
        }

        //Parser specific
        if (Temp->second.Parser)
        {
            if (Config->ParseSpeed<=1.0)
            {
                Fill(Temp->second.Parser);
                Temp->second.Parser->Open_Buffer_Unsynch();
            }

            //Finalizing and Merging
            Finish(Temp->second.Parser);
            if (StreamKind_Last==Stream_General)
            {
                //Special case for TimeCode without link
                for (std::map<int32u, stream>::iterator Target=Streams.begin(); Target!=Streams.end(); ++Target)
                    if (Target->second.StreamKind!=Stream_General)
                        Merge(*Temp->second.Parser, Target->second.StreamKind, 0, Target->second.StreamPos);
            }
            else
            {
                //Hacks - Before
                Ztring FrameRate_Temp, FrameRate_Mode_Temp, Duration_Temp, Delay_Temp;
                if (StreamKind_Last==Stream_Video)
                {
                    if (Temp->second.Parser && Retrieve(Stream_Video, 0, Video_CodecID_Hint)==__T("DVCPRO HD"))
                    {
                        Temp->second.Parser->Clear(Stream_Video, 0, Video_FrameRate);
                        Temp->second.Parser->Clear(Stream_Video, 0, Video_Width);
                        Temp->second.Parser->Clear(Stream_Video, 0, Video_Height);
                        Temp->second.Parser->Clear(Stream_Video, 0, Video_DisplayAspectRatio);
                        Temp->second.Parser->Clear(Stream_Video, 0, Video_PixelAspectRatio);
                    }

                    FrameRate_Temp=Retrieve(Stream_Video, StreamPos_Last, Video_FrameRate);
                    FrameRate_Mode_Temp=Retrieve(Stream_Video, StreamPos_Last, Video_FrameRate_Mode);
                    Duration_Temp=Retrieve(Stream_Video, StreamPos_Last, Video_Duration);
                    Delay_Temp=Retrieve(Stream_Video, StreamPos_Last, Video_Delay);

                    //Special case: DV 1080i and MPEG-4 header is lying (saying this is 1920 pixel wide, but this is 1440 pixel wide)
                    if (Temp->second.Parser->Get(Stream_Video, 0, Video_Format)==__T("DV") && Retrieve(Stream_Video, StreamKind_Last, Video_Width)==__T("1080"))
                        Clear(Stream_Video, StreamKind_Last, Video_Width);
                }

                //Special case - Multiple sub-streams in a stream
                if ((Temp->second.Parser->Retrieve(Stream_General, 0, General_Format)==__T("ChannelGrouping") && Temp->second.Parser->Count_Get(Stream_Audio))
                 ||  Temp->second.Parser->Retrieve(Stream_General, 0, General_Format)==__T("Final Cut EIA-608")
                 ||  Temp->second.Parser->Retrieve(Stream_General, 0, General_Format)==__T("Final Cut CDP"))
                {
                    //Before
                    Clear(StreamKind_Last, StreamPos_Last, Fill_Parameter(StreamKind_Last, Generic_StreamSize));
                    if (StreamKind_Last==Stream_Audio)
                    {
                        Clear(Stream_Audio, StreamPos_Last, Audio_Format_Settings_Sign);
                    }
                    ZtringList StreamSave; StreamSave.Write((*File__Analyze::Stream)[StreamKind_Last][StreamPos_Last].Read());
                    ZtringListList StreamMoreSave; StreamMoreSave.Write((*Stream_More)[StreamKind_Last][StreamPos_Last].Read());

                    //Erasing former streams data
                    stream_t NewKind=StreamKind_Last;
                    size_t NewPos1;
                    Ztring ID;
                    if (Temp->second.Parser->Retrieve(Stream_General, 0, General_Format)==__T("ChannelGrouping"))
                    {
                        //Channel coupling, removing the 2 corresponding streams
                        NewPos1=(StreamPos_Last/2)*2;
                        size_t NewPos2=NewPos1+1;
                        ID=Retrieve(StreamKind_Last, NewPos1, General_ID)+__T(" / ")+Retrieve(StreamKind_Last, NewPos2, General_ID);

                        Stream_Erase(NewKind, NewPos2);
                        Stream_Erase(NewKind, NewPos1);

                        streams::iterator NextStream=Temp;
                        ++NextStream;
                        size_t NewAudio_Count=Temp->second.Parser->Count_Get(Stream_Audio);
                        while (NextStream!=Streams.end())
                        {
                            if (NextStream->second.StreamKind==Stream_Audio)
                            {
                                NextStream->second.StreamPos-=2;
                                NextStream->second.StreamPos+=NewAudio_Count;
                            }
                            ++NextStream;
                        }
                    }
                    else
                    {
                        //One channel
                        NewPos1=StreamPos_Last;
                        ID=Retrieve(StreamKind_Last, NewPos1, General_ID);
                        Stream_Erase(StreamKind_Last, StreamPos_Last);
                    }

                    //After
                    size_t New_Count=Temp->second.Parser->Count_Get(NewKind);
                    for (size_t StreamPos=0; StreamPos<New_Count; StreamPos++)
                    {
                        Stream_Prepare(NewKind, NewPos1+StreamPos);
                        Merge(*Temp->second.Parser, StreamKind_Last, StreamPos, StreamPos_Last);
                        Ztring Parser_ID=Retrieve(StreamKind_Last, StreamPos_Last, General_ID);
                        Fill(StreamKind_Last, StreamPos_Last, General_ID, ID+__T("-")+Parser_ID, true);
                        for (size_t Pos=0; Pos<StreamSave.size(); Pos++)
                            if (Retrieve(StreamKind_Last, StreamPos_Last, Pos).empty())
                                Fill(StreamKind_Last, StreamPos_Last, Pos, StreamSave[Pos]);
                        for (size_t Pos=0; Pos<StreamMoreSave.size(); Pos++)
                            Fill(StreamKind_Last, StreamPos_Last, StreamMoreSave(Pos, 0).To_Local().c_str(), StreamMoreSave(Pos, 1));
                    }
                }
                else
                {
                    //Temp->second.Parser->Clear(StreamKind_Last, StreamPos_Last, "Delay"); //DV TimeCode is removed
                    Merge(*Temp->second.Parser, StreamKind_Last, 0, StreamPos_Last);
                }

                //Hacks - After
                if (StreamKind_Last==Stream_Video)
                {
                    Fill(Stream_Video, StreamPos_Last, Video_Duration, Duration_Temp, true);
                    if (!FrameRate_Temp.empty() && FrameRate_Temp!=Retrieve(Stream_Video, StreamPos_Last, Video_FrameRate))
                        Fill(Stream_Video, StreamPos_Last, Video_FrameRate, FrameRate_Temp, true);
                    if (!FrameRate_Mode_Temp.empty() && FrameRate_Mode_Temp!=Retrieve(Stream_Video, StreamPos_Last, Video_FrameRate_Mode))
                        Fill(Stream_Video, StreamPos_Last, Video_FrameRate_Mode, FrameRate_Mode_Temp, true);

                    //Special case for TimeCode and DV multiple audio
                    if (!Delay_Temp.empty() && Delay_Temp!=Retrieve(Stream_Video, StreamPos_Last, Video_Delay))
                    {
                        for (size_t Pos=0; Pos<Count_Get(Stream_Audio); Pos++)
                            if (Retrieve(Stream_Audio, Pos, "MuxingMode_MoreInfo")==__T("Muxed in Video #1"))
                            {
                                //Fill(Stream_Audio, Pos, Audio_Delay_Original, Retrieve(Stream_Audio, Pos, Audio_Delay));
                                Fill(Stream_Audio, Pos, Audio_Delay, Retrieve(Stream_Video, StreamPos_Last, Video_Delay), true);
                                Fill(Stream_Audio, Pos, Audio_Delay_Settings, Retrieve(Stream_Video, StreamPos_Last, Video_Delay_Settings), true);
                                Fill(Stream_Audio, Pos, Audio_Delay_Source, Retrieve(Stream_Video, StreamPos_Last, Video_Delay_Source), true);
                                Fill(Stream_Audio, Pos, Audio_Delay_Original, Retrieve(Stream_Video, StreamPos_Last, Video_Delay_Original), true);
                                Fill(Stream_Audio, Pos, Audio_Delay_Original_Settings, Retrieve(Stream_Video, StreamPos_Last, Video_Delay_Original_Settings), true);
                                Fill(Stream_Audio, Pos, Audio_Delay_Original_Source, Retrieve(Stream_Video, StreamPos_Last, Video_Delay_Original_Source), true);
                            }
                        for (size_t Pos=0; Pos<Count_Get(Stream_Text); Pos++)
                            if (Retrieve(Stream_Text, Pos, "MuxingMode_MoreInfo")==__T("Muxed in Video #1"))
                            {
                                //Fill(Stream_Text, Pos, Text_Delay_Original, Retrieve(Stream_Text, Pos, Text_Delay));
                                Fill(Stream_Text, Pos, Text_Delay, Retrieve(Stream_Video, StreamPos_Last, Video_Delay), true);
                                Fill(Stream_Text, Pos, Text_Delay_Settings, Retrieve(Stream_Video, StreamPos_Last, Video_Delay_Settings), true);
                                Fill(Stream_Text, Pos, Text_Delay_Source, Retrieve(Stream_Video, StreamPos_Last, Video_Delay_Source), true);
                                Fill(Stream_Text, Pos, Text_Delay_Original, Retrieve(Stream_Video, StreamPos_Last, Video_Delay_Original), true);
                                Fill(Stream_Text, Pos, Text_Delay_Original_Settings, Retrieve(Stream_Video, StreamPos_Last, Video_Delay_Original_Settings), true);
                                Fill(Stream_Text, Pos, Text_Delay_Original_Source, Retrieve(Stream_Video, StreamPos_Last, Video_Delay_Original_Source), true);
                            }
                    }
                }

                //Special case: AAC
                if (StreamKind_Last==Stream_Audio
                 && (Retrieve(Stream_Audio, StreamPos_Last, Audio_Format)==__T("AAC")
                  || Retrieve(Stream_Audio, StreamPos_Last, Audio_Format)==__T("MPEG Audio")
                  || Retrieve(Stream_Audio, StreamPos_Last, Audio_Format)==__T("Vorbis")))
                    Clear(Stream_Audio, StreamPos_Last, Audio_BitDepth); //Resolution is not valid for AAC / MPEG Audio / Vorbis

                //Special case: DV with Audio or/and Text in the video stream
                if (StreamKind_Last==Stream_Video && Temp->second.Parser && (Temp->second.Parser->Count_Get(Stream_Audio) || Temp->second.Parser->Count_Get(Stream_Text)))
                {
                    //Video and Audio are together
                    size_t Audio_Count=Temp->second.Parser->Count_Get(Stream_Audio);
                    for (size_t Audio_Pos=0; Audio_Pos<Audio_Count; Audio_Pos++)
                    {
                        Fill_Flush();
                        Stream_Prepare(Stream_Audio);
                        size_t Pos=Count_Get(Stream_Audio)-1;
                        Merge(*Temp->second.Parser, Stream_Audio, Audio_Pos, StreamPos_Last);
                        if (Retrieve(Stream_Audio, Pos, Audio_MuxingMode).empty())
                            Fill(Stream_Audio, Pos, Audio_MuxingMode, Retrieve(Stream_Video, Temp->second.StreamPos, Video_Format), true);
                        else
                            Fill(Stream_Audio, Pos, Audio_MuxingMode, Retrieve(Stream_Video, Temp->second.StreamPos, Video_Format)+__T(" / ")+Retrieve(Stream_Audio, Pos, Audio_MuxingMode), true);
                        Fill(Stream_Audio, Pos, Audio_MuxingMode_MoreInfo, __T("Muxed in Video #")+Ztring().From_Number(Temp->second.StreamPos+1));
                        Fill(Stream_Audio, Pos, Audio_Duration, Retrieve(Stream_Video, Temp->second.StreamPos, Video_Duration));
                        Fill(Stream_Audio, Pos, Audio_StreamSize_Encoded, 0); //Included in the DV stream size
                        Ztring ID=Retrieve(Stream_Audio, Pos, Audio_ID);
                        Fill(Stream_Audio, Pos, Audio_ID, Retrieve(Stream_Video, Temp->second.StreamPos, Video_ID)+__T("-")+ID, true);
                        Fill(Stream_Audio, Pos, "Source", Retrieve(Stream_Video, Temp->second.StreamPos, "Source"));
                        Fill(Stream_Audio, Pos, "Source_Info", Retrieve(Stream_Video, Temp->second.StreamPos, "Source_Info"));
                    }

                    //Video and Text are together
                    size_t Text_Count=Temp->second.Parser->Count_Get(Stream_Text);
                    for (size_t Text_Pos=0; Text_Pos<Text_Count; Text_Pos++)
                    {
                        Fill_Flush();
                        Stream_Prepare(Stream_Text);
                        size_t Pos=Count_Get(Stream_Text)-1;
                        Merge(*Temp->second.Parser, Stream_Text, Text_Pos, StreamPos_Last);
                        if (Retrieve(Stream_Text, Pos, Text_MuxingMode).empty())
                            Fill(Stream_Text, Pos, Text_MuxingMode, Retrieve(Stream_Video, Temp->second.StreamPos, Video_Format), true);
                        else
                            Fill(Stream_Text, Pos, Text_MuxingMode, Retrieve(Stream_Video, Temp->second.StreamPos, Video_Format)+__T(" / ")+Retrieve(Stream_Text, Pos, Text_MuxingMode), true);
                        Fill(Stream_Text, Pos, Text_MuxingMode_MoreInfo, __T("Muxed in Video #")+Ztring().From_Number(Temp->second.StreamPos+1));
                        Fill(Stream_Text, Pos, Text_Duration, Retrieve(Stream_Video, Temp->second.StreamPos, Video_Duration));
                        Fill(Stream_Text, Pos, Text_StreamSize_Encoded, 0); //Included in the DV stream size
                        Ztring ID=Retrieve(Stream_Text, Pos, Text_ID);
                        Fill(Stream_Text, Pos, Text_ID, Retrieve(Stream_Video, Temp->second.StreamPos, Video_ID)+__T("-")+ID, true);
                        Fill(Stream_Text, Pos, "Source", Retrieve(Stream_Video, Temp->second.StreamPos, "Source"));
                        Fill(Stream_Text, Pos, "Source_Info", Retrieve(Stream_Video, Temp->second.StreamPos, "Source_Info"));
                    }

                    StreamKind_Last=Temp->second.StreamKind;
                    StreamPos_Last=Temp->second.StreamPos;
                }
            }
        }

        //External file name specific
        if (Temp->second.MI && Temp->second.MI->Info)
        {
            //Preparing
            StreamKind_Last=Temp->second.StreamKind;
            StreamPos_Last=Temp->second.StreamPos;

            //Hacks - Before
            Ztring CodecID=Retrieve(StreamKind_Last, StreamPos_Last, Fill_Parameter(StreamKind_Last, Generic_CodecID));
            Ztring Source=Retrieve(StreamKind_Last, StreamPos_Last, "Source");
            Ztring Source_Info=Retrieve(StreamKind_Last, StreamPos_Last, "Source_Info");

            Merge(*Temp->second.MI->Info, Temp->second.StreamKind, 0, Temp->second.StreamPos);
            File_Size_Total+=Ztring(Temp->second.MI->Get(Stream_General, 0, General_FileSize)).To_int64u();

            //Hacks - After
            if (CodecID!=Retrieve(StreamKind_Last, StreamPos_Last, Fill_Parameter(StreamKind_Last, Generic_CodecID)))
            {
                if (!CodecID.empty())
                    CodecID+=__T(" / ");
                CodecID+=Retrieve(StreamKind_Last, StreamPos_Last, Fill_Parameter(StreamKind_Last, Generic_CodecID));
                Fill(StreamKind_Last, StreamPos_Last, Fill_Parameter(StreamKind_Last, Generic_CodecID), CodecID, true);
            }
            if (Source!=Retrieve(StreamKind_Last, StreamPos_Last, "Source"))
            {
                Ztring Source_Original=Retrieve(StreamKind_Last, StreamPos_Last, "Source");
                Ztring Source_Original_Info=Retrieve(StreamKind_Last, StreamPos_Last, "Source_Info");
                Fill(StreamKind_Last, StreamPos_Last, "Source", Source, true);
                Fill(StreamKind_Last, StreamPos_Last, "Source_Info", Source_Info, true);
                Fill(StreamKind_Last, StreamPos_Last, "Source_Original", Source_Original, true);
                Fill(StreamKind_Last, StreamPos_Last, "Source_Original_Info", Source_Original_Info, true);
            }

            //Muxing Mode
            Fill(StreamKind_Last, StreamPos_Last, "MuxingMode", Temp->second.MI->Get(Stream_General, 0, General_Format));

            //Special case: DV with Audio or/and Text in the video stream
            if (StreamKind_Last==Stream_Video && Temp->second.MI->Info && (Temp->second.MI->Info->Count_Get(Stream_Audio) || Temp->second.MI->Info->Count_Get(Stream_Text)))
            {
                //Video and Audio are together
                size_t Audio_Count=Temp->second.MI->Info->Count_Get(Stream_Audio);
                for (size_t Audio_Pos=0; Audio_Pos<Audio_Count; Audio_Pos++)
                {
                    Fill_Flush();
                    Stream_Prepare(Stream_Audio);
                    size_t Pos=Count_Get(Stream_Audio)-1;
                    Merge(*Temp->second.MI->Info, Stream_Audio, Audio_Pos, StreamPos_Last);
                    if (Retrieve(Stream_Audio, Pos, Audio_MuxingMode).empty())
                        Fill(Stream_Audio, Pos, Audio_MuxingMode, Retrieve(Stream_Video, Temp->second.StreamPos, Video_Format), true);
                    else
                        Fill(Stream_Audio, Pos, Audio_MuxingMode, Retrieve(Stream_Video, Temp->second.StreamPos, Video_Format)+__T(" / ")+Retrieve(Stream_Audio, Pos, Audio_MuxingMode), true);
                    Fill(Stream_Audio, Pos, Audio_MuxingMode_MoreInfo, __T("Muxed in Video #")+Ztring().From_Number(Temp->second.StreamPos+1));
                    Fill(Stream_Audio, Pos, Audio_Duration, Retrieve(Stream_Video, Temp->second.StreamPos, Video_Duration), true);
                    Fill(Stream_Audio, Pos, Audio_StreamSize_Encoded, 0); //Included in the DV stream size
                    Ztring ID=Retrieve(Stream_Audio, Pos, Audio_ID);
                    Fill(Stream_Audio, Pos, Audio_ID, Retrieve(Stream_Video, Temp->second.StreamPos, Video_ID)+__T("-")+ID, true);
                    Fill(Stream_Audio, Pos, "Source", Retrieve(Stream_Video, Temp->second.StreamPos, "Source"));
                    Fill(Stream_Audio, Pos, "Source_Info", Retrieve(Stream_Video, Temp->second.StreamPos, "Source_Info"));
                }

                //Video and Text are together
                size_t Text_Count=Temp->second.MI->Info->Count_Get(Stream_Text);
                for (size_t Text_Pos=0; Text_Pos<Text_Count; Text_Pos++)
                {
                    Fill_Flush();
                    Stream_Prepare(Stream_Text);
                    size_t Pos=Count_Get(Stream_Text)-1;
                    Merge(*Temp->second.MI->Info, Stream_Text, Text_Pos, StreamPos_Last);
                    if (Retrieve(Stream_Text, Pos, Text_MuxingMode).empty())
                        Fill(Stream_Text, Pos, Text_MuxingMode, Retrieve(Stream_Video, Temp->second.StreamPos, Video_Format), true);
                    else
                        Fill(Stream_Text, Pos, Text_MuxingMode, Retrieve(Stream_Video, Temp->second.StreamPos, Video_Format)+__T(" / ")+Retrieve(Stream_Text, Pos, Text_MuxingMode), true);
                    Fill(Stream_Text, Pos, Text_MuxingMode_MoreInfo, __T("Muxed in Video #")+Ztring().From_Number(Temp->second.StreamPos+1));
                    Fill(Stream_Text, Pos, Text_Duration, Retrieve(Stream_Video, Temp->second.StreamPos, Video_Duration));
                    Fill(Stream_Text, Pos, Text_StreamSize_Encoded, 0); //Included in the DV stream size
                    Ztring ID=Retrieve(Stream_Text, Pos, Text_ID);
                    Fill(Stream_Text, Pos, Text_ID, Retrieve(Stream_Video, Temp->second.StreamPos, Video_ID)+__T("-")+ID, true);
                    Fill(Stream_Text, Pos, "Source", Retrieve(Stream_Video, Temp->second.StreamPos, "Source"));
                    Fill(Stream_Text, Pos, "Source_Info", Retrieve(Stream_Video, Temp->second.StreamPos, "Source_Info"));
                }
            }
        }

        //Aperture size
        if (Temp->second.CleanAperture_Width)
        {
            Ztring CleanAperture_Width=Ztring().From_Number(Temp->second.CleanAperture_Width, 0);
            Ztring CleanAperture_Height=Ztring().From_Number(Temp->second.CleanAperture_Height, 0);
            if (CleanAperture_Width!=Retrieve(Stream_Video, StreamPos_Last, Video_Width))
            {
                Fill(Stream_Video, StreamPos_Last, Video_Width_Original, Retrieve(Stream_Video, StreamPos_Last, Video_Width), true);
                Fill(Stream_Video, StreamPos_Last, Video_Width, Temp->second.CleanAperture_Width, 0, true);
            }
            if (CleanAperture_Height!=Retrieve(Stream_Video, StreamPos_Last, Video_Height))
            {
                Fill(Stream_Video, StreamPos_Last, Video_Height_Original, Retrieve(Stream_Video, StreamPos_Last, Video_Height), true);
                Fill(Stream_Video, StreamPos_Last, Video_Height, Temp->second.CleanAperture_Height, 0, true);
            }
            if (Temp->second.CleanAperture_PixelAspectRatio)
            {
                Clear(Stream_Video, StreamPos_Last, Video_DisplayAspectRatio);
                Clear(Stream_Video, StreamPos_Last, Video_PixelAspectRatio);
                Fill(Stream_Video, StreamPos_Last, Video_PixelAspectRatio, Temp->second.CleanAperture_PixelAspectRatio, 3, true);
                if (Retrieve(Stream_Video, StreamPos_Last, Video_PixelAspectRatio)==Retrieve(Stream_Video, StreamPos_Last, Video_PixelAspectRatio_Original))
                    Clear(Stream_Video, StreamPos_Last, Video_PixelAspectRatio_Original);
                if (Retrieve(Stream_Video, StreamPos_Last, Video_DisplayAspectRatio)==Retrieve(Stream_Video, StreamPos_Last, Video_DisplayAspectRatio_Original))
                {
                    Clear(Stream_Video, StreamPos_Last, Video_DisplayAspectRatio_Original);
                    Clear(Stream_Video, StreamPos_Last, Video_DisplayAspectRatio_Original_String);
                }
            }
        }

        //Special case: QuickTime files and Stereo streams, there is a default value in QuickTime player, a QuickTime "standard"?
        if (StreamKind_Last==Stream_Audio
            && Retrieve(Stream_Audio, StreamPos_Last, Audio_Channel_s_)==__T("2")
            && Retrieve(Stream_Audio, StreamPos_Last, Audio_ChannelLayout).empty()
            && Retrieve(Stream_Audio, StreamPos_Last, Audio_ChannelPositions).empty()
            && Retrieve(Stream_General, 0, General_Format_Profile)==__T("QuickTime"))
        {
            Fill(Stream_Audio, StreamPos_Last, Audio_ChannelPositions, Mpeg4_chan(101));
            Fill(Stream_Audio, StreamPos_Last, Audio_ChannelLayout, Mpeg4_chan_Layout(101));
        }

        ++Temp;
    }
    if (Vendor!=0x00000000 && Vendor!=0xFFFFFFFF)
    {
        Ztring VendorS=Mpeg4_Encoded_Library(Vendor);
        if (!Vendor_Version.empty())
        {
            VendorS+=__T(' ');
            VendorS+=Vendor_Version;
        }
        Fill(Stream_General, 0, General_Encoded_Library, VendorS);
        Fill(Stream_General, 0, General_Encoded_Library_Name, Mpeg4_Encoded_Library(Vendor));
        Fill(Stream_General, 0, General_Encoded_Library_Version, Vendor_Version);
    }

    if (File_Size_Total!=File_Size)
        Fill(Stream_General, 0, General_FileSize, File_Size_Total, 10, true);
    if (Count_Get(Stream_Video)==0 && Count_Get(Stream_Image)==0 && Count_Get(Stream_Audio)>0)
        Fill(Stream_General, 0, General_InternetMediaType, "audio/mp4", Unlimited, true, true);

    //Parsing reference files
    #ifdef MEDIAINFO_REFERENCES_YES
        for (streams::iterator Stream=Streams.begin(); Stream!=Streams.end(); ++Stream)
            if (!Stream->second.File_Name.empty())
            {
                if (ReferenceFiles==NULL)
                    ReferenceFiles=new File__ReferenceFilesHelper(this, Config);

                File__ReferenceFilesHelper::reference ReferenceFile;
                ReferenceFile.FileNames.push_back(Stream->second.File_Name);
                ReferenceFile.StreamKind=Stream->second.StreamKind;
                ReferenceFile.StreamPos=Stream->second.StreamPos;
                ReferenceFile.StreamID=Retrieve(Stream->second.StreamKind, Stream->second.StreamPos, General_ID).To_int64u();
                if (Stream->second.StreamKind==Stream_Video)
                {
                    ReferenceFile.FrameRate=Retrieve(Stream_Video, Stream->second.StreamPos, Video_FrameRate).To_float64();

                    #ifdef MEDIAINFO_IBI_YES
                        for (size_t stss_Pos=0; stss_Pos<Stream->second.stss.size(); stss_Pos++)
                        {
                            int64u Value=Stream->second.stss[stss_Pos];

                            //Searching the corresponding stco
                            std::vector<stream::stsc_struct>::iterator Stsc=Stream->second.stsc.begin();
                            int64u SamplePos=0;
                            for (; Stsc!=Stream->second.stsc.end(); ++Stsc)
                            {
                                std::vector<stream::stsc_struct>::iterator Stsc_Next=Stsc; ++Stsc_Next;
                                int64u CountOfSamples=((Stsc_Next==Stream->second.stsc.end()?Stream->second.stco.size():Stsc_Next->FirstChunk)-Stsc->FirstChunk)*Stsc->SamplesPerChunk;
                                if (Stsc_Next!=Stream->second.stsc.end() && Value>=SamplePos+CountOfSamples)
                                    SamplePos+=CountOfSamples;
                                else
                                {
                                    int64u CountOfChunks=(Value-SamplePos)/Stsc->SamplesPerChunk;
                                    size_t stco_Pos=(size_t)(Stsc->FirstChunk-1+CountOfChunks); //-1 because first chunk is number 1
                                    if (stco_Pos<Stream->second.stco.size())
                                    {
                                        stream::stts_durations::iterator stts_Duration=Stream->second.stts_Durations.begin()+Stream->second.stts_Durations_Pos;
                                        ibi::stream::info IbiInfo;
                                        IbiInfo.StreamOffset=Stream->second.stco[stco_Pos];
                                        IbiInfo.FrameNumber=Value;
                                        IbiInfo.Dts=TimeCode_DtsOffset+(stts_Duration->DTS_Begin+(((int64u)stts_Duration->SampleDuration)*(Value-stts_Duration->Pos_Begin)))*1000000000/Stream->second.mdhd_TimeScale;
                                        ReferenceFile.IbiStream.Add(IbiInfo);
                                    }
                                }
                            }
                        }
                    #endif //MEDIAINFO_IBI_YES

                }
                ReferenceFiles->References.push_back(ReferenceFile);
            }

        if (ReferenceFiles)
        {
            ReferenceFiles->ParseReferences();
            #if MEDIAINFO_NEXTPACKET
                if (Config->NextPacket_Get() && ReferenceFiles && !ReferenceFiles->References.empty())
                {
                    ReferenceFiles_IsParsing=true;
                    return;
                }
            #endif //MEDIAINFO_NEXTPACKET
        }
    #endif //MEDIAINFO_REFERENCES_YES

    //Commercial names
    Streams_Finish_CommercialNames();
}

//---------------------------------------------------------------------------
void File_Mpeg4::Streams_Finish_CommercialNames()
{
    if (Count_Get(Stream_Video)==1)
    {
        Streams_Finish_StreamOnly();
        if (Retrieve(Stream_Video, 0, Video_Format)==__T("DV") && Retrieve(Stream_Video, 0, Video_Format_Commercial)==__T("DVCPRO HD"))
        {
            int32u BitRate=Retrieve(Stream_Video, 0, Video_BitRate).To_int32u();
            int32u BitRate_Max=Retrieve(Stream_Video, 0, Video_BitRate_Maximum).To_int32u();

            if (BitRate_Max && BitRate>=BitRate_Max)
            {
                Clear(Stream_Video, 0, Video_BitRate_Maximum);
                Fill(Stream_Video, 0, Video_BitRate, BitRate_Max, 10, true);
                Fill(Stream_Video, 0, Video_BitRate_Mode, "CBR", Unlimited, true, true);
            }
        }
             if (!Retrieve(Stream_Video, 0, Video_Format_Commercial_IfAny).empty())
        {
            Fill(Stream_General, 0, General_Format_Commercial_IfAny, Retrieve(Stream_Video, 0, Video_Format_Commercial_IfAny));
            Fill(Stream_General, 0, General_Format_Commercial, Retrieve(Stream_General, 0, General_Format)+__T(' ')+Retrieve(Stream_Video, 0, Video_Format_Commercial_IfAny));
        }
        else if (Retrieve(Stream_Video, 0, Video_Format)==__T("MPEG Video") && Retrieve(Stream_Video, 0, Video_Format_Settings_GOP)!=__T("N=1") && Retrieve(Stream_Video, 0, Video_Colorimetry)==__T("4:2:0") && (Retrieve(Stream_Video, 0, Video_BitRate)==__T("18000000") || Retrieve(Stream_Video, 0, Video_BitRate_Nominal)==__T("18000000") || Retrieve(Stream_Video, 0, Video_BitRate_Maximum)==__T("18000000")))
        {
            Fill(Stream_General, 0, General_Format_Commercial_IfAny, "XDCAM EX 18");
            Fill(Stream_Video, 0, Video_Format_Commercial_IfAny, "XDCAM EX 18");
        }
        else if (Retrieve(Stream_Video, 0, Video_Format)==__T("MPEG Video") && Retrieve(Stream_Video, 0, Video_Format_Settings_GOP)!=__T("N=1") && Retrieve(Stream_Video, 0, Video_Colorimetry)==__T("4:2:0") && (Retrieve(Stream_Video, 0, Video_BitRate)==__T("25000000") || Retrieve(Stream_Video, 0, Video_BitRate_Nominal)==__T("25000000") || Retrieve(Stream_Video, 0, Video_BitRate_Maximum)==__T("25000000")))
        {
            Fill(Stream_General, 0, General_Format_Commercial_IfAny, "XDCAM EX 25");
            Fill(Stream_Video, 0, Video_Format_Commercial_IfAny, "XDCAM EX 25");
        }
        else if (Retrieve(Stream_Video, 0, Video_Format)==__T("MPEG Video") && Retrieve(Stream_Video, 0, Video_Format_Settings_GOP)!=__T("N=1") && Retrieve(Stream_Video, 0, Video_Colorimetry)==__T("4:2:0") && (Retrieve(Stream_Video, 0, Video_BitRate)==__T("35000000") || Retrieve(Stream_Video, 0, Video_BitRate_Nominal)==__T("35000000") || Retrieve(Stream_Video, 0, Video_BitRate_Maximum)==__T("35000000")))
        {
            Fill(Stream_General, 0, General_Format_Commercial_IfAny, "XDCAM EX 35");
            Fill(Stream_Video, 0, Video_Format_Commercial_IfAny, "XDCAM EX 35");
        }
        else if (Retrieve(Stream_Video, 0, Video_Format)==__T("MPEG Video") && Retrieve(Stream_Video, 0, Video_Format_Settings_GOP)!=__T("N=1") && Retrieve(Stream_Video, 0, Video_Colorimetry)==__T("4:2:2") && (Retrieve(Stream_Video, 0, Video_BitRate)==__T("50000000") || Retrieve(Stream_Video, 0, Video_BitRate_Nominal)==__T("50000000") || Retrieve(Stream_Video, 0, Video_BitRate_Maximum)==__T("50000000")))
        {
            Fill(Stream_General, 0, General_Format_Commercial_IfAny, "XDCAM HD422");
            Fill(Stream_Video, 0, Video_Format_Commercial_IfAny, "XDCAM HD422");
        }
    }
}

//***************************************************************************
// Buffer - Global
//***************************************************************************

//---------------------------------------------------------------------------
void File_Mpeg4::Read_Buffer_Unsynched()
{
    if (!IsSub && MajorBrand==0x6A703220) //"jp2 "
        return Read_Buffer_Unsynched_OneFramePerFile();

    mdat_Pos_Temp=mdat_Pos.begin();
    while (mdat_Pos_Temp!=mdat_Pos.end() && mdat_Pos_Temp->first<File_GoTo)
        mdat_Pos_Temp++;
    if (mdat_Pos_Temp!=mdat_Pos.end() && mdat_Pos_Temp->first>File_GoTo)
        mdat_Pos_Temp--; //Previous frame
    if (mdat_Pos_Temp==mdat_Pos.end())
    {
        IsParsing_mdat=false;
        return;
    }
    IsParsing_mdat=true;

    #if MEDIAINFO_SEEK
        //Searching the ID of the first stream to be demuxed
        std::map<int32u, stream>::iterator Next_Stream=Streams.end();
        size_t Next_Stream_Stco=(size_t)-1;
        for (std::map<int32u, stream>::iterator Stream=Streams.begin(); Stream!=Streams.end(); ++Stream)
        {
            for (size_t Stco_Pos=0; Stco_Pos<Stream->second.stco.size(); Stco_Pos++)
                if (Stream->second.stco[Stco_Pos]==mdat_Pos_Temp->first)
                {
                    Next_Stream=Stream;
                    Next_Stream_Stco=Stco_Pos;
                    break;
                }
            if (Next_Stream!=Streams.end())
                break;
        }
    #endif //MEDIAINFO_SEEK

    for (std::map<int32u, stream>::iterator Stream=Streams.begin(); Stream!=Streams.end(); ++Stream)
    {
        if (Stream->second.Parser)
            Stream->second.Parser->Open_Buffer_Unsynch();

        #if MEDIAINFO_SEEK
            //Searching the next position for this stream
            int64u StreamOffset=(int64u)-1;
            if (StreamOffset_Jump.empty() || File_GoTo==mdat_Pos.begin()->first)
                StreamOffset=mdat_Pos_Temp->first;
            else if (Next_Stream_Stco!=(size_t)-1)
            {
                //Searching the right place for this stream
                int64u StreamOffset_Temp=Next_Stream->second.stco[Next_Stream_Stco];
                std::map<int64u, int64u>::iterator StreamOffset_Jump_Temp;
                for (;;)
                {
                    StreamOffset_Jump_Temp=StreamOffset_Jump.find(StreamOffset_Temp);
                    if (StreamOffset_Jump_Temp==StreamOffset_Jump.end())
                        break;
                    if (Stream==Next_Stream)
                        StreamOffset_Temp=StreamOffset_Jump_Temp->first;
                    else
                    {
                        ++StreamOffset_Jump_Temp;
                        if (StreamOffset_Jump_Temp==StreamOffset_Jump.end())
                            break;
                        StreamOffset_Temp=StreamOffset_Jump_Temp->second;
                    }

                    if (!Stream->second.stco.empty() && StreamOffset_Temp>=Stream->second.stco[0] && StreamOffset_Temp<=Stream->second.stco[Stream->second.stco.size()-1])
                        for (size_t Stco_Pos=0; Stco_Pos<Stream->second.stco.size(); Stco_Pos++)
                            if (StreamOffset_Temp==Stream->second.stco[Stco_Pos])
                            {
                                StreamOffset=Stream->second.stco[Stco_Pos];
                                break;
                            }

                    if (StreamOffset!=(int64u)-1)
                        break;
                }
            }

            if (StreamOffset!=(int64u)-1)
                for (size_t stco_Pos=0; stco_Pos<Stream->second.stco.size(); stco_Pos++)
                    if (Stream->second.stco[stco_Pos]>=StreamOffset)
                    {
                        //Searching the corresponding frame position
                        std::vector<stream::stsc_struct>::iterator Stsc=Stream->second.stsc.begin();
                        int64u SamplePos=0;
                        for (; Stsc!=Stream->second.stsc.end();  ++Stsc)
                        {
                            std::vector<stream::stsc_struct>::iterator Stsc_Next=Stsc; ++Stsc_Next;
                            if (Stsc_Next!=Stream->second.stsc.end() && stco_Pos+1>=Stsc_Next->FirstChunk)
                            {
                                int64u CountOfSamples=(Stsc_Next->FirstChunk-Stsc->FirstChunk)*Stsc->SamplesPerChunk;
                                SamplePos+=CountOfSamples;
                            }
                            else
                            {
                                int64u CountOfSamples=(stco_Pos+1-Stsc->FirstChunk)*Stsc->SamplesPerChunk;
                                SamplePos+=CountOfSamples;

                                Stream->second.stts_FramePos=SamplePos;

                                //Searching the corresponding duration block position
                                for (stream::stts_durations::iterator Stts_Duration=Stream->second.stts_Durations.begin(); Stts_Duration!=Stream->second.stts_Durations.end(); ++Stts_Duration)
                                    if (SamplePos>=Stts_Duration->Pos_Begin && SamplePos<Stts_Duration->Pos_End)
                                    {
                                        Stream->second.stts_Durations_Pos=Stts_Duration-Stream->second.stts_Durations.begin();
                                        break;
                                    }

                                break;
                            }
                        }

                        break;
                    }
        #endif //MEDIAINFO_SEEK
    }
}

//---------------------------------------------------------------------------
#if MEDIAINFO_SEEK
size_t File_Mpeg4::Read_Buffer_Seek (size_t Method, int64u Value, int64u ID)
{
    if (ReferenceFiles)
        return ReferenceFiles->Read_Buffer_Seek(Method, Value, ID);
    if (!IsSub && MajorBrand==0x6A703220) //"jp2 "
        return Read_Buffer_Seek_OneFramePerFile(Method, Value, ID);

    //Parsing
    switch (Method)
    {
        case 0  :
                    if (Value==0)
                        return Read_Buffer_Seek(3, 0, ID);

                    if (FirstMoovPos==(int64u)-1)
                        return 6;     //Internal error

                    if (Value>=LastMdatPos)
                    {
                        GoTo(File_Size);
                        Open_Buffer_Unsynch();
                        return 1;
                    }

                    {
                        //Looking for the minimal stream offset, for every video/audio/text stream
                        int64u JumpTo=File_Size;
                        for (std::map<int32u, stream>::iterator Stream=Streams.begin(); Stream!=Streams.end(); ++Stream)
                            switch (Stream->second.StreamKind)
                            {
                                case Stream_Video :
                                case Stream_Audio :
                                case Stream_Text  :
                                                    {
                                                        //Searching the corresponding chunk offset
                                                        std::vector<int64u>::iterator Stco=Stream->second.stco.begin();
                                                        if (Value<*Stco)
                                                            return Read_Buffer_Seek(3, 0, ID);

                                                        for (; Stco!=Stream->second.stco.end(); ++Stco)
                                                        {
                                                            std::vector<int64u>::iterator Stco_Next=Stco; ++Stco_Next;
                                                            if (Stco_Next!=Stream->second.stco.end() && Value>=*Stco && Value<*Stco_Next)
                                                            {
                                                                if (JumpTo>*Stco)
                                                                    JumpTo=*Stco;
                                                                break;
                                                            }
                                                        }
                                                    }
                                                    break;
                                default           : ;
                            }

                        GoTo(JumpTo);
                        Open_Buffer_Unsynch();
                        return 1;
                    }
        case 1  :
                    if (Value==0)
                        return Read_Buffer_Seek(3, 0, ID);

                    if (FirstMoovPos==(int64u)-1)
                        return 6;     //Internal error

                    return Read_Buffer_Seek(0, FirstMdatPos+(LastMdatPos-FirstMdatPos)*Value/10000, ID);
        case 2  :   //Timestamp
                    {
                        Value=Value*Stream->second.mdhd_TimeScale/1000000000; //Transformed in mpeg4 ticks
                        if (Value>TimeCode_FrameOffset*Stream->second.stts_Duration/Stream->second.stts_FrameCount) //Removing Time Code offset
                            Value-=TimeCode_FrameOffset*Stream->second.stts_Duration/Stream->second.stts_FrameCount;

                        //Looking for the minimal stream offset, for every video/audio/text stream
                        int64u JumpTo=File_Size;
                        for (std::map<int32u, stream>::iterator Stream=Streams.begin(); Stream!=Streams.end(); ++Stream)
                            switch (Stream->second.StreamKind)
                            {
                                case Stream_Video :
                                case Stream_Audio :
                                case Stream_Text  :
                                                    {
                                                        //Searching the corresponding frame
                                                        for (stream::stts_durations::iterator stts_Duration=Stream->second.stts_Durations.begin(); stts_Duration!=Stream->second.stts_Durations.end(); ++stts_Duration)
                                                        {
                                                            if (Value>=stts_Duration->DTS_Begin && Value<stts_Duration->DTS_End)
                                                            {
                                                                int64u FrameNumber=stts_Duration->Pos_Begin+(Value-stts_Duration->DTS_Begin)/stts_Duration->SampleDuration;

                                                                //Searching the I-Frame
                                                                if (!Stream->second.stss.empty())
                                                                {
                                                                    for (size_t Pos=0; Pos<Stream->second.stss.size(); Pos++)
                                                                        if (FrameNumber<=Stream->second.stss[Pos])
                                                                        {
                                                                            if (Pos && FrameNumber<Stream->second.stss[Pos])
                                                                                FrameNumber=Stream->second.stss[Pos-1];
                                                                            break;
                                                                        }
                                                                }

                                                                //Searching the corresponding stco
                                                                std::vector<stream::stsc_struct>::iterator Stsc=Stream->second.stsc.begin();
                                                                int64u SamplePos=0;
                                                                for (; Stsc!=Stream->second.stsc.end(); ++Stsc)
                                                                {
                                                                    std::vector<stream::stsc_struct>::iterator Stsc_Next=Stsc; ++Stsc_Next;
                                                                    int64u CountOfSamples=((Stsc_Next==Stream->second.stsc.end()?Stream->second.stco.size():Stsc_Next->FirstChunk)-Stsc->FirstChunk)*Stsc->SamplesPerChunk;
                                                                    if (Stsc_Next!=Stream->second.stsc.end() && FrameNumber>=SamplePos+CountOfSamples)
                                                                        SamplePos+=CountOfSamples;
                                                                    else
                                                                    {
                                                                        int64u CountOfChunks=(FrameNumber-SamplePos)/Stsc->SamplesPerChunk;
                                                                        size_t stco_Pos=(size_t)(Stsc->FirstChunk-1+CountOfChunks); //-1 because first chunk is number 1
                                                                        if (stco_Pos>Stream->second.stco.size())
                                                                            return 2; //Invalid FrameNumber
                                                                        if (JumpTo>Stream->second.stco[stco_Pos])
                                                                            JumpTo=Stream->second.stco[stco_Pos];
                                                                        break;
                                                                    }
                                                                }

                                                            }
                                                        }

                                                    }
                                                    break;
                                default           : ;
                            }

                        GoTo(JumpTo);
                        Open_Buffer_Unsynch();
                        return 1;
                    }
        case 3  :
                    //FrameNumber
                    {
                        //Looking for video stream
                        std::map<int32u, stream>::iterator Stream;
                        for (Stream=Streams.begin(); Stream!=Streams.end(); ++Stream)
                            if (Stream->second.StreamKind==Stream_Video)
                                break;
                        if (Stream==Streams.end())
                            for (Stream=Streams.begin(); Stream!=Streams.end(); ++Stream)
                                if (Stream->second.StreamKind==Stream_Audio)
                                    break;
                        if (Stream==Streams.end())
                            return 0; //Not supported

                        //Searching the I-Frame
                        if (!Stream->second.stss.empty())
                        {
                            for (size_t Pos=0; Pos<Stream->second.stss.size(); Pos++)
                                if (Value<=Stream->second.stss[Pos])
                                {
                                    if (Pos && Value<Stream->second.stss[Pos])
                                        Value=Stream->second.stss[Pos-1];
                                    break;
                                }
                        }

                        //Searching the corresponding stco
                        std::vector<stream::stsc_struct>::iterator Stsc=Stream->second.stsc.begin();
                        int64u SamplePos=0;
                        for (; Stsc!=Stream->second.stsc.end(); ++Stsc)
                        {
                            std::vector<stream::stsc_struct>::iterator Stsc_Next=Stsc; ++Stsc_Next;
                            int64u CountOfSamples=((Stsc_Next==Stream->second.stsc.end()?Stream->second.stco.size():Stsc_Next->FirstChunk)-Stsc->FirstChunk)*Stsc->SamplesPerChunk;
                            if (Stsc_Next!=Stream->second.stsc.end() && Value>=SamplePos+CountOfSamples)
                                SamplePos+=CountOfSamples;
                            else
                            {
                                int64u CountOfChunks=(Value-SamplePos)/Stsc->SamplesPerChunk;
                                size_t stco_Pos=(size_t)(Stsc->FirstChunk-1+CountOfChunks); //-1 because first chunk is number 1
                                if (stco_Pos>Stream->second.stco.size())
                                    return 2; //Invalid value
                                int64u Offset=Stream->second.stco[stco_Pos];

                                //Seeking back to audio/text frames before this video frame
                                if (!StreamOffset_Jump.empty())
                                {
                                    if (stco_Pos==0) //The first Stco is considered as the last byte of previous stram
                                    {
                                        if (!mdat_Pos.empty())
                                            Offset=mdat_Pos.begin()->first;
                                    }
                                    else
                                    {
                                        std::map<int64u, int64u>::iterator StreamOffset_Jump_Temp=StreamOffset_Jump.find(Stream->second.stco[stco_Pos]);
                                        if (StreamOffset_Jump_Temp!=StreamOffset_Jump.end())
                                            Offset=StreamOffset_Jump_Temp->second;
                                    }
                                }
                                else
                                {
                                    //TODO
                                }

                                GoTo(Offset);
                                Open_Buffer_Unsynch();
                                return 1;
                            }
                        }

                        return 2; //Invalid value
                    }
        default :   return 0;
    }
}
#endif //MEDIAINFO_SEEK

//***************************************************************************
// Buffer
//***************************************************************************

//---------------------------------------------------------------------------
bool File_Mpeg4::Header_Begin()
{
    #if MEDIAINFO_DEMUX
        //Handling of multiple frames in one block
        if (IsParsing_mdat && Config->Demux_Unpacketize_Get())
        {
            Open_Buffer_Continue(Streams[(int32u)Element_Code].Parser, Buffer+Buffer_Offset, 0);
            if (Config->Demux_EventWasSent)
                return false;
        }
    #endif //MEDIAINFO_DEMUX

    if (IsParsing_mdat && Element_Level==0)
        Element_Begin0();

    return true;
}

//---------------------------------------------------------------------------
void File_Mpeg4::Header_Parse()
{
    //mdat
    if (IsParsing_mdat)
    {
        //Positionning
        if (File_Offset+Buffer_Offset<mdat_Pos_Temp->first)
        {
            if (File_Offset+Buffer_Size>mdat_Pos_Temp->first)
            {
                Buffer_Offset=(size_t)(File_Offset+Buffer_Size-mdat_Pos_Temp->first);
            }
            else
            {
                Buffer_Offset=Buffer_Size;
                return;
            }
        }

        //Filling
        Header_Fill_Code(mdat_Pos_Temp->second.StreamID, Ztring::ToZtring(mdat_Pos_Temp->second.StreamID));
        Header_Fill_Size(mdat_Pos_Temp->second.Size);
        if (Buffer_Offset+mdat_Pos_Temp->second.Size<=Buffer_Size) //Only if we will not need it later (in case of partial data, this function will be called again for the same chunk)
        {
            mdat_Pos_Temp++;
            while (mdat_Pos_Temp!=mdat_Pos.end())
            {
                if (mdat_Pos_NormalParsing && !Streams[mdat_Pos_Temp->second.StreamID].IsPriorityStream)
                    break;
                if (!mdat_Pos_NormalParsing && Streams[mdat_Pos_Temp->second.StreamID].IsPriorityStream)
                    break;
                mdat_Pos_Temp++;
            }
        }
        else
            Element_WaitForMoreData();
        return;
    }

    //Parsing
    int64u Size;
    int32u Size_32, Name;
    Get_B4 (Size_32,                                            "Size");
    if (Size_32==0 && (Element_Size==4 || Element_Size==8))
    {
        //Filling
        Header_Fill_Code(0, "Junk");
        Header_Fill_Size(4);
        return;
    }
    Size=Size_32;
    Get_C4 (Name,                                               "Name");
    if (Name==0x33647666) //3dvf
        Name=0x6D6F6F76; //moov
    if (Name==0x61766964) //avid
        Name=0x6D646174; //mdat

    if (Size<8)
    {
        //Special case: until the end of the atom
            if (Size==0)
        {
            Size=Config->File_Current_Size-(File_Offset+Buffer_Offset);
            if (Status[IsAccepted] && Element_Level==2 && Name==0x00000000) //First real level (Level 1 is atom, level 2 is header block)
            {
                Element_Offset=0;
                Name=Elements::mdat;
            }
        }
        //Special case: Big files, size is 64-bit
        else if (Size==1)
        {
            //Reading Extended size
            Get_B8 (Size,                                       "Size (Extended)");
        }
        //Not in specs!
        else
        {
            Size=Config->File_Current_Size-(File_Offset+Buffer_Offset);
        }
    }

    //Specific case: file begin with "free" atom
    if (!Status[IsAccepted]
     && (Name==Elements::free
      || Name==Elements::skip
      || Name==Elements::wide))
    {
        Accept("MPEG-4");
        Fill(Stream_General, 0, General_Format, "QuickTime");
    }

    //Filling
    Header_Fill_Code(Name, Ztring().From_CC4(Name));
    Header_Fill_Size(Size);
}

//---------------------------------------------------------------------------
struct Mpeg4_muxing
{
    int64u  MinimalOffset;
    int64u  MaximalOffset;

    Mpeg4_muxing()
    {
        MinimalOffset=(int64u)-1;
        MaximalOffset=0;
    }
};
bool File_Mpeg4::BookMark_Needed()
{
    if (!mdat_MustParse)
        return false;

    //Handling of some wrong stsz and stsc atoms (ADPCM)
    if (!IsSecondPass)
        for (std::map<int32u, stream>::iterator Temp=Streams.begin(); Temp!=Streams.end(); ++Temp)
            if (Temp->second.StreamKind==Stream_Audio
             && (Retrieve(Stream_Audio, Temp->second.StreamPos, Audio_CodecID)==__T("ima4")
              || Retrieve(Stream_Audio, Temp->second.StreamPos, Audio_CodecID)==__T("11")))
            {
                Temp->second.stsz_StreamSize/=16;
                Temp->second.stsz_StreamSize*=17;
                float32 BitRate_Nominal=Retrieve(Stream_Audio, Temp->second.StreamPos, Audio_BitRate_Nominal).To_float32();
                if (BitRate_Nominal)
                {
                    BitRate_Nominal/=16;
                    BitRate_Nominal*=17;
                    Fill(Stream_Audio, Temp->second.StreamPos, Audio_BitRate_Nominal, BitRate_Nominal, 0, true);
                }
                int64u Channels=Retrieve(Stream_Audio, Temp->second.StreamPos, Audio_Channel_s_).To_int64u();
                if (Channels!=2)
                {
                    Temp->second.stsz_StreamSize/=2;
                    Temp->second.stsz_StreamSize*=Channels;
                }
                for (size_t Pos=0; Pos<Temp->second.stsc.size(); Pos++)
                {
                    Temp->second.stsc[Pos].SamplesPerChunk/=16;
                    Temp->second.stsc[Pos].SamplesPerChunk*=17;
                    if (Channels!=2)
                    {
                        Temp->second.stsc[Pos].SamplesPerChunk/=2;
                        Temp->second.stsc[Pos].SamplesPerChunk*=(int32u)Channels;
                    }
                }
            }

    //In case of second pass
    if (mdat_Pos.empty())
    {
        std::map<int32u, struct Mpeg4_muxing> Muxing; //key is StreamID
        size_t  stco_Count=(size_t)-1;
        bool    stco_IsDifferent=false;

        //For each stream
        for (std::map<int32u, stream>::iterator Temp=Streams.begin(); Temp!=Streams.end(); ++Temp)
        {
            #if MEDIAINFO_DEMUX
                if (Config_Demux && Config->File_Demux_Interleave_Get() && !Temp->second.File_Name.empty())
                {
                    //Remark: supporting both embedded and referenced streams is currently not supported
                    mdat_Pos.clear();
                    return false;
                }
            #endif // MEDIAINFO_DEMUX

            if (Temp->second.Parser && (!Temp->second.stsz.empty() || Temp->second.stsz_Sample_Size))
            {
                if (!stco_IsDifferent)
                {
                    if (stco_Count==(size_t)-1)
                        stco_Count=Temp->second.stco.size();
                    else if (stco_Count!=Temp->second.stco.size())
                        stco_IsDifferent=true;
                }

                size_t stsc_Pos=0;
                size_t stsz_Pos=0;
                size_t Chunk_FrameCount=0;
                int32u Chunk_Number=1;
                int32u Sample_ByteSize=0;
                if (Temp->second.StreamKind==Stream_Audio)
                    Sample_ByteSize=Retrieve(Stream_Audio, Temp->second.StreamPos, Audio_BitDepth).To_int32u()*Retrieve(Stream_Audio, Temp->second.StreamPos, Audio_Channel_s_).To_int32u()/8;

                #if MEDIAINFO_DEMUX
                    stream::stts_durations Temp_stts_Durations;
                #endif //MEDIAINFO_DEMUX
                for (size_t stco_Pos=0; stco_Pos<Temp->second.stco.size(); stco_Pos++)
                {
                    if (Muxing[Temp->first].MinimalOffset>Temp->second.stco[stco_Pos])
                        Muxing[Temp->first].MinimalOffset=Temp->second.stco[stco_Pos];
                    if (Muxing[Temp->first].MaximalOffset<Temp->second.stco[stco_Pos])
                        Muxing[Temp->first].MaximalOffset=Temp->second.stco[stco_Pos];

                    while (stsc_Pos+1<Temp->second.stsc.size() && Chunk_Number>=Temp->second.stsc[stsc_Pos+1].FirstChunk)
                        stsc_Pos++;

                    if (Temp->second.stsz_Sample_Size==0)
                    {
                        //Each sample has its own size
                        int64u Chunk_Offset=0;
                        for (size_t Pos=0; Pos<Temp->second.stsc[stsc_Pos].SamplesPerChunk; Pos++)
                            if (Temp->second.stsz[stsz_Pos])
                            {
                                mdat_Pos[Temp->second.stco[stco_Pos]+Chunk_Offset].StreamID=Temp->first;
                                mdat_Pos[Temp->second.stco[stco_Pos]+Chunk_Offset].Size=Temp->second.stsz[stsz_Pos];
                                Chunk_Offset+=Temp->second.stsz[stsz_Pos];
                                stsz_Pos++;
                                if (stsz_Pos>=Temp->second.stsz.size())
                                break;
                            }
                        if (stsz_Pos>=Temp->second.stsz.size())
                            break;
                    }
                    else if (Temp->second.StreamKind==Stream_Audio && Temp->second.stsz_Sample_Size<=Sample_ByteSize && Temp->second.stsc[stsc_Pos].SamplesPerChunk*Temp->second.stsz_Sample_Size*Temp->second.stsz_Sample_Multiplier<0x1000000)
                    {
                        //Same size per sample, but granularity is too small
                        mdat_Pos[Temp->second.stco[stco_Pos]].StreamID=Temp->first;
                        mdat_Pos[Temp->second.stco[stco_Pos]].Size=Temp->second.stsc[stsc_Pos].SamplesPerChunk*Temp->second.stsz_Sample_Size*Temp->second.stsz_Sample_Multiplier;

                        #if MEDIAINFO_DEMUX
                            if (Temp_stts_Durations.empty() || Temp->second.stsc[stsc_Pos].SamplesPerChunk!=Temp_stts_Durations[Temp_stts_Durations.size()-1].SampleDuration)
                            {
                                stream::stts_duration  stts_Duration;
                                stts_Duration.Pos_Begin=Temp_stts_Durations.empty()?0:Temp_stts_Durations[Temp_stts_Durations.size()-1].Pos_End;
                                stts_Duration.Pos_End=stts_Duration.Pos_Begin+1;
                                stts_Duration.SampleDuration=Temp->second.stsc[stsc_Pos].SamplesPerChunk;
                                stts_Duration.DTS_Begin=Temp_stts_Durations.empty()?0:Temp_stts_Durations[Temp_stts_Durations.size()-1].DTS_End;
                                stts_Duration.DTS_End=stts_Duration.DTS_Begin+stts_Duration.SampleDuration;
                                Temp_stts_Durations.push_back(stts_Duration);
                                //Temp->second.stsc[stsc_Pos].SamplesPerChunk=1;
                            }
                            else
                            {
                                Temp_stts_Durations[Temp_stts_Durations.size()-1].Pos_End++;
                                Temp_stts_Durations[Temp_stts_Durations.size()-1].DTS_End+=Temp_stts_Durations[Temp_stts_Durations.size()-1].SampleDuration;
                            }
                        #endif //MEDIAINFO_DEMUX
                    }
                    else
                    {
                        //Same size per sample
                        int64u Chunk_Offset=0;
                        for (size_t Pos=0; Pos<Temp->second.stsc[stsc_Pos].SamplesPerChunk; Pos++)
                            if (Temp->second.stsz_Sample_Size*Temp->second.stsz_Sample_Multiplier)
                            {
                                mdat_Pos[Temp->second.stco[stco_Pos]+Chunk_Offset].StreamID=Temp->first;
                                mdat_Pos[Temp->second.stco[stco_Pos]+Chunk_Offset].Size=Temp->second.stsz_Sample_Size*Temp->second.stsz_Sample_Multiplier;
                                Chunk_Offset+=Temp->second.stsz_Sample_Size*Temp->second.stsz_Sample_Multiplier;
                                Chunk_FrameCount++;
                            }
                        if (Config->ParseSpeed<1.0 && Chunk_FrameCount>=300)
                            break;
                    }

                    Chunk_Number++;
                }
                Temp->second.Parser->Stream_BitRateFromContainer=Temp->second.stsz_StreamSize*8/(((float64)Temp->second.stts_Duration)/Temp->second.mdhd_TimeScale);
                #if MEDIAINFO_DEMUX
                    if (!Temp_stts_Durations.empty())
                    {
                       Temp->second.stts_Durations=Temp_stts_Durations;
                        for (stsc_Pos=0; stsc_Pos<Temp->second.stsc.size(); stsc_Pos++)
                            Temp->second.stsc[stsc_Pos].SamplesPerChunk=1;
                        Temp->second.stts_FrameCount=Temp_stts_Durations[Temp_stts_Durations.size()-1].Pos_End;
                    }
                #endif //MEDIAINFO_DEMUX
            }
        }
        mdat_Pos_Temp=mdat_Pos.begin();

        #if MEDIAINFO_DEMUX
            if (!stco_IsDifferent && Muxing.size()==2)
            {
                std::map<int32u, struct Mpeg4_muxing>::iterator Muxing_1=Muxing.begin();
                std::map<int32u, struct Mpeg4_muxing>::iterator Muxing_2=Muxing.begin(); ++Muxing_2;
                if (Muxing_1->second.MaximalOffset>Muxing_2->second.MinimalOffset)
                    swap(Muxing_1, Muxing_2);
                if (Muxing_1->second.MaximalOffset<=Muxing_2->second.MinimalOffset)
                {
                    for (size_t stco_Pos=1; stco_Pos<stco_Count; stco_Pos++)
                    {
                        StreamOffset_Jump[Streams[Muxing_1->first].stco[stco_Pos]]=Streams[Muxing_2->first].stco[stco_Pos-1];
                        StreamOffset_Jump[Streams[Muxing_2->first].stco[stco_Pos]]=Streams[Muxing_1->first].stco[stco_Pos];
                    }
                    StreamOffset_Jump[Streams[Muxing_2->first].stco[0]]=Streams[Muxing_2->first].stco[stco_Count-1];
                }
            }
        #endif //MEDIAINFO_DEMUX
    }
    if (mdat_Pos.empty())
        return false;

    IsParsing_mdat=false;
    if (!mdat_Pos_ToParseInPriority_StreamIDs.empty())
    {
        //Hanlding StreamIDs to parse in priority (currently, only the first block of each stream is parsed in priority)
        if (!Streams[mdat_Pos_ToParseInPriority_StreamIDs[0]].stco.empty())
        {
            mdat_pos::iterator Temp=mdat_Pos.find(Streams[mdat_Pos_ToParseInPriority_StreamIDs[0]].stco[0]);
            if (Temp!=mdat_Pos.end() && Temp->first<File_Size) //Skipping data not in a truncated file
            {
                Element_Show();
                while (Element_Level>0)
                    Element_End0();
                Element_Begin1("Priority streams");

                mdat_Pos_Temp=Temp;
                GoTo(Temp->first);
                IsParsing_mdat=true;
            }
        }
        mdat_Pos_ToParseInPriority_StreamIDs.erase(mdat_Pos_ToParseInPriority_StreamIDs.begin());
    }

    if (File_GoTo==(int64u)-1 && !mdat_Pos_NormalParsing)
    {
        Element_Show();
        while (Element_Level>0)
            Element_End0();
        Element_Begin1("Second pass");

        mdat_Pos_Temp=mdat_Pos.begin();
        GoTo(mdat_Pos_Temp->first);
        IsParsing_mdat=true;
        mdat_Pos_NormalParsing=true;
    }

    IsSecondPass=true;
    return false; //We do not want to use the bookmark feature, only detect the end of the file
}

//---------------------------------------------------------------------------
//Get language string from 2CC
Ztring File_Mpeg4::Language_Get(int16u Language)
{
    if (Language==0x7FFF || Language==0xFFFF)
        return Ztring();

    if (Language<0x100)
        return Mpeg4_Language_Apple(Language);

    Ztring ToReturn;
    ToReturn.append(1, (Char)((Language>>10&0x1F)+0x60));
    ToReturn.append(1, (Char)((Language>> 5&0x1F)+0x60));
    ToReturn.append(1, (Char)((Language>> 0&0x1F)+0x60));
    return ToReturn;
}

//---------------------------------------------------------------------------
//Get Metadata definition from 4CC
File_Mpeg4::method File_Mpeg4::Metadata_Get(std::string &Parameter, int64u Meta)
{
    File_Mpeg4::method Method;
    switch (Meta)
    {
        //http://atomicparsley.sourceforge.net/mpeg-4files.html
        case Elements::moov_meta___ART : Parameter="Performer"; Method=Method_String; break;
        case Elements::moov_meta___alb : Parameter="Album"; Method=Method_String; break;
        case Elements::moov_meta___aut : Parameter="Performer"; Method=Method_String; break;
        case Elements::moov_meta___cmt : Parameter="Comment"; Method=Method_String; break;
        case Elements::moov_meta___cpy : Parameter="Copyright"; Method=Method_String; break;
        case Elements::moov_meta___day : Parameter="Recorded_Date"; Method=Method_String; break;
        case Elements::moov_meta___des : Parameter="Title/More"; Method=Method_String; break;
        case Elements::moov_meta___dir : Parameter="Director"; Method=Method_String; break;
        case Elements::moov_meta___dis : Parameter="TermsOfUse"; Method=Method_String; break;
        case Elements::moov_meta___edl : Parameter="Tagged_Date"; Method=Method_String; break;
        case Elements::moov_meta___enc : Parameter="Encoded_Application"; Method=Method_String; break;
        case Elements::moov_meta___fmt : Parameter="Origin"; Method=Method_String; break;
        case Elements::moov_meta___gen : Parameter="Genre"; Method=Method_String; break;
        case Elements::moov_meta___grp : Parameter="Grouping"; Method=Method_String; break;
        case Elements::moov_meta___hos : Parameter="HostComputer"; Method=Method_String; break;
        case Elements::moov_meta___inf : Parameter="Title/More"; Method=Method_String; break;
        case Elements::moov_meta___key : Parameter="Keywords"; Method=Method_String; break;
        case Elements::moov_meta___mak : Parameter="Make"; Method=Method_String; break;
        case Elements::moov_meta___mod : Parameter="Model"; Method=Method_String; break;
        case Elements::moov_meta___nam : Parameter="Title"; Method=Method_String3; break;
        case Elements::moov_meta___prd : Parameter="Producer"; Method=Method_String; break;
        case Elements::moov_meta___PRD : Parameter="Product"; Method=Method_String; break;
        case Elements::moov_meta___prf : Parameter="Performer"; Method=Method_String; break;
        case Elements::moov_meta___req : Parameter="Comment"; Method=Method_String; break;
        case Elements::moov_meta___src : Parameter="DistribtedBy"; Method=Method_String; break;
        case Elements::moov_meta___swr : Parameter="Encoded_Application"; Method=Method_String; break;
        case Elements::moov_meta___too : Parameter="Encoded_Application"; Method=Method_String; break;
        case Elements::moov_meta___wrn : Parameter="Warning"; Method=Method_String; break;
        case Elements::moov_meta___wrt : Parameter="Composer"; Method=Method_String; break;
        case Elements::moov_meta__auth : Parameter="Performer"; Method=Method_String2; break;
        case Elements::moov_meta__albm : Parameter="Album"; Method=Method_String2; break; //Has a optional track number after the NULL byte
        case Elements::moov_meta__aART : Parameter="Album/Performer"; Method=Method_String2; break;
        case Elements::moov_meta__cpil : Parameter="Compilation"; Method=Method_Binary; break;
        case Elements::moov_meta__cprt : Parameter="Copyright"; Method=Method_String2; break;
        case Elements::moov_meta__disk : Parameter="Part"; Method=Method_Binary; break;
        case Elements::moov_meta__dscp : Parameter="Title/More"; Method=Method_String2; break;
        case Elements::moov_meta__gnre : Parameter="Genre"; Method=Method_String2; break;
        case Elements::moov_meta__name : Parameter="Title"; Method=Method_String; break;
        case Elements::moov_meta__perf : Parameter="Performer"; Method=Method_String2; break;
        case Elements::moov_meta__pgap : Parameter.clear(); Method=Method_None; break;
        case Elements::moov_meta__titl : Parameter="Title"; Method=Method_String2; break;
        case Elements::moov_meta__tool : Parameter="Encoded_Application"; Method=Method_String3; break;
        case Elements::moov_meta__trkn : Parameter="Track"; Method=Method_Binary; break;
        case Elements::moov_meta__year : Parameter="Recorded_Date"; Method=Method_String2; break;
        case Elements::moov_meta__tmpo : Parameter="BPM"; Method=Method_Binary; break;
        default :
            {
                Parameter.clear();
                Parameter.append(1, (char)((Meta&0xFF000000)>>24));
                Parameter.append(1, (char)((Meta&0x00FF0000)>>16));
                Parameter.append(1, (char)((Meta&0x0000FF00)>> 8));
                Parameter.append(1, (char)((Meta&0x000000FF)>> 0));
                Method=Method_String;
            }
    }

    Ztring Value;
    Value.append(1, (Char)((Meta&0xFF000000)>>24)); //Can not use From_CC4 because there is sometimes the (C) character, not in Ansi 7-bit, so wrongly decoded on UTF-8 systems
    Value.append(1, (Char)((Meta&0x00FF0000)>>16));
    Value.append(1, (Char)((Meta&0x0000FF00)>> 8));
    Value.append(1, (Char)((Meta&0x000000FF)>> 0));
    if (MediaInfoLib::Config.CustomMapping_IsPresent(__T("MP4"), Value))
        Parameter=MediaInfoLib::Config.CustomMapping_Get(__T("MP4"), Value).To_Local();

    return Method;
}

//---------------------------------------------------------------------------
//Get Metadata definition from string
File_Mpeg4::method File_Mpeg4::Metadata_Get(std::string &Parameter, const std::string &Meta)
{
         if (Meta=="com.apple.quicktime.copyright") Parameter="Copyright";
    else if (Meta=="com.apple.quicktime.displayname") Parameter="Title";
    else if (Meta=="DATE") Parameter="Encoded_Date";
    else if (Meta=="iTunNORM") Parameter="";
    else if (Meta=="iTunes_CDDB_IDs") Parameter="";
    else if (Meta=="iTunSMPB") Parameter="";
    else if (Meta=="PERFORMER") Parameter="Performer";
    else if (Meta=="PUBLISHER") Parameter="Publisher";
    else Parameter=Meta;
    return Method_String;
}

//---------------------------------------------------------------------------
void File_Mpeg4::Descriptors()
{
    //Preparing
    File_Mpeg4_Descriptors MI;
    MI.KindOfStream=StreamKind_Last;
    MI.Parser_DoNotFreeIt=true;

    int64u Elemen_Code_Save=Element_Code;
    Element_Code=moov_trak_tkhd_TrackID; //Element_Code is use for stream identifier
    Open_Buffer_Init(&MI);
    Element_Code=Elemen_Code_Save;
    mdat_MustParse=true; //Data is in MDAT

    //Parsing
    Open_Buffer_Continue(&MI);

    //Filling
    Finish(&MI);
    Merge(MI, StreamKind_Last, 0, StreamPos_Last);

    //Special case: AAC
    if (StreamKind_Last==Stream_Audio
     && (Retrieve(Stream_Audio, StreamPos_Last, Audio_Format)==__T("AAC")
      || Retrieve(Stream_Audio, StreamPos_Last, Audio_Format)==__T("MPEG Audio")
      || Retrieve(Stream_Audio, StreamPos_Last, Audio_Format)==__T("Vorbis")))
        Clear(Stream_Audio, StreamPos_Last, Audio_BitDepth); //Resolution is not valid for AAC / MPEG Audio / Vorbis

    //Parser from Descriptor
    if (MI.Parser)
    {
        if (Streams[moov_trak_tkhd_TrackID].Parser)
            delete Streams[moov_trak_tkhd_TrackID].Parser; //Streams[moov_trak_tkhd_TrackID].Parser=NULL
        Streams[moov_trak_tkhd_TrackID].Parser=MI.Parser;
        mdat_MustParse=true;
    }
}

//---------------------------------------------------------------------------
void File_Mpeg4::TimeCode_Associate(int32u TrackID)
{
    //Trying to detect time code attached to 1 video only but for all streams in reality
    int32u TimeCode_TrackID=(int32u)-1;
    bool   TimeCode_TrackID_MoreThanOne=false;
    for (std::map<int32u, stream>::iterator Strea=Streams.begin(); Strea!=Streams.end(); ++Strea)
        if (Strea->second.TimeCode_TrackID!=(int32u)-1)
        {
            if (TimeCode_TrackID==(int32u)-1)
                TimeCode_TrackID=Strea->second.TimeCode_TrackID;
            else
                TimeCode_TrackID_MoreThanOne=true;
        }
    if (!TimeCode_TrackID_MoreThanOne && TimeCode_TrackID!=(int32u)-1)
        for (std::map<int32u, stream>::iterator Strea=Streams.begin(); Strea!=Streams.end(); ++Strea)
            Strea->second.TimeCode_TrackID=TimeCode_TrackID; //For all tracks actually

    //Is it general or for a specific stream?
    bool IsGeneral=true;
    for (std::map<int32u, stream>::iterator Strea=Streams.begin(); Strea!=Streams.end(); ++Strea)
        if (Strea->second.TimeCode_TrackID==TrackID)
            IsGeneral=false;

    //For each track in the file (but only the last one will be used!)
    for (std::map<int32u, stream>::iterator Strea=Streams.begin(); Strea!=Streams.end(); ++Strea)
        if ((IsGeneral && Strea->second.StreamKind!=Stream_Max) || Strea->second.TimeCode_TrackID==TrackID)
        {
            if (Strea->second.StreamKind==Stream_Video)
            {
                Fill(Stream_Video, Strea->second.StreamPos, Video_Delay_Settings, Ztring(__T("DropFrame="))+(Streams[TrackID].TimeCode->DropFrame?__T("Yes"):__T("No")));
                Fill(Stream_Video, Strea->second.StreamPos, Video_Delay_Settings, Ztring(__T("24HourMax="))+(Streams[TrackID].TimeCode->H24?__T("Yes"):__T("No")));
                Fill(Stream_Video, Strea->second.StreamPos, Video_Delay_Settings, Ztring(__T("IsVisual="))+(Streams[TrackID].TimeCode_IsVisual?__T("Yes"):__T("No")));
            }
            Fill(Strea->second.StreamKind, Strea->second.StreamPos, "Delay", Streams[TrackID].Parser->Get(Stream_General, 0, "Delay"));
            Fill(Strea->second.StreamKind, Strea->second.StreamPos, "Delay_Source", "Container");
        }
 }

//***************************************************************************
// C++
//***************************************************************************

} //NameSpace

#endif //MEDIAINFO_MPEG4_YES
