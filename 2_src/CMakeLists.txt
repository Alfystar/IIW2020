CMAKE_MINIMUM_REQUIRED(VERSION 3.10)
project(HTTP-ImageServer)
message("Sono Unix : ${UNIX}")

add_compile_options(-Wall -Wextra -pedantic -Werror)


## Define Oggetti di lavoro
add_executable(mainTest main.cpp)

## Ricerca e linking codice necessario
add_subdirectory(httpSystem)
target_link_libraries(mainTest httpSystem)

add_subdirectory(ncs)
target_link_libraries(mainTest ncs)

add_subdirectory(worker)
target_link_libraries(mainTest worker)

#Include della libreria fmt per Formattare le stringhe
find_library(fmt REQUIRED)
target_link_libraries(mainTest fmt)
message("-FMT library linked on mainTest")

message("\t##Build Eseguita\n")
