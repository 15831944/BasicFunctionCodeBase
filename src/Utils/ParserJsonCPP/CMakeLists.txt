
########################################################################################
#############               生成 jsoncpp 库文件                           ############## 
########################################################################################
LIST (APPEND jsoncpp_LIB_SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/json/json_reader.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/json/json_value.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/json/json_writer.cpp
#    ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
    )
    

    
########################################################################################
#############               设置 jsoncpp 库版本号                         ############## 
########################################################################################
set(jsoncpp_LIB_VERSION     "1.0.0")
set(jsoncpp_LIB_SOVERSION   "1")


   
########################################################################################
#############              添加头文件目录                                 ############## 
########################################################################################
include_directories(  
    ${PROJECT_SOURCE_DIR}/includes  
    ${PROJECT_SOURCE_DIR}/Utils
    ${PROJECT_SOURCE_DIR}/Utils/ParserJsonCPP/json
    ${PROJECT_SOURCE_DIR}/Utils/ParserJsonCPP/include
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
IF (MODULE_jsoncpp)
    #生成动态库  静态库则 STATIC  
    add_library (jsoncpplib SHARED          ${jsoncpp_LIB_SRCS})  
    add_library (jsoncpps   STATIC          ${jsoncpp_LIB_SRCS})  
    # 设置版本号，SOVERSION为API版本号
    set_target_properties(jsoncpplib    PROPERTIES 
                                        VERSION ${jsoncpp_LIB_VERSION} 
                                        SOVERSION ${jsoncpp_LIB_SOVERSION} )
ELSE (MODULE_jsoncpp)
    MESSAGE(STATUS "Not Include jsoncpp module.")
ENDIF (MODULE_jsoncpp)



########################################################################################
#############                 生成目标库测试文件                          ############## 
########################################################################################
IF (TEST_MODULE_FLAG)
    add_executable(TestJsonCpp.elf  jsonCppTest.cpp)
    add_dependencies(TestJsonCpp.elf    jsoncpplib)
    target_link_libraries(TestJsonCpp.elf   jsoncpplib)

ELSE (TEST_MODULE_FLAG)
    MESSAGE(STATUS "Not Include jsoncpp module.")
ENDIF (TEST_MODULE_FLAG)




