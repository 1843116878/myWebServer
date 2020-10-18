cmake_minimum_required(VERSION 3.10)
project(myWebServer)

set(CMAKE_CXX_STANDARD 14)

add_executable(myWebServer main.cpp net/base/CountDownLatch.h net/base/CountDownLatch.cpp net/base/Thread.h net/base/Thread.cpp net/base/FileUtil.h net/base/FileUtil.cpp net/base/CurrentThread.h)