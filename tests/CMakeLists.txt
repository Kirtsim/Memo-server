add_executable(${tests_exe})

set(test_dir ${CMAKE_CURRENT_LIST_DIR})
target_sources(${tests_exe}
        PRIVATE
        ${test_dir}/main.cpp
        ${test_dir}/TestingUtils.cpp
        ${test_dir}/ModelTestHelperFunctions.cpp
        ${test_dir}/db/Sqlite3TestHelperFunctions.cpp
        ${test_dir}/db/TestSqlite3Wrapper.inl
        ${test_dir}/db/TestSqlite3Functions.inl
        ${test_dir}/db/TestSqlite3Database.inl
        ${test_dir}/db/TestTools.inl
        )

set(googletest_dir ${CMAKE_SOURCE_DIR}/3rdparty/googletest)
target_include_directories(${tests_exe}
        PRIVATE
        ${test_dir}
        ${googletest_dir}/include
        ${CMAKE_SOURCE_DIR}/src)
target_link_directories(${tests_exe} PRIVATE ${googletest_dir}/lib)
target_link_libraries(${tests_exe} PRIVATE ${exe_lib} gtest)

add_definitions(-DTESTS_DIR="${test_dir}")
