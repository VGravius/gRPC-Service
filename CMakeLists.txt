# CMakeList.txt: проект CMake для gRPC-Service; включите исходный код и определения,
# укажите здесь логику для конкретного проекта.
#
cmake_minimum_required (VERSION 3.15) # Версия для gRPC

# Включение горячей перезагрузки для компиляторов MSVC, если поддерживается.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project ("gRPC-Service")

# Установка стандарта C++ 
set(CMAKE_CXX_STANDARD 17)               # Задаём C++(17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)      # Необходимо обязательно использовать C++(17)

# Настройка gRPC И Protobuf
find_package(gRPC CONFIG REQUIRED)       # Поиск установленного gRPC
find_package(Protobuf CONFIG REQUIRED)   # Поиска Protobuf

# Генерация кода из .proto
set(PROTO_DIR ${CMAKE_CURRENT_SOURCE_DIR}/proto)
set(PROTO_FILES ${PROTO_DIR}/telemetry.proto)

protobuf_generate(
    TARGET gRPC-Service
    LANGUAGE cpp
    PROTOS ${PROTO_FILES}
)

grpc_generate(
    TARGET gRPC-Service
    LANGUAGE cpp
    PROTOS ${PROTO_FILES}
)

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

# TODO: Добавьте тесты и целевые объекты, если это необходимо.
