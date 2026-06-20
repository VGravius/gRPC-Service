# CMakeList.txt: проект CMake для gRPC-Service
cmake_minimum_required (VERSION 3.15)

# Включение горячей перезагрузки для компиляторов MSVC
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project ("gRPC-Service")

# Установка стандарта C++ 
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Настройка gRPC И Protobuf
find_package(gRPC CONFIG REQUIRED)
find_package(Protobuf CONFIG REQUIRED)

include_directories(${CMAKE_CURRENT_BINARY_DIR})

# Добавьте источник в исполняемый файл этого проекта.
add_executable (gRPC-Service 
    "src/main.cpp"             
    "src/telemetry_service.cpp"        
    "src/telemetry_service.h"
    "src/cache.cpp"
    "src/cache.h"
    "src/validator.cpp"
    "src/validator.h"
    "src/logger.cpp"
    "src/logger.h"
)

# Генерация кода из .proto
set(PROTO_FILES ${CMAKE_CURRENT_SOURCE_DIR}/proto/telemetry.proto)
set(PROTO_GOOGLE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/proto)

# Сгенерировать код для status.proto
set(STATUS_PROTO ${CMAKE_CURRENT_SOURCE_DIR}/proto/google/rpc/status.proto)

add_custom_command(
    OUTPUT
        ${CMAKE_CURRENT_BINARY_DIR}/google/rpc/status.pb.cc
        ${CMAKE_CURRENT_BINARY_DIR}/google/rpc/status.pb.h
    COMMAND ${Protobuf_PROTOC_EXECUTABLE}
    ARGS --cpp_out=${CMAKE_CURRENT_BINARY_DIR}
         --proto_path=${PROTO_GOOGLE_PATH}
         ${STATUS_PROTO}
    DEPENDS ${STATUS_PROTO}
    VERBATIM
)

# Найти плагин grpc_cpp_plugin
set(GRPC_CPP_PLUGIN "C:/dev/vcpkg/installed/x64-windows/tools/grpc/grpc_cpp_plugin.exe")

# Сгенерировать protobuf-файлы
add_custom_command(
    OUTPUT
        ${CMAKE_CURRENT_BINARY_DIR}/telemetry.pb.cc
        ${CMAKE_CURRENT_BINARY_DIR}/telemetry.pb.h
    COMMAND ${Protobuf_PROTOC_EXECUTABLE}
    ARGS --cpp_out=${CMAKE_CURRENT_BINARY_DIR}
         --proto_path=${PROTO_GOOGLE_PATH}
         ${PROTO_FILES}
    DEPENDS ${PROTO_FILES}
    VERBATIM
)

# Сгенерировать grpc-файлы
add_custom_command(
    OUTPUT
        ${CMAKE_CURRENT_BINARY_DIR}/telemetry.grpc.pb.cc
        ${CMAKE_CURRENT_BINARY_DIR}/telemetry.grpc.pb.h
    COMMAND ${Protobuf_PROTOC_EXECUTABLE}
    ARGS --grpc_out=${CMAKE_CURRENT_BINARY_DIR}
         --plugin=protoc-gen-grpc=${GRPC_CPP_PLUGIN}
         --proto_path=${PROTO_GOOGLE_PATH}
         ${PROTO_FILES}
    DEPENDS ${PROTO_FILES}
    VERBATIM
)

# Подключить сгенерированные файлы к цели
target_sources(gRPC-Service PRIVATE
    ${CMAKE_CURRENT_BINARY_DIR}/telemetry.pb.cc
    ${CMAKE_CURRENT_BINARY_DIR}/telemetry.grpc.pb.cc
    ${CMAKE_CURRENT_BINARY_DIR}/google/rpc/status.pb.cc
)

# Подключение библиотек gRPC
target_link_libraries(gRPC-Service PRIVATE
    gRPC::gpr
    gRPC::grpc
    gRPC::grpc++
    gRPC::grpc++_alts
    protobuf::libprotobuf
)

# Пути для заголовочных файлов
target_include_directories(gRPC-Service PRIVATE
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)