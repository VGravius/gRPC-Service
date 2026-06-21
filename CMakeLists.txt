# =============================================================================
#  @file CMakeLists.txt
#  @brief Конфигурационный файл сборки проекта gRPC-Service.
#  @version 1.0
#  @date 2026-06-20
#  @author vladcrim
#
#  Данный файл содержит инструкции для CMake по сборке gRPC-сервиса.
#  Определяет требования к версии CMake, компилятору, подключает зависимости
#  gRPC и Protobuf, а также управляет генерацией кода из .proto файлов.
#  Включает сборку основного сервиса и unit-тестов на Google Test.
# =============================================================================

# -----------------------------------------------------------------------------
#  Минимальная версия CMake
# -----------------------------------------------------------------------------

cmake_minimum_required (VERSION 3.15)  # gRPC требует CMake 3.15+

# -----------------------------------------------------------------------------
#  Политика для MSVC (горячая перезагрузка)
# -----------------------------------------------------------------------------

# Включает поддержку "Edit and Continue" для Visual Studio
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

# -----------------------------------------------------------------------------
#  Название проекта
# -----------------------------------------------------------------------------

project ("gRPC-Service")

# -----------------------------------------------------------------------------
#  Стандарт C++
# -----------------------------------------------------------------------------

set(CMAKE_CXX_STANDARD 17)                # Используем C++17
set(CMAKE_CXX_STANDARD_REQUIRED ON)       # Обязательно использовать C++17

# -----------------------------------------------------------------------------
#  Поиск зависимостей (gRPC и Protobuf)
# -----------------------------------------------------------------------------

find_package(gRPC CONFIG REQUIRED)       # Поиск установленного gRPC через vcpkg
find_package(Protobuf CONFIG REQUIRED)   # Поиск установленного Protobuf

# -----------------------------------------------------------------------------
#  Пути для заголовочных файлов
# -----------------------------------------------------------------------------

include_directories(${CMAKE_CURRENT_BINARY_DIR})  # Путь к сгенерированным .pb.h файлам

# =============================================================================
#  ОСНОВНОЙ СЕРВИС
# =============================================================================

# Создаёт исполняемый файл gRPC-Service.exe
# Содержит только исходные файлы сервиса (без тестов)
add_executable (gRPC-Service 
    "src/main.cpp"                          # Точка входа
    "src/telemetry_service.cpp"             # Реализация gRPC-сервиса
    "src/telemetry_service.h"               # Заголовок gRPC-сервиса
    "src/cache.cpp"                         # Реализация кэша
    "src/cache.h"                           # Заголовок кэша
    "src/validator.cpp"                     # Реализация валидатора
    "src/validator.h"                       # Заголовок валидатора
    "src/logger.cpp"                        # Реализация логгера
    "src/logger.h"                          # Заголовок логгера
)

# =============================================================================
#  ГЕНЕРАЦИЯ КОДА ИЗ .proto ФАЙЛОВ
# =============================================================================

# Пути к .proto файлам
set(PROTO_FILES ${CMAKE_CURRENT_SOURCE_DIR}/proto/telemetry.proto)
set(PROTO_GOOGLE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/proto)
set(STATUS_PROTO ${CMAKE_CURRENT_SOURCE_DIR}/proto/google/rpc/status.proto)

# Путь к плагину gRPC для protoc
set(GRPC_CPP_PLUGIN "C:/dev/vcpkg/installed/x64-windows/tools/grpc/grpc_cpp_plugin.exe")

# -----------------------------------------------------------------------------
#  1. Генерация status.proto (google/rpc/status.proto)
# -----------------------------------------------------------------------------

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

# -----------------------------------------------------------------------------
#  2. Генерация protobuf-кода (telemetry.pb.cc / .h)
# -----------------------------------------------------------------------------

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

# -----------------------------------------------------------------------------
#  3. Генерация gRPC-кода (telemetry.grpc.pb.cc / .h)
# -----------------------------------------------------------------------------

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

# -----------------------------------------------------------------------------
#  Подключение сгенерированных .pb.cc файлов к цели
# -----------------------------------------------------------------------------

target_sources(gRPC-Service PRIVATE
    ${CMAKE_CURRENT_BINARY_DIR}/telemetry.pb.cc
    ${CMAKE_CURRENT_BINARY_DIR}/telemetry.grpc.pb.cc
    ${CMAKE_CURRENT_BINARY_DIR}/google/rpc/status.pb.cc
)

# -----------------------------------------------------------------------------
#  Линковка библиотек gRPC и Protobuf
# -----------------------------------------------------------------------------

target_link_libraries(gRPC-Service PRIVATE
    gRPC::gpr                 # Базовые утилиты gRPC
    gRPC::grpc                # Основная библиотека gRPC
    gRPC::grpc++              # C++ обёртка для gRPC
    gRPC::grpc++_alts         # Альтернативная транспортная подсистема
    gRPC::grpc++_reflection   # Поддержка gRPC Reflection API (для grpcurl)
    protobuf::libprotobuf     # Библиотека Protocol Buffers
)

# -----------------------------------------------------------------------------
#  Пути для заголовочных файлов (включая сгенерированные)
# -----------------------------------------------------------------------------

target_include_directories(gRPC-Service PRIVATE
    ${CMAKE_CURRENT_BINARY_DIR}       # Сгенерированные .pb.h файлы
    ${CMAKE_CURRENT_SOURCE_DIR}/src   # Исходные заголовки проекта
)

# =============================================================================
#  UNIT-ТЕСТЫ (Google Test)
# =============================================================================

# Найти Google Test через vcpkg
find_package(GTest CONFIG REQUIRED)

# Создать исполняемый файл unit_tests.exe
add_executable(unit_tests
    unit_tests/test_cache.cpp               # Тесты для кэша
    unit_tests/test_validator.cpp           # Тесты для валидатора
    unit_tests/test_logger.cpp              # Тесты для логгера
    unit_tests/test_telemetry_service.cpp   # Тесты для сервиса
    src/cache.cpp                           # Реализация кэша (для тестов)
    src/validator.cpp                       # Реализация валидатора (для тестов)
    src/logger.cpp                          # Реализация логгера (для тестов)
    src/telemetry_service.cpp               # Реализация сервиса (для тестов)
    ${CMAKE_CURRENT_BINARY_DIR}/telemetry.pb.cc
    ${CMAKE_CURRENT_BINARY_DIR}/telemetry.grpc.pb.cc
    ${CMAKE_CURRENT_BINARY_DIR}/google/rpc/status.pb.cc
)

# Подключение библиотек для тестов
target_link_libraries(unit_tests PRIVATE
    GTest::gtest_main          # Google Test с main()
    gRPC::gpr
    gRPC::grpc
    gRPC::grpc++
    gRPC::grpc++_alts
    protobuf::libprotobuf
)

# Пути для заголовков в тестах
target_include_directories(unit_tests PRIVATE
    ${CMAKE_CURRENT_BINARY_DIR}       # Сгенерированные .pb.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src   # Исходные заголовки проекта
)

# Регистрация тестов в CTest
include(GoogleTest)
gtest_discover_tests(unit_tests)  # Автоматическое обнаружение тестов