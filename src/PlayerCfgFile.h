#ifndef __PLAYER_CFG_FILE_H__
#define __PLAYER_CFG_FILE_H__


//=================================================================================================
//=================================================================================================


/**
====================================================================================
{
    "version" : "1.2.1.2345",
    "build" : "Thu Jul  9 14:42:32 CST 2015",
    "language" : "Chinese",
    "debug" : [
        {
            "DebugEnable": 1,
            "Log" : [
                {
                   "message" : 4,
                   "threads" : 3,
                   "utils" : 5,
                   "views" : 2,
                   "player" : 4,
                   "entry" : 1,
                }
            ],
            "Management" : [
                {
                   "Type" : 4,
                   "IP" : 3,
                   "UserName" : 5,
                   "UserPasswd" : 2,
                   "CryptoType" : 4,
                }
            ],
            "SystemInfo" : [
                {
                    "Position" : [
                        {
                           "Left" : 55,
                           "Top" : 55,
                           "Width" : 3,
                           "Hight" : 5,
                        }
                    ]
                   "CPU" : 53,      //player所占用cpu比率
                   "MEM" : 39,      //player所占用mem比率
                   "DiskUsage" : 45,        //player所占用disk比率
                }
            ],
            "LanguageCode" : "zh",
        }
    ],
    "net" : [
        {
           "IP" : "192.168.12.112",
           "MAC" : "0C:56:5C:2B:3C:4D",
           "NetMask" : "255.255.255.0",
           "GateWay" : "192.168.12.1",
           "DNS" : "210.72.145.44",
        }
    ],
    "time" : [
        {
           "NTP" : "s1a.time.edu.cn", //210.72.145.44
           "TimeZone" : 8,
           "UTC" : 12,
           "Daylight-saving":
        }
    ],
    "playlist" : [
        {
           "URL" : "file:///210.72.145.44/playlist/20150709.txt"
           "CheckCode" : 12, //md5
           "ListLength" : 1028,
        }
    ],
    "audio" : [
        {
           "Volume" : 55,
           "Mute" : "True",
           "ChannelCurrent" : 3,
           "ChannelCont" : 5,   //LL,LR,RL,RR 
           "TrackCurrent" : 2,
           "TrackCont" : 4,
        }
    ],
    "output" : [
        {
            "BGColor" : "#FF0000",
            "StretchType" : 3,
            "Position" : [
                {
                   "PositionEnable": 1,
                   "Left" : 55,
                   "Top" : 55,
                   "Width" : 3,
                   "Hight" : 5,
                   "MaginTop" : 55,
                   "MaginLeft" : 55,
                   "MaginBottom" : 55,
                   "MaginRight" : 55,
                }
            ]
            "Font" : [
                {
                   "FontEnable": 1,
                   "Type" : 3,
                   "Size" : 55,
                   "Color" : 55,
                   "Hight" : 5,
                }
            ],
            "LangusgeNameEnglish" : "Chinese"
        }
    ],
    
    
    
    

   "SubtitleList" : [
      {
         "PID" : 34548,
         "LanguageCode" : "zh",
         "LangusgeNameEnglish" : "Chinese"
      }
   ]
}
====================================================================================
 **/










#endif  //__PLAYER_CFG_FILE_H__

