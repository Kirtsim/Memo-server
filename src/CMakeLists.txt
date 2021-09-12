set(src_dir ${CMAKE_CURRENT_LIST_DIR})

add_executable(${exe} "")
add_library(${exe_lib} "")

target_sources(${exe} PRIVATE ${src_dir}/main.cpp)
target_sources(${exe_lib}
    PRIVATE
        ${src_dir}/logger/logger.cpp
        ${src_dir}/logger/easylogging++.cc
        ${src_dir}/server/service/BaseService.cpp
        ${src_dir}/server/service/MemoService.cpp
        ${src_dir}/server/service/TagService.cpp
        ${src_dir}/server/process/ListMemosProcess.cpp
        ${src_dir}/server/process/AddMemoProcess.cpp
        ${src_dir}/server/process/ListTagsProcess.cpp
        ${src_dir}/server/process/AddTagProcess.cpp
        ${src_dir}/server/Server.cpp
        ${src_dir}/server/Resources.cpp
        ${src_dir}/db/Sqlite3Wrapper.cpp
        ${src_dir}/model/Memo.cpp
        ${src_dir}/model/Tag.cpp
        ${src_dir}/model/Color.cpp
    INTERFACE
        ${src_dir}/logger/logger.hpp
        ${src_dir}/server/service/IService.hpp
        ${src_dir}/server/service/BaseService.hpp
        ${src_dir}/server/service/MemoService.hpp
        ${src_dir}/server/service/TagService.hpp
        ${src_dir}/server/process/IProcess.hpp
        ${src_dir}/server/process/BaseProcess.hpp
        ${src_dir}/server/process/ListMemosProcess.hpp
        ${src_dir}/server/process/AddMemoProcess.hpp
        ${src_dir}/server/process/ListTagsProcess.hpp
        ${src_dir}/server/process/AddTagProcess.hpp
        ${src_dir}/server/Server.hpp
        ${src_dir}/server/Resources.hpp
        ${src_dir}/db/IDatabase.hpp
        ${src_dir}/db/ISqlite3Wrapper.hpp
        ${src_dir}/db/Sqlite3Wrapper.hpp
        ${src_dir}/model/ModelDefs.hpp
        ${src_dir}/model/Memo.hpp
        ${src_dir}/model/Tag.hpp
        ${src_dir}/model/Color.hpp)

target_include_directories(${exe_lib}
    PUBLIC
        ${src_dir}
        ${memo_proto_include_dir})

target_link_directories(${exe_lib} PUBLIC /usr/lib/sqlite3)
IF(UNIX AND NOT APPLE)
    target_link_directories(${exe_lib} PUBLIC /usr/include/)
ENDIF()

target_link_libraries(${exe_lib}
    PUBLIC
        ${memo_proto_lib}
        ${_REFLECTION}
        ${_GRPC_GRPCPP}
        ${_PROTOBUF_LIBPROTOBUF}
        sqlite3)

target_link_libraries(${exe} PRIVATE ${exe_lib})
