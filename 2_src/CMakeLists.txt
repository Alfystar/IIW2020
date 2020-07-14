CMAKE_MINIMUM_REQUIRED(VERSION 3.10)
project(HTTP-ImageServer)
message("Sono Unix : ${UNIX}")

## Define Oggetti di lavoro
add_executable(mainTest main.cpp)

## Ricerca e linking codice necessario
add_subdirectory(htmlCompose)
target_link_libraries(mainTest htmlCompose)

message("##-Bulid Eseguita\n")
