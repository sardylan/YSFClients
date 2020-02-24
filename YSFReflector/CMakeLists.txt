add_executable(YSFReflector
        Version.h
        YSFDefines.h
        ip_blacklist.cpp ip_blacklist.hpp
        ysf_repeater.cpp ysf_repeater.hpp
        cli_server.cpp cli_server.hpp
        configuration.cpp configuration.hpp
        log.cpp log.hpp
        Network.cpp Network.h
        StopWatch.cpp StopWatch.h
        Thread.cpp Thread.h
        Timer.cpp Timer.h
        UDPSocket.cpp UDPSocket.h
        utils.cpp utils.hpp
        ysf_reflector.cpp ysf_reflector.hpp)

target_link_libraries(YSFReflector
        LINK_PUBLIC
        Threads::Threads)

target_compile_options(YSFReflector
        PRIVATE
        -Wall)
