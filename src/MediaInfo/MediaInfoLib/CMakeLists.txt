project(mediainfo)

set(MEDIAINFO_MAJOR_VERSION 0)
set(MEDIAINFO_MINOR_VERSION 7)
set(MEDIAINFO_PATCH_VERSION 53)
set(MEDIAINFO_VERSION ${MEDIAINFO_MAJOR_VERSION}.${MEDIAINFO_MINOR_VERSION}.${MEDIAINFO_PATCH_VERSION})

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../)

include_directories(${ZLIB_INCLUDE_DIR})

add_definitions(-DUNICODE)

set(mediainfo_SRCS
	${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/tinyxml2/tinyxml2.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/MediaInfo/File__Analyze.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/MediaInfo/File__Analyze_Buffer.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/MediaInfo/File__Analyze_Buffer_MinimizeSize.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/MediaInfo/File__Analyze_Streams.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/MediaInfo/File__Analyze_Streams_Finish.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/MediaInfo/File__Base.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/MediaInfo/File__MultipleParsing.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/MediaInfo/File__Duplicate.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/MediaInfo/File_Dummy.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/MediaInfo/File_Other.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/MediaInfo/File_Unknown.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/MediaInfo/MediaInfo.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/MediaInfo/MediaInfo_Config.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/MediaInfo/MediaInfo_Config_Automatic.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/MediaInfo/MediaInfo_Config_MediaInfo.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/MediaInfo/MediaInfo_File.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/MediaInfo/MediaInfo_Inform.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/MediaInfo/MediaInfo_Internal.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/MediaInfo/MediaInfoList.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/MediaInfo/MediaInfoList_Internal.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/MediaInfo/TimeCode.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/MediaInfo/Archive/File_7z.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/MediaInfo/Archive/File_Ace.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/MediaInfo/Archive/File_Bzip2.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/MediaInfo/Archive/File_Elf.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/MediaInfo/Archive/File_Gzip.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/MediaInfo/Archive/File_Iso9660.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/MediaInfo/Archive/File_Mz.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/MediaInfo/Archive/File_Rar.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/MediaInfo/Archive/File_Tar.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/MediaInfo/Archive/File_Zip.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/MediaInfo/Audio/File_Aac.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/MediaInfo/Audio/File_Aac_GeneralAudio.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/MediaInfo/Audio/File_Aac_GeneralAudio_Sbr.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/MediaInfo/Audio/File_Aac_GeneralAudio_Sbr_Ps.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/MediaInfo/Audio/File_Aac_Main.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/MediaInfo/Audio/File_Aac_Others.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/MediaInfo/Audio/File_Ac3.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/MediaInfo/Audio/File_Adpcm.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/MediaInfo/Audio/File_Aes3.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/MediaInfo/Audio/File_Als.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/MediaInfo/Audio/File_Amr.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/MediaInfo/Audio/File_Amv.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/MediaInfo/Audio/File_Ape.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/MediaInfo/Audio/File_Au.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/MediaInfo/Audio/File_Celt.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/MediaInfo/Audio/File_ChannelGrouping.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/MediaInfo/Audio/File_Dts.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/MediaInfo/Audio/File_DolbyE.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/MediaInfo/Audio/File_ExtendedModule.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/MediaInfo/Audio/File_Flac.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/MediaInfo/Audio/File_ImpulseTracker.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/MediaInfo/Audio/File_La.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/MediaInfo/Audio/File_Midi.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/MediaInfo/Audio/File_Module.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/MediaInfo/Audio/File_Mpc.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/MediaInfo/Audio/File_MpcSv8.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/MediaInfo/Audio/File_Mpega.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/MediaInfo/Audio/File_Opus.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/MediaInfo/Audio/File_Pcm.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/MediaInfo/Audio/File_Ps2Audio.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/MediaInfo/Audio/File_Rkau.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/MediaInfo/Audio/File_ScreamTracker3.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/MediaInfo/Audio/File_Speex.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/MediaInfo/Audio/File_Tak.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/MediaInfo/Audio/File_Tta.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/MediaInfo/Audio/File_TwinVQ.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/MediaInfo/Audio/File_Vorbis.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/MediaInfo/Audio/File_Wvpk.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/MediaInfo/Duplicate/File__Duplicate__Base.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/MediaInfo/Duplicate/File__Duplicate__Writer.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/MediaInfo/Duplicate/File__Duplicate_MpegTs.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/MediaInfo/Export/Export_Mpeg7.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/MediaInfo/Export/Export_PBCore.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/MediaInfo/Export/Export_reVTMD.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/MediaInfo/Image/File_Bmp.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/MediaInfo/Image/File_Dpx.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/MediaInfo/Image/File_Exr.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/MediaInfo/Image/File_Gif.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/MediaInfo/Image/File_Ico.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/MediaInfo/Image/File_Jpeg.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/MediaInfo/Image/File_Pcx.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/MediaInfo/Image/File_Png.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/MediaInfo/Image/File_Psd.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/MediaInfo/Image/File_Rle.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/MediaInfo/Image/File_Tiff.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/MediaInfo/Image/File_Tga.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/MediaInfo/Multiple/File__ReferenceFilesHelper.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/MediaInfo/Multiple/File_Ancillary.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/MediaInfo/Multiple/File_Bdav.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/MediaInfo/Multiple/File_Cdxa.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/MediaInfo/Multiple/File_Dcp.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/MediaInfo/Multiple/File_Dpg.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/MediaInfo/Multiple/File_DvDif.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/MediaInfo/Multiple/File_DvDif_Analysis.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/MediaInfo/Multiple/File_Dvdv.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/MediaInfo/Multiple/File_Dxw.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/MediaInfo/Multiple/File_Flv.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/MediaInfo/Multiple/File_Gxf.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/MediaInfo/Multiple/File_Gxf_TimeCode.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/MediaInfo/Multiple/File_Ibi.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/MediaInfo/Multiple/File_Ibi_Create.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/MediaInfo/Multiple/File_Ism.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/MediaInfo/Multiple/File_Ivf.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/MediaInfo/Multiple/File_Lxf.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/MediaInfo/Multiple/File_Mk.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/MediaInfo/Multiple/File_Mpeg4.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/MediaInfo/Multiple/File_Mpeg4_Descriptors.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/MediaInfo/Multiple/File_Mpeg4_Elements.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/MediaInfo/Multiple/File_Mpeg4_TimeCode.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/MediaInfo/Multiple/File_Mpeg_Descriptors.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/MediaInfo/Multiple/File_Mpeg_Psi.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/MediaInfo/Multiple/File_MpegPs.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/MediaInfo/Multiple/File_MpegTs.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/MediaInfo/Multiple/File_MpegTs_Duplicate.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/MediaInfo/Multiple/File_Mxf.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/MediaInfo/Multiple/File_Nut.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/MediaInfo/Multiple/File_Ogg.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/MediaInfo/Multiple/File_Ogg_SubElement.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/MediaInfo/Multiple/File_P2_Clip.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/MediaInfo/Multiple/File_Riff.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/MediaInfo/Multiple/File_Riff_Elements.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/MediaInfo/Multiple/File_Rm.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/MediaInfo/Multiple/File_SequenceInfo.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/MediaInfo/Multiple/File_Skm.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/MediaInfo/Multiple/File_Swf.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/MediaInfo/Multiple/File_Umf.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/MediaInfo/Multiple/File_Wm.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/MediaInfo/Multiple/File_Wm_Elements.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/MediaInfo/Multiple/File_Xdcam_Clip.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/MediaInfo/Reader/Reader_Directory.cpp \
	${CMAKE_CURRENT_SOURCE_DIR}/MediaInfo/Reader/Reader_File.cpp \
	${CMAKE_CURRENT_SOURCE_DIR}/MediaInfo/Reader/Reader_libcurl.cpp \
	${CMAKE_CURRENT_SOURCE_DIR}/MediaInfo/Reader/Reader_libmms.cpp \
	${CMAKE_CURRENT_SOURCE_DIR}/MediaInfo/Tag/File__Tags.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/MediaInfo/Tag/File_ApeTag.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/MediaInfo/Tag/File_Id3.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/MediaInfo/Tag/File_Id3v2.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/MediaInfo/Tag/File_Lyrics3.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/MediaInfo/Tag/File_Lyrics3v2.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/MediaInfo/Tag/File_VorbisCom.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/MediaInfo/Text/File_Cdp.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/MediaInfo/Text/File_Cmml.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/MediaInfo/Text/File_DvbSubtitle.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/MediaInfo/Text/File_DtvccTransport.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/MediaInfo/Text/File_Eia608.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/MediaInfo/Text/File_Eia708.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/MediaInfo/Text/File_Kate.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/MediaInfo/Text/File_N19.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/MediaInfo/Text/File_OtherText.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/MediaInfo/Text/File_Pgs.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/MediaInfo/Text/File_Scc.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/MediaInfo/Text/File_Scte20.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/MediaInfo/Text/File_Teletext.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/MediaInfo/Video/File_Avc.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/MediaInfo/Video/File_Avc_Duplicate.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/MediaInfo/Video/File_AvsV.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/MediaInfo/Video/File_AfdBarData.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/MediaInfo/Video/File_Dirac.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/MediaInfo/Video/File_Flic.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/MediaInfo/Video/File_Fraps.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/MediaInfo/Video/File_H263.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/MediaInfo/Video/File_Lagarith.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/MediaInfo/Video/File_Mpeg4v.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/MediaInfo/Video/File_Mpegv.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/MediaInfo/Video/File_ProRes.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/MediaInfo/Video/File_Theora.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/MediaInfo/Video/File_Vc1.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/MediaInfo/Video/File_Vc3.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/MediaInfo/Video/File_Vp8.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/MediaInfo/Video/File_Y4m.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/MediaInfoDLL/MediaInfoDLL.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/MediaInfoDLL/MediaInfoDLL.def
)

if (WIN32)
	#FIXME MediaInfo.rc should use ${MEDIAINFO_MAJOR_VERSION} and friends
	list(APPEND mediainfo_SRCS
		MediaInfo.rc
	)
endif (WIN32)

add_library(mediainfo SHARED ${mediainfo_SRCS})

target_link_libraries(mediainfo
	zenlib
	${ZLIB_LIBRARY}
)

set_target_properties(mediainfo PROPERTIES
	VERSION ${MEDIAINFO_VERSION}
	SOVERSION ${MEDIAINFO_MAJOR_VERSION}
)

install(TARGETS mediainfo DESTINATION ${INSTALL_TARGETS_DEFAULT_ARGS})