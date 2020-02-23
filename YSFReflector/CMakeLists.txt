add_executable(YSFReflector
        Version.h
        YSFDefines.h
        ip_blacklist.cpp ip_blacklist.hpp
        Conf.cpp Conf.h
        Log.cpp Log.h
        Network.cpp Network.h
        StopWatch.cpp StopWatch.h
        Thread.cpp Thread.h
        Timer.cpp Timer.h
        UDPSocket.cpp UDPSocket.h
        utils.cpp utils.hpp
        YSFReflector.cpp YSFReflector.h)

target_link_libraries(YSFReflector
        LINK_PUBLIC
        Threads::Threads)

target_compile_options(YSFReflector
        PRIVATE
        -Wall)
