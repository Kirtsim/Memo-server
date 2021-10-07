add_library(${logger_lib})
add_library(${logger_test_lib})

set(cur_dir ${CMAKE_CURRENT_LIST_DIR})
target_sources(${logger_lib}
        PRIVATE
        ${cur_dir}/easylogging++.cc
        ${cur_dir}/Logger.cpp
        PUBLIC
        ${cur_dir}/Logger.hpp
        )
target_sources(${logger_test_lib}
        PRIVATE
        ${cur_dir}/easylogging++.cc
        ${cur_dir}/Logger.cpp
        PUBLIC
        ${cur_dir}/Logger.hpp
        )
target_include_directories(${exe_lib}
        PUBLIC
        ${cur_dir}
        )
target_compile_definitions(${logger_test_lib} PRIVATE ELPP_DISABLE_LOGS)
