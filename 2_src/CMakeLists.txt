CMAKE_MINIMUM_REQUIRED(VERSION 3.10)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(HTTP-ImageServer)
message("Sono Unix : ${UNIX}")

add_compile_options(-Wall -Wextra -pedantic -Werror)
#add_compile_definitions(DEBUG_LOG)  # Add debugLog macro to all Source


## Define Oggetti di lavoro
add_executable(mainTest main.cpp logSystem/Log.cpp logSystem/Log.h)


#add_executable(${PROJECT_NAME} main.cpp)

## Ricerca e linking codice necessario
add_subdirectory(simpleWeb)
target_link_libraries(mainTest simpleWeb)

add_subdirectory(ncs)
target_link_libraries(mainTest ncs)

add_subdirectory(logSystem)
target_link_libraries(mainTest logSystem)

add_subdirectory(ces)
target_link_libraries(mainTest ces)

#Include della libreria fmt per Formattare le stringhe
message("\nStart linking to MainTest")
find_library(fmt REQUIRED)
target_link_libraries(mainTest fmt)
message("-FMT library linked on mainTest")

message("##Build Eseguita\n")
