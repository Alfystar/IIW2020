#Assegna a libName il nome della directory corrente
get_filename_component(libName ${CMAKE_CURRENT_SOURCE_DIR} NAME)
if (NOT TARGET ${libName})
    message("\n")
    add_library(${libName}
            httpMgt.cpp httpMgt.h)
    target_include_directories(${libName} PUBLIC ".")

    #Direcotry Utility
    include_directories(${PROJECT_SOURCE_DIR}/simpleWeb)
    target_link_libraries(${libName} PUBLIC simpleWeb)

    #Sotto sistemi propri
    add_subdirectory(cacheMgt)
    target_link_libraries(${libName} PUBLIC cacheMgt)

    #Librerie di SO
    find_package(fmt REQUIRED)
    target_link_libraries(${libName} PUBLIC fmt)
    message("\t-FMT library linked on ${libName}")

    message("##${libName} subsystem included")

endif ()