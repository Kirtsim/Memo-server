add_executable(${tests_exe})

set(test_dir ${CMAKE_CURRENT_LIST_DIR})
target_sources(${tests_exe}
        PRIVATE
        ${test_dir}/main.cpp
        )

set(googletest_dir ${CMAKE_SOURCE_DIR}/3rdparty/googletest)
target_include_directories(${tests_exe}
        PRIVATE
        ${googletest_dir}/include
        ${CMAKE_SOURCE_DIR}/src)
target_link_directories(${tests_exe} PRIVATE ${googletest_dir}/lib)
target_link_libraries(${tests_exe} PRIVATE ${exe_lib} gtest)