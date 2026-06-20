// =============================================================================
//  @file telemetry_service.cpp
//  @brief Реализация gRPC-сервиса для управления параметрами.
//  @version 1.0
//  @date 2026-06-20
//  @author vladcrim
//
//  Данный файл содержит реализацию методов GetParameter и SetParameter
//  для gRPC-сервиса TelemetryService.
//  Взаимодействует с компонентами: Cache (хранение), Validator (проверка),
//  Logger (логирование).
// =============================================================================

#include "telemetry_service.h"
#include "cache.h"
#include "validator.h"
#include "logger.h"
#include <chrono>
#include <google/protobuf/timestamp.pb.h>
#include <thread>   // для std::this_thread::sleep_for

// -----------------------------------------------------------------------------
//  Глобальные объекты (синглтоны для всего сервиса)
// -----------------------------------------------------------------------------

static Cache g_cache;          ///< Хранилище параметров (потокобезопасное)
static Validator g_validator;  ///< Валидатор входных данных
static Logger g_logger;        ///< Логгер для записи операций

// =============================================================================
//  Реализация метода GetParameter
// =============================================================================

/**
 * @brief Обрабатывает запрос на получение значения параметра.
 *
 * Алгоритм работы:
 * 1. Проверяет, что имя параметра не пустое.
 * 2. Ищет параметр в кэше.
 * 3. Если параметр найден — возвращает его значение с временной меткой.
 * 4. Если не найден — возвращает статус NOT_FOUND.
 * 5. Логирует результат операции.
 *
 * @param context Контекст сервера (не используется).
 * @param request Запрос, содержащий имя параметра.
 * @param response Ответ, содержащий имя, значение, время и статус.
 * @return grpc::Status Статус выполнения операции (OK или ошибка).
 */
grpc::Status TelemetryServiceImpl::GetParameter(
    grpc::ServerContext* context,
    const telemetry::v1::GetParameterRequest* request,
    telemetry::v1::GetParameterResponse* response) {

    const std::string& name = request->name();

    // 0. Проверка таймаута (клиент отменил запрос)
    if (context->IsCancelled()) {
        g_logger.LogError("GetParameter: request cancelled by client (timeout)");
        response->mutable_status()->set_code(grpc::StatusCode::DEADLINE_EXCEEDED);
        response->mutable_status()->set_message("Request timeout");
        return grpc::Status(grpc::StatusCode::DEADLINE_EXCEEDED, "Request timeout");
    }

    // ================================================================
    // ⏱️ БЛОК ДЛЯ ОТЛАДКИ ТАЙМАУТА: имитирует долгую обработку (закомментирован)
    // Раскомментируйте для проверки обработки таймаута клиентом.
    // Сервер "спит" 30 секунд, проверяя отмену каждые 100 мс.
    // При таймауте клиента (например, -connect-timeout 1) возвращает DEADLINE_EXCEEDED.
    //
    // ВАЖНО: Клиент должен отменить запрос ДО того, как сервер завершит обработку.
    // Если сервер успеет отправить ответ до истечения таймаута, клиент получит OK.
    // Реализовано специцально для теста:
    //           ОШИБКА: GetParameter: request cancelled by client (timeout)
    // ================================================================
     //for (int i = 0; i < 100; ++i) {
     //    if (context->IsCancelled()) {
     //        g_logger.LogError("GetParameter: request cancelled by client (timeout)");
     //        response->mutable_status()->set_code(grpc::StatusCode::DEADLINE_EXCEEDED);
     //        response->mutable_status()->set_message("Request timeout");
     //        return grpc::Status(grpc::StatusCode::DEADLINE_EXCEEDED, "Request timeout");
     //    }
     //    std::this_thread::sleep_for(std::chrono::milliseconds(10));
     //}
    // ================================================================

    // 1. Валидация имени параметра
    if (!g_validator.ValidateName(name)) {
        g_logger.LogError("GetParameter: invalid name (empty)");
        response->mutable_status()->set_code(grpc::StatusCode::INVALID_ARGUMENT);
        response->mutable_status()->set_message("Parameter name cannot be empty");
        return grpc::Status(grpc::StatusCode::INVALID_ARGUMENT, "Invalid name");
    }

    // 2. Поиск параметра в кэше
    auto value = g_cache.Get(name);
    if (!value.has_value()) {
        // 3. Параметр не найден — возвращаем NOT_FOUND
        g_logger.Log("GetParameter", name, "NOT_FOUND");
        response->mutable_status()->set_code(grpc::StatusCode::NOT_FOUND);
        response->mutable_status()->set_message("Parameter not found");
        return grpc::Status(grpc::StatusCode::NOT_FOUND, "Parameter not found");
    }

    // 4. Формирование успешного ответа
    response->set_name(name);
    response->set_value(value.value());

    // 5. Добавление текущего времени (UTC)
    auto now = std::chrono::system_clock::now();
    auto timestamp = google::protobuf::Timestamp();
    auto seconds = std::chrono::duration_cast<std::chrono::seconds>(
        now.time_since_epoch()).count();
    auto nanos = std::chrono::duration_cast<std::chrono::nanoseconds>(
        now.time_since_epoch()).count() % 1000000000;
    timestamp.set_seconds(seconds);
    timestamp.set_nanos(static_cast<int32_t>(nanos));
    *response->mutable_timestamp() = timestamp;

    // 6. Установка статуса OK
    response->mutable_status()->set_code(grpc::StatusCode::OK);
    response->mutable_status()->set_message("OK");

    // 👇 ДОБАВЬТЕ ПРОВЕРКУ ПЕРЕД ОТПРАВКОЙ ОТВЕТА
    if (context->IsCancelled()) {
        g_logger.LogError("GetParameter: request cancelled by client before response");
        response->mutable_status()->set_code(grpc::StatusCode::DEADLINE_EXCEEDED);
        response->mutable_status()->set_message("Request timeout");
        return grpc::Status(grpc::StatusCode::DEADLINE_EXCEEDED, "Request timeout");
    }

    // 7. Логирование успешной операции
    g_logger.Log("GetParameter", name, "OK", "value: " + value.value());

    return grpc::Status::OK;
}

