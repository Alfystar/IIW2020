if (NOT TARGET httpSystem)
    add_library(httpSystem
            status_code.hpp status_code.cpp
            utility.hpp
            )
target_include_directories(httpSystem PUBLIC ".")

message("-httpSystem subsystem included")
endif ()