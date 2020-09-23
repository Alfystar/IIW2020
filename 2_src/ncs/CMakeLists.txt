#Assegna a libName il nome della directory corrente
get_filename_component(libName ${CMAKE_CURRENT_SOURCE_DIR} NAME)
if (NOT TARGET ${libName})
    add_library(${libName} ncsDefine.h timeSpecOp.h
            NCS.h NCS.cpp
            Accept.h Accept.cpp
            Queue.h Queue.cpp
            ../GLOBAL_DEF.h
            )
    target_include_directories(${libName} PUBLIC ".")
    find_package(Threads REQUIRED)
    target_link_libraries(${libName} PUBLIC ${CMAKE_THREAD_LIBS_INIT})

    include_directories(${PROJECT_SOURCE_DIR}/simpleWeb)
    target_link_libraries(${libName} PUBLIC simpleWeb)

    include_directories(${PROJECT_SOURCE_DIR}/logSystem)
    target_link_libraries(${libName} PUBLIC logSystem)

    message("[${libName}] subsystem included")

endif ()