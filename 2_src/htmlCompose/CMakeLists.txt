if (NOT TARGET htmlCompose)
    add_library(htmlCompose
            status_code.hpp status_code.cpp
            utility.hpp
            )
target_include_directories(htmlCompose PUBLIC ".")

message("-htmlCompose adding")
endif ()