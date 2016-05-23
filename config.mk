CC=cc
CXX=c++
PLATFORM=OS_MACOSX
PLATFORM_LDFLAGS=
PLATFORM_LIBS=
PLATFORM_CCFLAGS=  -DOS_MACOSX -Dthread_local=__thread -Wno-deprecated-declarations -DLITTLE_ENDIAN=1 -std=c++11
PLATFORM_CXXFLAGS=  -DOS_MACOSX -Dthread_local=__thread -Wno-deprecated-declarations -DLITTLE_ENDIAN=1 -std=c++11
PLATFORM_SHARED_CFLAGS=-fPIC
PLATFORM_SHARED_EXT=dylib
PLATFORM_SHARED_LDFLAGS=-dynamiclib -install_name /Users/huanglong/Downloads/handy-master/
PLATFORM_SHARED_VERSIONED=true
