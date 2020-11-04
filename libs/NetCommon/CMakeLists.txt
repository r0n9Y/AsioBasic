add_library(NetCommon INTERFACE)

install(TARGETS NetCommon DESTINATION lib)
install(FILES 
    include/net_common.h
    include/net_connection.h
    include/net_tsqueue.h
    include/net_client.h
    include/net_server.h
    DESTINATION include)

# INTERFACE means things that consumers require but the producer doesn’t.
target_include_directories(NetCommon INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${ASIO_dir}/include
    )
