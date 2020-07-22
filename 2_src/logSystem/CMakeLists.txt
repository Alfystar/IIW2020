#Assegna a libName il nome della directory corrente
get_filename_component(libName ${CMAKE_CURRENT_SOURCE_DIR} NAME)
if (NOT TARGET ${libName})
    message("\n")
    add_library(${libName}
            Log.cpp Log.h)
    target_include_directories(${libName} PUBLIC ".")


    #Librerie di SO
    find_package(fmt REQUIRED)
    target_link_libraries(${libName} PUBLIC fmt)
    message("\t-FMT library linked on ${libName}")

    find_package(Threads REQUIRED)
    target_link_libraries(${libName} PUBLIC ${CMAKE_THREAD_LIBS_INIT})


    message("##${libName} subsystem included")

endif ()