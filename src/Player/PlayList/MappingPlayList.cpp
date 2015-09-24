
using namespace std;
#include <fstream>
#include <iostream>
#include <algorithm>

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

#include <stdio.h>  
#include <stdlib.h>
#include <time.h> 
#include <sys/types.h>  
#include <sys/stat.h>  

#include "log/LogC.h"
#include "../LogPlayer.h"

#ifdef __cplusplus
};
#endif  // __cplusplus


#include "PlayListUtility.h"

#include "MappingPlayList.h"


MappingPlayList::MappingPlayList()
{
    if (m_mutex)
        playerLogWarning("thread mutex has init.\n");
    m_mutex = ThreadMutexCreate( );
    
    m_outputFileName = OUTPUT_PLAYLIST_FILE_PATH;

}


MappingPlayList::~MappingPlayList()
{
    
    
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////   
bool MappingPlayList::addPlayListFile(std::string file)
{
    if(file.empty()){
        playerLogError("file[%s] cannot empty, ERROR.\n", file.c_str());
        return false;
    }
    m_cfgPlayingFileList.push_back(file);
    
    return true;
}


bool MappingPlayList::deletePlayListFile(std::string file)
{
    if(file.empty()){
        playerLogError("file[%s] cannot empty, ERROR.\n", file.c_str());
        return false;
    }
    m_cfgPlayingFileList.erase(std::find(m_cfgPlayingFileList.begin(), m_cfgPlayingFileList.end(), file));
    
    return true;
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////   
/**
 *  @Func: getValueByTag
 *         ps. :从文件 m_fileName 的每一行中得到所需的字段值
 *  @Param: str, type:: std::string&
 *          tag, type:: const char *
 *  @Return: std::string
 *  
 *  e.g: <2012-Jan-01 08:02:05.521>
 **/
std::string getValueByTag(std::string& str, const char *tag)   
{
    std::string::size_type position;
    std::string value("");
    std::string strTag = tag;
    
    position = str.find(strTag);
    if (position == std::string::npos) {
        playerLogError("Str[%s] cannot find strTag[%s], ERROR.\n", str.c_str(), strTag.c_str());
        return "fail";
    }
    value = str.substr(0, position);
    str = str.substr(std::string(strTag).size() + value.size(), str.size());
    
    return value;
}  


extern "C" {
PlayListItemS ERROR_PARAM_ITEM = {"string error", "string error", "string error", "string error", "string error", "string error"};
std::string gMonthStr[13] = {"Undefined", "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
}

/**
 *  @Func: parseRecordLine
 *         ps. :从文件 m_fileName 的每一行中得到所需的字段值
 *  @Param: strLine, type:: std::string&
 *  @Return: PlayListItemS*, a structure;
 *  
 *  e.g: 
 *       Name[file.mp4] in [/root], AddTime[2012-Jan-01 08:02:05.481], LastPlayPosition[01:02:05.481];
 **/
PlayListItemS* parseRecordLine(std::string& strLine)
{
    playerLogInfo("parseRecordLine.\n");
    PlayListItemS  *playItem = new PlayListItemS();
    
    if(strLine.find("Name[") == std::string::npos){
        playerLogError("parseRecordLine strLine[%s]\n", strLine.c_str());
        return &ERROR_PARAM_ITEM;
    }
    
    strLine = strLine.substr(std::string("Name[ ").size(), strLine.size());
    
    //get Name
    playItem->Name = getValueByTag(strLine, "] in [");
    if (playItem->Name == "fail")
        return &ERROR_PARAM_ITEM;
    
    //get Path
    playItem->Path = getValueByTag(strLine, "], AddTime[");
    if (playItem->Path == "fail")
        return &ERROR_PARAM_ITEM;
    
    //get Type
    std::string tName = playItem->Name;
    getValueByTag(tName, ".");
    playItem->Type = tName;
    
    //get Size
    std::string filepath = playItem->Path + "/" + playItem->Name;    
    std::ifstream fin(filepath.c_str());
    fin.seekg(0, ios::end);         //设置文件指针到文件流的尾部
    long size = fin.tellg();        //读取文件指针的位置
    fin.close();                    //关闭文件流
    playItem->Size = size;  
    
    //get AddTime
    playItem->AddTime = getValueByTag(strLine, "], LastPlayPosition[");
    if (playItem->AddTime == "fail")
        return &ERROR_PARAM_ITEM;
    
    //get LastPlayPosition
    playItem->LastPlayPosition = getValueByTag(strLine, "];");
    if (playItem->LastPlayPosition == "fail")
        return &ERROR_PARAM_ITEM;
        
     
    //get Duration
    playItem->Duration = "error";
    
    //get Singer
    playItem->Singer = "error";
    
    //get AVinfo
    playItem->AVinfo = "error";
    
    //get IsCurrentPlaying
    playItem->IsCurrentPlaying = 0;
    
    //get random value
    srand((int)time(NULL));     //每次执行种子不同，生成不同的随机数
    playItem->RandomValue = Random(10000);
    
 
/*     playerLogVerbose("parseRecordLine playItem.Name[%s] Path[%s]  AddTime[%s] LastPlayPosition[%s].\n", 
                    (playItem->Name).c_str(), (playItem->Path).c_str(), (playItem->AddTime).c_str(), (playItem->LastPlayPosition).c_str()
                    );
*/
    return playItem;
}


/**
 *  @Func: combineRecordLine
 *         ps. :从文件 m_fileName 的每一行中得到所需的字段值
 *  @Param: strLine, type:: std::string&
 *  @Return: PlayListItemS*, a structure;
 *  
 *  e.g: 
 *  Name[file.mp4] in [/root], AddTime[2012-Jan-01 08:02:05.481], LastPlayPosition[01:02:05.481];
 **/
int combineRecordLine(PlayListItemS& playListItem, std::string& line)
{
    
    line += "Name[" + playListItem.Name;
    line += "] in [" + playListItem.Path;
    line += "],AddTime[" + playListItem.AddTime;
    line += "], LastPlayPosition[" + playListItem.LastPlayPosition;
    line.append("];");
    
    playerLogInfo("combineRecordLine, line[%s].\n", line.c_str());
    return 0;
}
/**
 *  @Func: loadPlayListMap
 *         ps. :逐行从文件 config_param_modify.record 加载到 m_playListMap
 *  @Param: 
 *  @Return: int
 *  
 **/
int MappingPlayList::loadPlayListMap()    //将 m_cfgPlayingFileList 中配置文件里的键值对加载到 m_playListMap
{
    std::vector<std::string>::iterator cfgPlayingFileListIt;
    bool fileAvailableLineFlag = false;
    std::string line("");

    playerLogInfo("loadPlayListMap.\n");
    for (cfgPlayingFileListIt = m_cfgPlayingFileList.begin(); cfgPlayingFileListIt != m_cfgPlayingFileList.end(); ++cfgPlayingFileListIt){
        playerLogVerbose("I will load \"%s\".\n", (*cfgPlayingFileListIt).c_str());
        std::ifstream fin((*cfgPlayingFileListIt).c_str(), std::ios::in);
        while(std::getline(fin, line)) {
            if (!fileAvailableLineFlag) {   //清除第一行
                std::string::size_type position = line.find('=');
                if (position != std::string::npos) {
                    std::string tag   = line.substr(0, position);
                    std::string value = line.substr(position + 1);
                    
                    if (tag.compare("MyPlayList") == 0) {
                        fileAvailableLineFlag = true;
                        line.clear();
                        playerLogInfo("File is Available.\n");
                        continue;
                    }
                }
                playerLogWarning("The available information is not found.(%s)\n", line.c_str());
                break;
            }
            playerLogInfo("Start parse line[%s].\n", line.c_str());
            addItemToPlayListMap(parseRecordLine(line)); //逐行添加
            line.clear();
        }
        fin.close();
        playerLogInfo("Load %s ok.\n", (*cfgPlayingFileListIt).c_str());
    }

    return 0;
}


int MappingPlayList::unloadPlayListMap()
{
    if(m_playListMap.empty()){
        playerLogError("unloadModifyRecordParam:: m_playListMap has been empty, ERROR.\n");
        return -1;
    }
    m_playListMap.erase(m_playListMap.begin(), m_playListMap.end());
    playerLogVerbose("Has unload ModifyRecord OK.\n");
    
    return 0;
}


int MappingPlayList::savePlayListMapFormating()
{
    playerLogVerbose("savePlayListMapFormating\n");
    std::multimap<std::string, PlayListItemS>::iterator it;
    std::string line("");
    FILE *fp = NULL;
    
    if (ThreadMutexLock(m_mutex))
        playerLogWarning("thread mutex locking error.\n");
    if ((fp = fopen(m_outputFileName.c_str(), "wb")) == NULL) {
        playerLogError("Open file error.(%s)\n", m_outputFileName.c_str());
        return -1;
    }
    for(it = m_playListMap.begin(); it != m_playListMap.end(); ++it){
        combineRecordLine(it->second, line);
        fwrite(line.c_str(), 1, line.length(), fp);
    }
    fclose(fp);
    if (ThreadMutexUnLock(m_mutex))
        playerLogWarning("thread mutex locking error.\n");
    
    return 0;
}


int MappingPlayList::addItemToPlayListMap(PlayListItemS* playListItem)  //在每次将修改信息写入到 m_fileName 文件后，更新内存中的配置文件拷贝 m_paramConfigFileMap ，
{
    if( (playListItem->Name).compare("ERROR") == 0 || (playListItem->Name).empty() ){
        playerLogError("playListItem[%p], Name[%s].\n", playListItem, playListItem->Name.c_str());
        return -1;
    }
    
    std::pair<std::string, PlayListItemS> listItem(playListItem->Name, *playListItem);
    m_playListMap.insert(listItem);
    
    return 0;
}


int MappingPlayList::deleteItemToPlayListMap(std::string& name, PlayListItemS& playListItem)
{
    std::map<std::string, PlayListItemS>::iterator iter = m_playListMap.find(name);
    
    if(iter == m_playListMap.end()){
        playerLogError("Cannot find PlayList Item[%s].\n", name.c_str());
        return -1;
    }
    
    m_playListMap.erase(iter->first);
    
    return 0;
}


/**
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
    int IsCurrentPlaying;
    int RandomValue;
} PlayListItemS;
**/
int cmpFileName(const std::pair<std::string, PlayListItemS> &x, const std::pair<std::string, PlayListItemS> &y)
{
    playerLogDebug("\n");
    return ((x.second).Name).compare((y.second).Name);
}

int cmpFilePath(const std::pair<std::string, PlayListItemS> &x, const std::pair<std::string, PlayListItemS> &y)
{
    playerLogDebug("\n");
    return ((x.second).Path).compare((y.second).Path);
}

int cmpFileType(const std::pair<std::string, PlayListItemS> &x, const std::pair<std::string, PlayListItemS> &y)
{
    playerLogDebug("\n");
    return ((x.second).Type).compare((y.second).Type);
}

int cmpFileSize(const std::pair<std::string, PlayListItemS> &x, const std::pair<std::string, PlayListItemS> &y)
{
    return ((x.second).Size).compare((y.second).Size);
}

int cmpAddTime(const std::pair<std::string, PlayListItemS> &x, const std::pair<std::string, PlayListItemS> &y)
{
    playerLogDebug("\n");
    return ((x.second).AddTime).compare((y.second).AddTime);
}

int cmpPlayDuration(const std::pair<std::string, PlayListItemS> &x, const std::pair<std::string, PlayListItemS> &y)
{
    playerLogDebug("\n");
    return ((x.second).Duration).compare((y.second).Duration);
}

int cmpSinger(const std::pair<std::string, PlayListItemS> &x, const std::pair<std::string, PlayListItemS> &y)
{
    playerLogDebug("\n");
    return ((x.second).Singer).compare((y.second).Singer);
}

int cmpRandomValue(const std::pair<std::string, PlayListItemS> &x, const std::pair<std::string, PlayListItemS> &y)
{
    playerLogDebug("\n");
    return ((x.second).RandomValue - (y.second).RandomValue);
}


int MappingPlayList::sortPlayListByField(int sortField, int order)   //升序排列 ascending order 按降序排 descending order
{
    std::vector< std::pair<std::string, PlayListItemS> > t_vec;
    
    playerLogDebug("\n");
    for(std::map<std::string, PlayListItemS>::iterator iter = m_playListMap.begin(); iter != m_playListMap.end(); iter ++) {
        t_vec.push_back(make_pair(iter->first, iter->second));
    }
    
    switch(order){
    case SortOrder_Ascending:  //升序排列
        switch(sortField){
        case SortField_FileName:
            sort(t_vec.begin(), t_vec.end(), cmpFileName);
            break;
        case SortField_FilePath:
            sort(t_vec.begin(), t_vec.end(), cmpFilePath);
            break;
        case SortField_FileType:
            sort(t_vec.begin(), t_vec.end(), cmpFileType);
            break;
        case SortField_FileSize:
            sort(t_vec.begin(), t_vec.end(), cmpFileSize);
            break;
        case SortField_AddTime:
            sort(t_vec.begin(), t_vec.end(), cmpAddTime);
            break;
        case SortField_PlayDuration:
            sort(t_vec.begin(), t_vec.end(), cmpPlayDuration);
            break;
        case SortField_Singer:
            sort(t_vec.begin(), t_vec.end(), cmpSinger);
            break;
        default:
            break;
        }
        break;
        
    case SortOrder_Descending:  //按降序排
        switch(sortField){
        case SortField_FileName:
            sort(t_vec.end(), t_vec.begin(), cmpFileName);
            break;
        case SortField_FilePath:
            sort(t_vec.end(), t_vec.begin(), cmpFilePath);
            break;
        case SortField_FileType:
            sort(t_vec.end(), t_vec.begin(), cmpFileType);
            break;
        case SortField_FileSize:
            sort(t_vec.end(), t_vec.begin(), cmpFileSize);
            break;
        case SortField_AddTime:
            sort(t_vec.end(), t_vec.begin(), cmpAddTime);
            break;
        case SortField_PlayDuration:
            sort(t_vec.end(), t_vec.begin(), cmpPlayDuration);
            break;
        case SortField_Singer:
            sort(t_vec.end(), t_vec.begin(), cmpSinger);
            break;
        default:
            break;
        } 
        break;
    case SortOrder_Random:  //按随机序排
        sort(t_vec.begin(), t_vec.end(), cmpRandomValue);  //吧随机数按升序排列
        break;
    default:  //其他
        break;
    }
    
    playerLogDebug("\n");
    return 0;
}


int copyPlayListItem(PlayListItemS& playItemOut, PlayListItemS& playItemIn)
{
    playerLogDebug("\n");
    
    playItemOut.Name = playItemIn.Name;
    playItemOut.Path = playItemIn.Path;
    playItemOut.Type = playItemIn.Type;
    playItemOut.Size = playItemIn.Size;
    playItemOut.AddTime = playItemIn.AddTime;
    playItemOut.Duration = playItemIn.Duration;
    playItemOut.LastPlayPosition = playItemIn.LastPlayPosition;
    playItemOut.Singer = playItemIn.Singer;
    playItemOut.AVinfo = playItemIn.AVinfo;

    return 0;
}

/*
    std::string Name;               //primary key 文件名称
    std::string Path;               //文件所在路径
    std::string Type;               //文件类型,后缀名
    std::string Size;               //文件大小
    std::string AddTime;            //添加到列表的时间
    std::string Duration;           //文件播放时长
    std::string LastPlayPosition;   //记录上次播放位置
    
    std::string Singer;
*/
int MappingPlayList::getFirstItem(PlayListItemS& playListItem)
{
    std::map<std::string, PlayListItemS>::iterator iter = m_playListMap.begin();
    
    playerLogDebug("\n");
    copyPlayListItem(playListItem, iter->second);
    
    return 0;
}


int MappingPlayList::getLastItem(PlayListItemS& playListItem)
{
    std::map<std::string, PlayListItemS>::iterator iter = m_playListMap.end();
    
    playerLogDebug("\n");
    iter--;
    copyPlayListItem(playListItem, iter->second);
    
    return 0;
}


int MappingPlayList::getCurrentPlayingItem(PlayListItemS& playListItem)
{
    std::map<std::string, PlayListItemS>::iterator iter = m_playListMap.begin();
    
    playerLogDebug("\n");
    for( ;iter != m_playListMap.end(); iter++){
        if((iter->second).IsCurrentPlaying == 1){
            copyPlayListItem(playListItem, iter->second);
            break;
        }
    }
    if(iter == m_playListMap.end()){
        playerLogError("Can not find Item is Playing.\n");
        return -1;
    }
    
    return 0;
}


int MappingPlayList::getNextByItem(std::string name, PlayListItemS& playListItem)
{
    std::map<std::string, PlayListItemS>::iterator iter = m_playListMap.find(name);
    
    playerLogDebug("\n");
    iter++;
    copyPlayListItem(playListItem, iter->second);

    return 0;
}


int MappingPlayList::getPreviousByItem(std::string name, PlayListItemS& playListItem)
{
    std::map<std::string, PlayListItemS>::iterator iter = m_playListMap.find(name);
    
    playerLogDebug("\n");
    iter--;
    copyPlayListItem(playListItem, iter->second);
    
    return 0;
}



int MappingPlayList::getItemByListIndex(int index, PlayListItemS& playListItem)
{
    std::map<std::string, PlayListItemS>::iterator iter;
    int i = 0;
    
    playerLogDebug("\n");
    if(index <= 0 || index > m_playListMap.size()){
        printf("getItemByListIndex error.\n");
        return -1;
    }
    for(iter = m_playListMap.begin(); iter != m_playListMap.end(); ) {
        i ++;
        iter ++;
        if(index == i){
            copyPlayListItem(playListItem, iter->second);
            break;
        }
    }
    
    return 0;
}




int MappingPlayList::formatingListItemOutput()
{
    playerLogDebug("\n");
    std::map<std::string, PlayListItemS>::iterator iter;
    for(iter = m_playListMap.begin(); iter != m_playListMap.end(); iter ++) {
        ;
    }

    return 0;
}


int MappingPlayList::playListMapOutput()
{
    playerLogDebug("\n");
    std::multimap<std::string, PlayListItemS>::iterator it;
    
    for(it = m_playListMap.begin(); it != m_playListMap.end(); ++it){
        std::cout << "playListMapOutput:: key:"<< it->first << " Name:" << (it->second).Name << " Path:" << (it->second).Path << " Type:" << (it->second).Type << " Size:" << (it->second).Size;
        std::cout << " AddTime: " << (it->second).AddTime << " LastPlayPosition:" << (it->second).LastPlayPosition <<std::endl;
    }
    
    return 0;
}


























