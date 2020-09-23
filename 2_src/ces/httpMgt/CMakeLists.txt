#Assegna a libName il nome della directory corrente
get_filename_component(libName ${CMAKE_CURRENT_SOURCE_DIR} NAME)
if (NOT TARGET ${libName})
    add_library(${libName}
            HttpMgt.cpp HttpMgt.h
            HtmlMessage.cpp HtmlMessage.h headerForm.h)
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
    message("[${libName}]\t-FMT library linked on ${libName}")


    set(Boost_USE_STATIC_LIBS ON)
    set(Boost_USE_MULTITHREADED ON)
    set(Boost_USE_STATIC_RUNTIME OFF)
    # Boost cmake è fatto in maniera tale che è necessario richiedere un componente
    find_package(Boost COMPONENTS filesystem REQUIRED)
    include_directories(${Boost_INCLUDE_DIRS})
    target_link_libraries(${libName} PUBLIC ${Boost_FILESYSTEM_LIBRARY})
    message("[${libName}]\t-Boost library linked on ${libName}")

    message("[${libName}] subsystem included")

endif ()