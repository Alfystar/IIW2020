if (NOT TARGET worker)
    add_library(worker
            asdsadas.h asdsadas.cpp
            )
    target_include_directories(worker PUBLIC ".")
    find_package(Threads REQUIRED)
    target_link_libraries(worker PUBLIC ${CMAKE_THREAD_LIBS_INIT})

    message("-worker subsystem included")

endif ()