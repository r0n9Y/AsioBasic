add_library(NetCommon INTERFACE)

install(TARGETS NetCommon DESTINATION lib)
install(FILES 
    include/net_common.h
    include/net_connection.hpp
    include/net_tsqueue.hpp
    include/net_client.hpp
    include/net_server.hpp
    #include/net_utils.h
    DESTINATION include)
#
# INTERFACE means things that consumers require but the producer doesn’t.
target_include_directories(NetCommon INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${ASIO_dir}/include
    )
