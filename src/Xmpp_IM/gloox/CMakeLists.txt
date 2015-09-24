########################################################################################
#############           添加所要编译的源文件.                             ############## 
########################################################################################
list( APPEND gloox_Libs_SRCS
	${CMAKE_CURRENT_SOURCE_DIR}/gloox/adhoc.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/gloox/amp.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/gloox/annotations.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/gloox/atomicrefcount.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/gloox/attention.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/gloox/base64.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/gloox/bookmarkstorage.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/gloox/capabilities.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/gloox/carbons.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/gloox/chatstate.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/gloox/chatstatefilter.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/gloox/client.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/gloox/clientbase.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/gloox/component.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/gloox/compressiondefault.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/gloox/compressionzlib.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/gloox/connectionbosh.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/gloox/connectionhttpproxy.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/gloox/connectionsocks5proxy.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/gloox/connectiontcpbase.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/gloox/connectiontcpclient.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/gloox/connectiontcpserver.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/gloox/connectiontls.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/gloox/connectiontlsserver.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/gloox/dataform.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/gloox/dataformfield.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/gloox/dataformfieldcontainer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/gloox/dataformitem.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/gloox/dataformreported.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/gloox/delayeddelivery.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/gloox/disco.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/gloox/dns.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/gloox/error.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/gloox/eventdispatcher.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/gloox/featureneg.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/gloox/flexoff.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/gloox/forward.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/gloox/gloox.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/gloox/gpgencrypted.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/gloox/gpgsigned.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/gloox/inbandbytestream.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/gloox/instantmucroom.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/gloox/iodata.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/gloox/iq.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/gloox/jid.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/gloox/jinglecontent.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/gloox/jinglefiletransfer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/gloox/jingleiceudp.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/gloox/jinglepluginfactory.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/gloox/jinglesession.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/gloox/jinglesessionmanager.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/gloox/lastactivity.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/gloox/linklocalclient.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/gloox/linklocalmanager.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/gloox/logsink.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/gloox/md5.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/gloox/message.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/gloox/messageevent.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/gloox/messageeventfilter.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/gloox/messagefilter.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/gloox/messagesession.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/gloox/mucinvitationhandler.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/gloox/mucmessagesession.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/gloox/mucroom.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/gloox/mutex.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/gloox/nickname.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/gloox/nonsaslauth.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/gloox/oob.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/gloox/parser.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/gloox/prep.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/gloox/presence.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/gloox/privacyitem.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/gloox/privacymanager.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/gloox/privatexml.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/gloox/pubsubevent.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/gloox/pubsubitem.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/gloox/pubsubmanager.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/gloox/receipt.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/gloox/registration.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/gloox/rosteritem.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/gloox/rostermanager.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/gloox/search.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/gloox/sha.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/gloox/shim.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/gloox/simanager.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/gloox/siprofileft.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/gloox/socks5bytestream.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/gloox/socks5bytestreammanager.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/gloox/socks5bytestreamserver.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/gloox/softwareversion.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/gloox/stanza.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/gloox/stanzaextensionfactory.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/gloox/subscription.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/gloox/tag.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/gloox/tlsdefault.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/gloox/tlsgnutlsbase.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/gloox/tlsgnutlsclient.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/gloox/tlsgnutlsclientanon.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/gloox/tlsgnutlsserveranon.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/gloox/tlsopensslbase.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/gloox/tlsopensslclient.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/gloox/tlsopensslserver.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/gloox/tlsschannel.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/gloox/uniquemucroom.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/gloox/util.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/gloox/vcard.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/gloox/vcardmanager.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/gloox/vcardupdate.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/gloox/version.rc
    ${CMAKE_CURRENT_SOURCE_DIR}/gloox/xhtmlim.cpp
    )


########################################################################################
#############               添加源码子目录                                ############## 
########################################################################################
#add_subdirectory( gloox )

MESSAGE("${CMAKE_CURRENT_SOURCE_DIR} status.")



########################################################################################
#############               设置 gloox 库版本号                         ############## 
########################################################################################
set(gloox_LIB_VERSION     "1.0.0")
set(gloox_LIB_SOVERSION   "1")



########################################################################################
#############           生成目标库文件 并 标明版本号                      ############## 
########################################################################################
IF (MODULE_gloox)
    #生成动态库  静态库则 STATIC  
    add_library (glooxlib   SHARED          ${gloox_Libs_SRCS})  
    add_library (glooxs     STATIC          ${gloox_Libs_SRCS})  
    
    # 设置依赖关系，如果当前库是最底层的共享库，则不需要设置
    add_dependencies (glooxlib  loglib )
    add_dependencies (glooxs    loglib )
    
    # 添加要链接的共享库, 库的顺序就是编译器的链接时顺序
    target_link_libraries (glooxlib  loglib )
    target_link_libraries (glooxs    loglib )
    
    # 设置版本号，SOVERSION为API版本号
    set_target_properties(glooxlib  PROPERTIES 
                                    VERSION     ${gloox_LIB_VERSION} 
                                    SOVERSION   ${gloox_LIB_SOVERSION} )
ELSE (MODULE_gloox)
    MESSAGE(STATUS "Not Include MODULE_gloox module.")
ENDIF (MODULE_gloox)


