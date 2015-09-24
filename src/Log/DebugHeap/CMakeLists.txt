########################################################################################
##########         生成 debug_heaplib 库文件                                  ########### 
########################################################################################
LIST (APPEND log_LIB_SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/DebugHeap.c
    )



########################################################################################
#############               设置 debug_heaplib 库版本号                    ############## 
########################################################################################
set(debug_heap_LIB_VERSION     "1.0.0")
set(debug_heap_LIB_SOVERSION   "1")



########################################################################################
##########            添加 编译库文件所需的头文件目录                        ########### 
########################################################################################
include_directories(  
    ${PROJECT_SOURCE_DIR}/includes  
    ${PROJECT_SOURCE_DIR}/Log   
    ${PROJECT_SOURCE_DIR}/Log/DebugHeap
)  
  
  
########################################################################################
##########            添加 链接库文件目录                                    ########### 
########################################################################################
link_directories(  
    /usr/local/lib   
    )  


########################################################################################
##########            生成目标库文件 并 标明版本号                           ########### 
########################################################################################
IF (MODULE_debug_heap)
    #生成动态库  静态库则 STATIC  
    add_library (debug_heaplib SHARED          ${log_LIB_SRCS})  
    add_library (debug_heaps STATIC            ${log_LIB_SRCS})  
    
    # 设置版本号，SOVERSION为API版本号
    set_target_properties(debug_heaplib   PROPERTIES 
                            VERSION         ${debug_heap_LIB_VERSION} 
                            SOVERSION       ${debug_heap_LIB_SOVERSION} )
    
ELSE (MODULE_debug_heap)
    MESSAGE(STATUS "Not Include debug heap module.")
ENDIF (MODULE_debug_heap)



########################################################################################
#############                 生成目标库测试文件                          ############## 
########################################################################################
IF (TEST_MODULE_FLAG)
    add_executable(Testdebug_heap.elf    debug_heapTest.c)
    add_dependencies(Testdebug_heap.elf          debug_heaplib )
    target_link_libraries(Testdebug_heap.elf     debug_heaplib )

ELSE (TEST_MODULE_FLAG)
    MESSAGE(STATUS "Not Include jsoncpp module.")
ENDIF (TEST_MODULE_FLAG)



