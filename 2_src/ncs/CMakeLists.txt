if (NOT TARGET ncs)
    add_library(ncs ncsDefine.h
            NCS.h NCS.cpp
            Accept.h Accept.cpp
            Connection.h Connection.cpp
            Queue.h Queue.cpp
            )
    target_include_directories(ncs PUBLIC ".")
    find_package(Threads REQUIRED)
    target_link_libraries(ncs PUBLIC ${CMAKE_THREAD_LIBS_INIT})

    message("-ncs subsystem included")

endif ()