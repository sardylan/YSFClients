add_executable(YSFReflector
        Version.h
        YSFDefines.h
        Conf.cpp Conf.h
        Log.cpp Log.h
        Network.cpp Network.h
        StopWatch.cpp StopWatch.h
        Thread.cpp Thread.h
        Timer.cpp Timer.h
        UDPSocket.cpp UDPSocket.h
        Utils.cpp Utils.h
        YSFReflector.cpp YSFReflector.h)

target_link_libraries(YSFReflector
        LINK_PUBLIC
        Threads::Threads)

target_compile_options(YSFReflector
        PRIVATE
        -Wall)
