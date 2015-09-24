
########################################################################################
#############               生成 sqlite3 库文件                           ############## 
########################################################################################
LIST (APPEND sqlite3_LIB_SRCS
    #${CMAKE_CURRENT_SOURCE_DIR}/sqlite/shell.c
    ${CMAKE_CURRENT_SOURCE_DIR}/sqlite/sqlite3.c
    )
    

    
########################################################################################
#############               设置 sqlite3 库版本号                         ############## 
########################################################################################
set(sqlite3_LIB_VERSION     "3.0.0")
set(sqlite3_LIB_SOVERSION   "1")


   
########################################################################################
#############              添加头文件目录                                 ############## 
########################################################################################
include_directories(  
    ${PROJECT_SOURCE_DIR}/includes  
    ${PROJECT_SOURCE_DIR}/Utils
    ${PROJECT_SOURCE_DIR}/Utils/Sqlite3
    ${PROJECT_SOURCE_DIR}/Utils/Sqlite3/sqlite
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
IF (MODULE_sqlite)
    #生成动态库  静态库则 STATIC  
    add_library (sqlite3lib SHARED      ${sqlite3_LIB_SRCS})  
    add_library (sqlite3s   STATIC      ${sqlite3_LIB_SRCS})  
    
    # 设置依赖关系，如果当前库是最底层的共享库，则不需要设置
    add_dependencies (sqlite3lib   utilslib  dl  pthread)
    add_dependencies (sqlite3s     utilslib  dl  pthread)
    
    # 设置版本号，SOVERSION为API版本号
    set_target_properties(sqlite3lib    PROPERTIES 
                                        VERSION     ${sqlite3_LIB_VERSION} 
                                        SOVERSION   ${sqlite3_LIB_SOVERSION} )
ELSE (MODULE_sqlite)
    MESSAGE(STATUS "Not Include sqlite3 module.")
ENDIF (MODULE_sqlite)



########################################################################################
#############                 生成目标库测试文件                          ############## 
########################################################################################
IF (TEST_MODULE_FLAG)
    add_executable(TestSqlite.elf      sqlite3Test.c)
    add_dependencies(TestSqlite.elf         loglib utilslib sqlite3lib  dl  pthread)
    target_link_libraries(TestSqlite.elf    loglib utilslib sqlite3lib  dl  pthread)

ELSE (TEST_MODULE_FLAG)
    MESSAGE(STATUS "Not Include sqlite3 module.")
ENDIF (TEST_MODULE_FLAG)




