    
########################################################################################
#############                生成 ini-parser 库文件                       ############## 
########################################################################################
LIST (APPEND ini_parser_LIB_SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/dictionary.c
    ${CMAKE_CURRENT_SOURCE_DIR}/iniparser.c
    )
    
    
    
########################################################################################
#############               设置 ini-parser 库版本号                         ############## 
########################################################################################
set(iniparser_LIB_VERSION     "1.0.0")
set(iniparser_LIB_SOVERSION   "1")


   
########################################################################################
#############              添加头文件目录                                 ############## 
########################################################################################
include_directories(  
    ${PROJECT_SOURCE_DIR}/includes  
    ${PROJECT_SOURCE_DIR}/Utils
    ${PROJECT_SOURCE_DIR}/Utils/IniParser
)  

########################################################################################
#############               添加 链接库文件目录                           ############## 
########################################################################################
link_directories(  
    /usr/local/lib   
    )  


    
########################################################################################
#############           生成目标库文件 并 标明版本号                      ############## 
########################################################################################
IF (MODULE_ini-parser)
    #生成动态库  静态库则 STATIC  
    add_library (ini-parserlib  SHARED          ${ini_parser_LIB_SRCS})  
    add_library (ini-parsers    STATIC          ${ini_parser_LIB_SRCS})  
    # 设置版本号，SOVERSION为API版本号
    set_target_properties(ini-parserlib     PROPERTIES 
                                            VERSION ${iniparser_LIB_VERSION} 
                                            SOVERSION ${iniparser_LIB_SOVERSION} )
ELSE (MODULE_ini-parser)
    MESSAGE(STATUS "Not Include ini-parser module.")
ENDIF (MODULE_ini-parser)



########################################################################################
#############                 生成目标库测试文件                          ############## 
########################################################################################
IF (TEST_MODULE_FLAG)
    add_executable(TestIniParser.elf    iniParserTest.c )
    add_dependencies(TestIniParser.elf  ini-parserlib   utilslib    loglib)
    target_link_libraries(TestIniParser.elf     ini-parserlib   utilslib    loglib)

ELSE (TEST_MODULE_FLAG)
    MESSAGE(STATUS "Not Include jsoncpp module.")
ENDIF (TEST_MODULE_FLAG)



