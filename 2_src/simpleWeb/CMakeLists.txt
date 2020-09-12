
## IMPORTANTE: installare libreria boost++ tramite apt:
# sudo apt-get install libboost-all-dev

#Assegna a libName il nome della directory corrente
get_filename_component(libName ${CMAKE_CURRENT_SOURCE_DIR} NAME)
if (NOT TARGET ${libName})
    message("\n")
    add_library(${libName}
            Connection.h Connection.cpp
            status_code.hpp
            utility.hpp
            )
    target_include_directories(${libName} PUBLIC ".")

    include_directories(${PROJECT_SOURCE_DIR}/logSystem)
    target_link_libraries(${libName} PUBLIC logSystem)

    message("##${libName} subsystem included")
endif ()