// =============================================================================
//  Реализация метода SetParameter
// =============================================================================

/**
 * @brief Обрабатывает запрос на установку значения параметра.
 *
 * Алгоритм работы:
 * 1. Проверяет, что имя, значение и request_id не пустые.
 * 2. Проверяет, что request_id не использовался ранее.
 * 3. Сохраняет параметр в кэш и отмечает request_id как использованный.
 * 4. Возвращает статус OK или ошибку.
 * 5. Логирует результат операции.
 *
 * @param context Контекст сервера (не используется).
 * @param request Запрос, содержащий имя, значение и request_id.
 * @param response Ответ, содержащий статус операции.
 * @return grpc::Status Статус выполнения операции (OK или ошибка).
 */
grpc::Status TelemetryServiceImpl::SetParameter(
    grpc::ServerContext* context,
    const telemetry::v1::SetParameterRequest* request,
    telemetry::v1::SetParameterResponse* response) {

    const std::string& name = request->name();
    const std::string& value = request->value();
    const std::string& request_id = request->request_id();

    // 0. Проверка таймаута (клиент отменил запрос)
    if (context->IsCancelled()) {
        g_logger.LogError("SetParameter: request cancelled by client (timeout)");
        response->mutable_status()->set_code(grpc::StatusCode::DEADLINE_EXCEEDED);
        response->mutable_status()->set_message("Request timeout");
        return grpc::Status(grpc::StatusCode::DEADLINE_EXCEEDED, "Request timeout");
    }

    // 1. Валидация имени
    if (!g_validator.ValidateName(name)) {
        g_logger.LogError("SetParameter: invalid name (empty)");
        response->mutable_status()->set_code(grpc::StatusCode::INVALID_ARGUMENT);
        response->mutable_status()->set_message("Parameter name cannot be empty");
        return grpc::Status(grpc::StatusCode::INVALID_ARGUMENT, "Invalid name");
    }

    // 2. Валидация значения
    if (!g_validator.ValidateValue(value)) {
        g_logger.LogError("SetParameter: invalid value (empty)");
        response->mutable_status()->set_code(grpc::StatusCode::INVALID_ARGUMENT);
        response->mutable_status()->set_message("Parameter value cannot be empty");
        return grpc::Status(grpc::StatusCode::INVALID_ARGUMENT, "Invalid value");
    }

    // 3. Валидация request_id
    if (!g_validator.ValidateRequestId(request_id)) {
        g_logger.LogError("SetParameter: invalid request_id (empty)");
        response->mutable_status()->set_code(grpc::StatusCode::INVALID_ARGUMENT);
        response->mutable_status()->set_message("Request ID cannot be empty");
        return grpc::Status(grpc::StatusCode::INVALID_ARGUMENT, "Invalid request_id");
    }

    // 4. Проверка уникальности request_id (защита от дублей)
    if (g_validator.IsRequestIdUsed(request_id)) {
        g_logger.Log("SetParameter", name, "ALREADY_EXISTS", "request_id: " + request_id);
        response->mutable_status()->set_code(grpc::StatusCode::ALREADY_EXISTS);
        response->mutable_status()->set_message("Request ID already used");
        return grpc::Status(grpc::StatusCode::ALREADY_EXISTS, "Request ID already used");
    }

    // 5. Сохранение параметра в кэш
    g_cache.Set(name, value);
    g_validator.MarkRequestIdUsed(request_id);

    // 6. Успешный ответ
    response->mutable_status()->set_code(grpc::StatusCode::OK);
    response->mutable_status()->set_message("OK");

    // 7. Логирование успешной операции
    g_logger.Log("SetParameter", name, "OK", "value: " + value + ", request_id: " + request_id);

    return grpc::Status::OK;
}