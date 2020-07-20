if (NOT TARGET worker)
    add_library(worker worker.h
            Job.h Job.cpp
            )
    target_include_directories(worker PUBLIC ".")
    find_package(Threads REQUIRED)
    target_link_libraries(worker PUBLIC ${CMAKE_THREAD_LIBS_INIT})

    include_directories("../ncs")


    message("-worker subsystem included")

endif ()