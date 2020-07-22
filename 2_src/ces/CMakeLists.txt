#Assegna a libName il nome della directory corrente
get_filename_component(libName ${CMAKE_CURRENT_SOURCE_DIR} NAME)
if (NOT TARGET ${libName})
    message("\n")
    add_library(${libName}
            CES.cpp CES.h
            Worker.cpp Worker.h)
    target_include_directories(${libName} PUBLIC ".")

    #Direcotry Utility
    include_directories(${PROJECT_SOURCE_DIR}/simpleWeb)
    target_link_libraries(${libName} PUBLIC simpleWeb)

    include_directories(${PROJECT_SOURCE_DIR}/logSystem)
    target_link_libraries(${libName} PUBLIC logSystem)

    #Sistemi dello stesso livello
    include_directories("${PROJECT_SOURCE_DIR}/ncs")
    target_link_libraries(${libName} PUBLIC ncs)

    #Sotto sistemi propri
    add_subdirectory(httpMgt)
    target_link_libraries(${libName} PUBLIC httpMgt)

    #Librerie di SO
    find_package(Threads REQUIRED)
    target_link_libraries(${libName} PUBLIC ${CMAKE_THREAD_LIBS_INIT})

    #compito di httpMgt, presente solo per la demo
    find_package(fmt REQUIRED)
    target_link_libraries(${libName} PUBLIC fmt)
    message("\t-FMT library linked on ${libName}")


    message("##${libName} subsystem included")

endif ()