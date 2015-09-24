
########################################################################################
#############               生成 Crypto 库文件                           ############## 
########################################################################################
LIST (APPEND crypto_LIB_SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/logCrypto.c
    ${CMAKE_CURRENT_SOURCE_DIR}/CryptoInterface.c
    )
    

    
########################################################################################
#############               添加源码子目录                                ############## 
########################################################################################
#add_subdirectory( AES )
add_subdirectory( Base64 )
#add_subdirectory( DES )
#add_subdirectory( MD5 )
#add_subdirectory( RSA )
#add_subdirectory( SHA1 )
#add_subdirectory( SHA256 )

MESSAGE("${CMAKE_CURRENT_SOURCE_DIR} status.")



########################################################################################
#############               设置 crypto 库版本号                         ############## 
########################################################################################
set(crypto_LIB_VERSION     "1.0.0")
set(crypto_LIB_SOVERSION   "1")


    
########################################################################################
#############           生成目标库文件 并 标明版本号                      ############## 
########################################################################################
IF (COMPONENT_crypto)
    #生成动态库  静态库则 STATIC  
    add_library (cryptolib  SHARED          ${crypto_LIB_SRCS})  
    add_library (cryptos    STATIC          ${crypto_LIB_SRCS})  
        
    # 设置依赖关系，如果当前库是最底层的共享库，则不需要设置
    add_dependencies (cryptolib  loglib )
    add_dependencies (cryptos    loglib )
    
    # 添加要链接的共享库, 库的顺序就是编译器的链接时顺序
    target_link_libraries (cryptolib  loglib )
    target_link_libraries (cryptos    loglib )
    
    # 设置版本号，SOVERSION为API版本号
    set_target_properties(cryptolib     PROPERTIES 
                                        VERSION     ${crypto_LIB_VERSION} 
                                        SOVERSION   ${crypto_LIB_SOVERSION} )
ELSE (COMPONENT_crypto)
    MESSAGE(STATUS "Not Include COMPONENT_crypto module.")
ENDIF (COMPONENT_crypto)




