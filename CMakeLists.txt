#CMakeLists.tst

project(INTELLIGENTPERCEPTIONSERVER)

set(SRC_LIST IntelligentPerceptionServer.cpp
		./network/FSocket.cpp
		./network/TCPServer.cpp
		./common/Common.cpp
		./model/Model.cpp
        ./ipc/MessageQueueHandle.cpp
        ./serial/serial.cpp
        ./viewlink/ViewLinkHandle.cpp
        ./network/UDPClient.cpp
        ./common/Common-mq.cpp
        ./model/Model-mq.cpp
        ./mq/MQTTClient.cpp
        )

set(MQTT_LIBRARIES "/usr/lib/aarch64-linux-gnu/") 
		
# 加入一个配置头文件，用于处理 CMake 对源码的设置
configure_file (
  "${INTELLIGENTPERCEPTIONSERVER_SOURCE_DIR}/Config.ini.in"
  "${INTELLIGENTPERCEPTIONSERVER_BINARY_DIR}/Config.ini"
  )
  
# 加入gdb调试 
set(CMAKE_BUILD_TYPE "Debug")
set(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g -ggdb")
set(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -Wall")

include_directories("/usr/local/include/cjson")
include_directories(${PROJECT_SOURCE_DIR}/mq) 


message(STATUS "This is BINARY dir" ${INTELLIGENTPERCEPTIONSERVER_BINARY_DIR})

message(STATUS "This is SOURCE dir" ${INTELLIGENTPERCEPTIONSERVER_SOURCE_DIR})

add_executable(IntelligentPerceptionServer ${SRC_LIST})

target_link_libraries(IntelligentPerceptionServer pthread

                  ${MQTT_LIBRARIES}/libmosquitto.so 
                  -L/usr/local/lib/ -lPocoFoundation
                  -L/usr/local/lib/ -lPocoJSON
                  -L/usr/local/lib/ -lPocoUtil 
                  -L/usr/local/lib/ -lPocoNet
                  -L/usr/local/lib/ -lcjson)

target_link_libraries(IntelligentPerceptionServer ${INTELLIGENTPERCEPTIONSERVER_SOURCE_DIR}/lib/libViewLink.so)

