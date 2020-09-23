CMAKE_MINIMUM_REQUIRED(VERSION 3.10)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
project(HTTP-ImageServer)

if (IMAGE_RESIZE_EN)
    message("IMAGE_RESIZE_EN evaluates to True")
    set(EXTENSION ${EXTENSION}img_)
    add_compile_definitions(IMAGE_RESIZE)   # Abilita il ridimensionamento dinamico delle immagini
endif ()
if (DAD_RECREATE_EN)
    message("DAD_RECREATE_EN evaluates to True")
    set(EXTENSION ${EXTENSION}dadRecreate)
    add_compile_definitions(DAD_RECREATE)   # Abilita la resilienza di ricreare il server se crasha
endif ()
if (DEBUG_LOG_EN)
    message("DEBUG_LOG_EN evaluates to True")
    add_compile_definitions(DEBUG_LOG)      # Abilita la scrittura sul file di log
    set(EXTENSION ${EXTENSION}log)
endif ()
if (DEBUG_VERBOSE_EN)
    message("DEBUG_VERBOSE_EN evaluates to True")
    add_compile_definitions(DEBUG_VERBOSE)  # Abilita la scrittura sul terminale dei passi
    set(EXTENSION ${EXTENSION}verbose)
endif ()

message("Platform type Unix : ${UNIX}")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address")
add_compile_options(-Wall -Wextra -pedantic -Werror)

if (NOT DEFINED EXTENSION)    # Define e setting di defautl
    set(EXTENSION basic)
    #add_compile_definitions(IMAGE_RESIZE)   # Abilita il ridimensionamento dinamico delle immagini
    #add_compile_definitions(DAD_RECREATE)   # Abilita la resilienza di ricreare il server se crasha
    #add_compile_definitions(DEBUG_LOG)      # Abilita la scrittura sul file di log
    #add_compile_definitions(DEBUG_VERBOSE)  # Abilita la scrittura sul terminale dei passi
endif ()

set(EXE_NAME badAlphaWeb_${EXTENSION})
add_executable(${EXE_NAME} main.cpp GLOBAL_DEF.h)

## Ricerca e linking codice necessario
message("<====simpleWeb====>")
add_subdirectory(simpleWeb)
target_link_libraries(${EXE_NAME} simpleWeb)
message("")

message("<====ncs====>")
add_subdirectory(ncs)
target_link_libraries(${EXE_NAME} ncs)
message("")

message("<====logSystem====>")
add_subdirectory(logSystem)
target_link_libraries(${EXE_NAME} logSystem)
message("")

message("<====ces====>")
add_subdirectory(ces)
target_link_libraries(${EXE_NAME} ces)
message("")

message("/=========${EXE_NAME}=========\\")
message("  [${EXE_NAME}] Build Eseguita")
message("\\=========${EXE_NAME}=========/\n")


### Variabili da inserire da terminale per scegliere i flag con cui generare il make
message("To activate variable please call: cmake [...] -D<VariableName>=True")
message("IMAGE_RESIZE_EN=${IMAGE_RESIZE_EN}")
message("DAD_RECREATE_EN=${DAD_RECREATE_EN}")
message("DEBUG_LOG_EN=${DEBUG_LOG_EN}")
message("DEBUG_VERBOSE_EN=${DEBUG_VERBOSE_EN}")
