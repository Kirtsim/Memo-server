set(src_dir ${CMAKE_CURRENT_LIST_DIR})

add_executable(${exe} "")
add_library(${exe_lib} "")

target_sources(${exe} PRIVATE ${src_dir}/main.cpp)
target_sources(${exe_lib}
    PRIVATE
        ${src_dir}/logger/logger.cpp
        ${src_dir}/logger/easylogging++.cc
        ${src_dir}/server/service/BaseService.cpp
        ${src_dir}/server/service/tag/TagSvc.cpp
        ${src_dir}/server/service/memo/MemoSvc.cpp
        ${src_dir}/server/service/MemoService.cpp
        ${src_dir}/server/service/TagService.cpp
        ${src_dir}/server/service/process/ListMemosProcess.cpp
        ${src_dir}/server/service/process/AddMemoProcess.cpp
        ${src_dir}/server/service/process/ListTagsProcess.cpp
        ${src_dir}/server/service/process/AddTagProcess.cpp
        ${src_dir}/server/service/memo/process/SearchProcess.cpp
        ${src_dir}/server/service/memo/process/SearchByIdProcess.cpp
        ${src_dir}/server/service/memo/process/CreateProcess.cpp
        ${src_dir}/server/service/memo/process/DeleteProcess.cpp
        ${src_dir}/server/service/memo/process/UpdateProcess.cpp
        ${src_dir}/server/service/tag/process/SearchProcess.cpp
        ${src_dir}/server/service/tag/process/CreateProcess.cpp
        ${src_dir}/server/service/tag/process/DeleteProcess.cpp
        ${src_dir}/server/service/tag/process/UpdateProcess.cpp
        ${src_dir}/server/Server.cpp
        ${src_dir}/server/Resources.cpp
    INTERFACE
        ${src_dir}/logger/logger.hpp
        ${src_dir}/server/service/IService.hpp
        ${src_dir}/server/service/BaseService.hpp
        ${src_dir}/server/service/IProcess.hpp
        ${src_dir}/server/service/tag/TagSvc.hpp
        ${src_dir}/server/service/memo/MemoSvc.hpp
        ${src_dir}/server/service/MemoService.hpp
        ${src_dir}/server/service/TagService.hpp
        ${src_dir}/server/service/BaseProcess.hpp
        ${src_dir}/server/service/process/ListMemosProcess.hpp
        ${src_dir}/server/service/process/AddMemoProcess.hpp
        ${src_dir}/server/service/process/ListTagsProcess.hpp
        ${src_dir}/server/service/process/AddTagProcess.hpp
        ${src_dir}/server/service/memo/process/SearchProcess.hpp
        ${src_dir}/server/service/memo/process/SearchByIdProcess.hpp
        ${src_dir}/server/service/memo/process/CreateProcess.hpp
        ${src_dir}/server/service/memo/process/DeleteProcess.hpp
        ${src_dir}/server/service/memo/process/UpdateProcess.hpp
        ${src_dir}/server/service/tag/process/SearchProcess.hpp
        ${src_dir}/server/service/tag/process/CreateProcess.hpp
        ${src_dir}/server/service/tag/process/DeleteProcess.hpp
        ${src_dir}/server/service/tag/process/UpdateProcess.hpp
        ${src_dir}/server/Server.hpp
        ${src_dir}/server/Resources.hpp)

target_include_directories(${exe_lib}
    PUBLIC
        ${src_dir}
        ${memo_proto_include_dir})

target_link_libraries(${exe_lib}
    PUBLIC
        ${memo_proto_lib}
        ${_REFLECTION}
        ${_GRPC_GRPCPP}
        ${_PROTOBUF_LIBPROTOBUF})

target_link_libraries(${exe} PRIVATE ${exe_lib})