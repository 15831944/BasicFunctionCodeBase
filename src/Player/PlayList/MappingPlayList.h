#ifndef __MAPPING_PLAY_LIST_H__
#define __MAPPING_PLAY_LIST_H__

#ifdef __cplusplus

#include <string>
#include <vector>
#include <map>

#include "ThreadMutex.h"

#define     PLAYLIST_FILE_PATH              "./playlists.json"
#define     OUTPUT_PLAYLIST_FILE_PATH       "./output_playlist.list"

#define     Random(x)               (rand() % x)            //通过取余取得指定范围的随机数


/**
========================================================
播放列表中每一条信息的格式
{
    "Name" : [
        {
            "Name" : "",                  //primary key 文件名称
            "Path" : "",                  //文件所在路径
            "Type" : "",                  //文件类型
            "Size" : "",                  //文件大小
            "Duration" : "",              //文件播放时长
            "LastPlayPosition" : "",      //记录上次播放位置
            /// 以上字段是基于playlist文件获得 ///
            
            "AVinfo" : [  //记录音视频信息，json格式
                {
                    "Audio" :[
                        {
                            "BitRate" : "",       //比特率
                            "SamplingRate" : "",  //采样率
                            "Channel" : "",       //声道
                            "Track" : ""
                        }
                    ],
                    "Video" : [
                        {
                            "TotalBitRate" : "",   //总比特率
                            "FrameRate" : "",      //帧速率
                            "FrameSize" : ""       //帧大小（长X宽）
                        }
                    ],
                    "Common" : [
                        {
                            "Title" : "",       //标题
                            "Artist" : "",      //艺术家，歌手
                            "Album" : "",       //专辑
                            "Notes" : ""        //备注
                        }
                    ]
                }
            ]
         
        }
    ],
    
    "Original" : "" //原始条目信息，json格式
}
========================================================
 **/


typedef struct _PlayList{
    std::string Name;               //primary key 文件名称
    std::string Path;               //文件所在路径
    std::string Type;               //文件类型,后缀名
    std::string Size;               //文件大小
    std::string AddTime;            //添加到列表的时间
    std::string Duration;           //文件播放时长
    std::string LastPlayPosition;   //记录上次播放位置
    
    std::string Singer;
    std::string AVinfo; //记录音视频信息，json格式
    int IsCurrentPlaying;  //0- un-playing; 1- current-playing
    int RandomValue;
} PlayListItemS;
 
enum _SortField{
    SortField_Reserved,      //0
    SortField_FileName,
    SortField_FilePath,
    SortField_FileType,
    SortField_FileSize,
    SortField_AddTime,
    SortField_PlayDuration,  //5
    SortField_Singer,
    SortField_Max
};

//按比较项目排序,排序后输出, 
enum _SortOrder{
    SortOrder_Reserved,
    SortOrder_Ascending,    //升序排列 ascending order 
    SortOrder_Descending,   //按降序排 descending order
    SortOrder_Random,
    SortOrder_Max
};



/*
playlist 格式：
        行号索引,Type:类型,FileName:全路径名,LastPlayPosition:上次播放位置

    过程：
        首先，加载playlist，填充PlayListItemS结构体字段（包括：Name、Path、AddTime、LastPlayPosition）；
        然后，通过playlist中的文件名找到该文件；
        接下来，获取该文件属性，补充完整PlayListItemS结构体中的其他字段（包括：Name、Path、Type、Size、AddTime）；
        再接下来，
        
        
    1,type,0,filename:C人初学者.HD1280高清中英双字.mp4
    2,type,0,filename,C人初学者.HD1280高清中英双字.mp4
    3,type,0,filename,【6v电影www.dy131.com】马利和我2小狗岁月BD中英双字1280高清.rmvb
    4,type,0,filename,【6v电影www.dy131.com】马利和我BD中英双字1280高清.rmvb
    5,type,0,filename,海上毒战.El.Ni?o.2014.BDRip.x264.中西字幕-深影字幕组.mp4
    6,type,0,iscurrent,1,filename,[阳光电影www.ygdy8.com].暗夜逐仇.BD.720p.中英双字幕.rmvb
*/

class MappingPlayList { /* 在内存中维护对配置文件中参数的键值对的拷贝 */
    public:
        MappingPlayList();
        ~MappingPlayList();
        
        bool addPlayListFile(std::string file);
        bool deletePlayListFile(std::string file);
        
        int loadPlayListMap();     //将 m_cfgPlayingFileList 中配置文件里的键值对加载到 m_playListMap
        int unloadPlayListMap();
        int savePlayListMapFormating();
        
        int addItemToPlayListMap(PlayListItemS* playListItem);  //在每次将修改信息写入到 m_fileName 文件后，更新内存中的配置文件拷贝 m_paramConfigFileMap ，
        int deleteItemToPlayListMap(std::string& name, PlayListItemS& playListItem);
        int sortPlayListByField(int sortField, int order) ;   //按比较项目排序,排序后输出到另一种容器
        
        int getFirstItem(PlayListItemS& playListItem);
        int getLastItem(PlayListItemS& playListItem);
        int getCurrentPlayingItem(PlayListItemS& playListItem);   //获取当前播放文件
        
        int getNextByItem(std::string name, PlayListItemS& playListItem);
        int getPreviousByItem(std::string name, PlayListItemS& playListItem);
        int getItemByListIndex(int index, PlayListItemS& playListItem);
        
        int formatingListItemOutput();
        int playListMapOutput();
        
    private:
        
        std::map<std::string, PlayListItemS>  m_playListMap;   //保存各个配置文件中的键值对
        std::vector<std::string>    m_cfgPlayingFileList;                  //可能会修改的配置文件列表，在构造时初始化
        std::string         m_outputFileName;
        ThreadMutex_Type    m_mutex;
};

#endif  // __cplusplus





#endif //__MAPPING_PLAY_LIST_H__

