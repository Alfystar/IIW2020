CMAKE_MINIMUM_REQUIRED(VERSION 3.10)
project(HTTP-ImageServer)
message("Sono Unix : ${UNIX}")

## Define Oggetti di lavoro
add_executable(mainTest main.cpp)

## Ricerca e linking codice necessario
add_subdirectory(htmlCompose)
target_link_libraries(mainTest htmlCompose)
#Include della libreria fmt per Formattare le stringhe
find_library(fmt REQUIRED)
target_link_libraries(mainTest fmt)
message("-FMT library linked on mainTest")

message("\t##Build Eseguita\n")
