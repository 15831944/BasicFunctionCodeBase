########################################################################################
######################             配置包含的库文件                 ####################
########################################################################################
set (Module_ffmpeg_Version      "0.8.15"    CACHE STRING    "ffmpeg version .")
set (Module_SDL2_Version        "2.0.3"     CACHE STRING    "SDL2 version .")




########################################################################################
####################        一般情况下所 必须 包含的模块组件         ###################
########################################################################################
option  (COMPONENT_player                   "Enable player component."          ON)
option  (COMPONENT_message                  "Enable message component"          ON)
option  (COMPONENT_thread                   "Enable thread component"           ON)
option  (COMPONENT_mediainfo                "Enable media info component"       ON)

option  (COMPONENT_log                      "Enable log component."             ON)
option  (COMPONENT_utils                    "Enable utils component."           ON)
option  (COMPONENT_monitor                  "Enable monitor component"          ON)
option  (COMPONENT_settings                 "Enable settings component"         ON)
option  (COMPONENT_network                  "Enable network component"          ON)
option  (COMPONENT_crypto                   "Enable crypto component"           ON)
option  (COMPONENT_views                    "Enable views component."           ON)




########################################################################################
##################               配置所包含的功能模块                 ##################
########################################################################################
## log
option  (MODULE_debug_heap              "Enable debug heap module"                  ON)
option  (MODULE_stackinfo               "Enable function stackinfo module"          ON)
option  (MODULE_functions_statistics    "Enable functions_statistics module"        ON)

## utils
option  (MODULE_jsoncpp                 "Enable json module"                        ON)
option  (MODULE_ini-parser              "Enable ini-parser module"                  ON)
option  (MODULE_xml                     "Enable xml module"                         ON)
option  (MODULE_sqlite                  "Enable sqlite3 module"                     ON)

## monitors
option  (MODULE_ftpserver               "Enable monitor ftp server module"          ON)
option  (MODULE_ftpclient               "Enable monitor ftp client module"          ON)
option  (MODULE_system_monitor          "Enable system monitor module"              ON)

## settings
option  (MODULE_modifykeeping           "Enable modify record keeping module"       ON)

## network
option  (MODULE_tinytcp                "Enable tiny tcp module"                     OFF)
option  (MODULE_arp                    "Enable arp module"                          OFF)
option  (MODULE_rarp                   "Enable rarp module"                         OFF)
option  (MODULE_dhcp                   "Enable dhcp module"                         OFF)
option  (MODULE_dns                    "Enable dns module"                          OFF)
option  (MODULE_adns                   "Enable adns module"                         OFF)
option  (MODULE_sntp                   "Enable sntp module"                         OFF)

## crypto
option  (MODULE_base64                 "Enable base64 crypto module"                OFF)
option  (MODULE_AES                    "Enable AES crypto module"                   OFF)
option  (MODULE_DES                    "Enable DES crypto module"                   OFF)
option  (MODULE_MD5                    "Enable MD5 crypto module"                   OFF)
option  (MODULE_RSA                    "Enable RSA crypto module"                   OFF)
option  (MODULE_SHA128                 "Enable SHA128 crypto module"                OFF)
option  (MODULE_SHA256                 "Enable SHA256 crypto module"                OFF)

## views
option  (MODULE_gif                     "Enable gif codec module"                   ON)
option  (MODULE_tinyjpeg                "Enable tiny jpeg codec module"             ON)
option  (MODULE_lodepng                 "Enable lodepng codec module"               ON)
option  (MODULE_easybmp                 "Enable easybmp codec module"               ON)

## xmpp
option  (MODULE_xmpp                    "Enable xmpp IM module"                     ON)


## independent module
option  (APPLICATIONS_astyle                  "Enable astyle module"                ON)
option  (APPLICATIONS_weather                 "Enable weather APP module"           ON)



#option  (MODULE_                         "Enable xml"                        OFF)



########################################################################################
##################            功能模块 or 组件 测试标志位             ##################
########################################################################################
option  (TEST_MODULE_FLAG                   "Enable module test"                ON)









