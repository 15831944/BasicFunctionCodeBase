#ifndef __BASIC_PLAYER_WIDGET_H__
#define __BASIC_PLAYER_WIDGET_H__

#include <string>


#ifdef __cplusplus

namespace Player {
using namespace std;

class BasicPlayerWidget {
public:
    BasicPlayerWidget();
    ~BasicPlayerWidget();
    
    
    /** 基本的播放控制 **/
    int AddItemToPlayList();
    int DeleteItemFromPlayList();

    int SortByFileName();
    int SortByFilePath();
    int SortByFileType();
    int SortByFileSize();
    int SortByPlayDuration();
    int SortBySinger();


    


protected:
    
    
private:
    //Program *mProgram;
    /* 不是所有播放实例都可以操作界面元素，应该由主的、全屏的控制。其它的播放实例mUIEnabled == false。*/
    static bool mUIEnabled;
    static uint32_t mUIFlags;
    static uint32_t mUIFlagsForcedMask;
    static uint32_t mUIFlagsForcedValue;



    
};  //end class BasicPlayer



}  //end namespace Player

#endif //__cplusplus

#endif //__BASIC_PLAYER_WIDGET_H__

