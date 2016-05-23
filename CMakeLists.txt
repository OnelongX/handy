cmake_minimum_required(VERSION 3.5)
project(handy_master)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DOS_MACOSX -Dthread_local=__thread -Wno-deprecated-declarations -DLITTLE_ENDIAN=1 -std=c++11")

set(SOURCE_FILES
    examples/hsha.cc
    handy/codec.cc
    handy/codec.h
#    handy/conf.cc
#    handy/conf.h
    handy/conn.cc
    handy/conn.h
#    handy/daemon.cc
#    handy/daemon.h
    handy/event_base.cc
    handy/event_base.h
#    handy/file.cc
#        handy/file.h
    handy/handy-imp.h
    handy/handy.h
#    handy/http.cc
#    handy/http.h
    handy/logging.cc
    handy/logging.h
    handy/net.cc
    handy/net.h
    handy/poller.cc
    handy/poller.h
    handy/port_posix.cc
    handy/port_posix.h
    handy/slice.h
#    handy/status.h
    handy/threads.cc
    handy/threads.h
    handy/util.cc
    handy/util.h
        )

add_executable(handy_master ${SOURCE_FILES})