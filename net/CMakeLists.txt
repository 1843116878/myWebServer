set(SRCS
        Channel.cpp
        Epoll.cpp
        EventLoop.cpp
        EventLoopThread.cpp
        EventLoopThreadPool.cpp
        HttpData.cpp
        Timer.cpp
        Util.cpp
        Server.cpp
        main.cpp
)
include_directories(${PROJECT_SOURCE_DIR}/base)


add_executable(myWebServer ${SRCS})
target_link_libraries(myWebServer libserver_base)

add_subdirectory(base)
add_subdirectory(test)