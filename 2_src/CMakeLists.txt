CMAKE_MINIMUM_REQUIRED(VERSION 3.10)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(HTTP-ImageServer)
message("Sono Unix : ${UNIX}")

add_compile_options(-Wall -Wextra -pedantic -Werror)
add_compile_definitions(DEBUG_LOG)  # Add debugLog, define for all Source Code
#add_compile_definitions(DAD_RECREATE)  # Enable dad-recreate if son fail, define for all Source Code

set(EXE_NAME badAlphaWeb)
## Define Oggetti di lavoro
add_executable(${EXE_NAME} main.cpp GLOBAL_DEF.h) #logSystem/Log.cpp logSystem/Log.h


## Ricerca e linking codice necessario
add_subdirectory(simpleWeb)
target_link_libraries(${EXE_NAME} simpleWeb)

add_subdirectory(ncs)
target_link_libraries(${EXE_NAME} ncs)

add_subdirectory(logSystem)
target_link_libraries(${EXE_NAME} logSystem)

add_subdirectory(ces)
target_link_libraries(${EXE_NAME} ces)

##Include della libreria fmt per Formattare le stringhe
#message("\nStart linking to MainTest")
#find_library(fmt REQUIRED)
#target_link_libraries(${EXE_NAME} fmt)
#message("-FMT library linked on mainTest")

message("##Build Eseguita\n")
