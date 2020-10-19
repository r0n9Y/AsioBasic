add_library(NetCommon STATIC NetCommon.cpp)

install(TARGETS NetCommon DESTINATION lib)
install(FILES include/NetCommon.h DESTINATION include)

# INTERFACE means things that consumers require but the producer doesn’t.
target_include_directories(NetCommon INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)