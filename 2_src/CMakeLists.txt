CMAKE_MINIMUM_REQUIRED(VERSION 3.10)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address")

project(HTTP-ImageServer)
message("Sono Unix : ${UNIX}")

add_compile_options(-Wall -Wextra -pedantic -Werror)
#add_compile_definitions(IMAGE_RESIZE)   # Abilita il ridimensionamento dinamico delle immagini
#add_compile_definitions(DAD_RECREATE)   # Abilita la resilienza di ricreare il server se crasha

#add_compile_definitions(DEBUG_LOG)      # Abilita la scrittura sul file di log
add_compile_definitions(DEBUG_VERBOSE)      # Abilita la scrittura sul file di log


set(EXE_NAME badAlphaWeb)
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

message("##Build Eseguita\n")