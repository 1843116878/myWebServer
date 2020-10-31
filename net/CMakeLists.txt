set(SRCS
        Util.cpp)

include_directories(${PROJECT_SOURCE_DIR}/base)


add_executable(myWebServer ${SRCS})
target_link_libraries(myWebServer libserver_base)

add_subdirectory(base)