
概述：

最简单的基于FFmpeg的视频播放器
Simplest FFmpeg Player

雷霄骅 Lei Xiaohua
leixiaohua1020@126.com
http://blog.csdn.net/leixiaohua1020

本程序实现了视频文件的解码和显示（支持HEVC，H.264，MPEG2等）。
是最简单的FFmpeg视频解码方面的教程。
通过学习本例子可以了解FFmpeg的解码流程。
项目包含两个工程：
simplest_ffmpeg_player：标准版，FFmpeg学习的开始。
simplest_ffmpeg_player_su：SU（SDL Update）版，加入了简单的SDL的Event。

备注:
标准版在播放视频的时候，画面显示使用延时40ms的方式。这么做有两个后果：
（1）SDL弹出的窗口无法移动，一直显示是忙碌状态
（2）画面显示并不是严格的40ms一帧，因为还没有考虑解码的时间。
SU（SDL Update）版在视频解码的过程中，不再使用延时40ms的方式，而是创建了一个线程，每隔40ms发送一个自定义的消息，告知主函数进行解码显示。这样做之后：
（1）SDL弹出的窗口可以移动了
（2）画面显示是严格的40ms一帧






基本逻辑：
    1、
    2、
    3、
    4、
    5、
    6、
    7、
    8、




文件说明：
    
    
    
    
    
类实现说明：    
    

    
    
    
    
实例化说明：
    
    
    
    
    
    
    
功能使用说明：
    
   




   
    
    