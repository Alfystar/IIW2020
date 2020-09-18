#Assegna a libName il nome della directory corrente
get_filename_component(libName ${CMAKE_CURRENT_SOURCE_DIR} NAME)
if (NOT TARGET ${libName})
    message("\n")
    add_library(${libName}
            Resource.cpp Resource.h
            Shredder.cpp Shredder.h
            SyncUtilities.cpp SyncUtilities.h
            ../../../GLOBAL_DEF.h
            timeSpecOp.h)
    target_include_directories(${libName} PUBLIC ".")

    #Librerie di SO
    find_package(Threads REQUIRED)
    target_link_libraries(${libName} PUBLIC ${CMAKE_THREAD_LIBS_INIT})


    message("##${libName} subsystem included")

endif ()