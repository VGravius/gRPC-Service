// =============================================================================
//  @file telemetry_service.h
//  @brief Объявление класса gRPC-сервиса для управления параметрами.
//  @version 1.0
//  @date 2026-06-20
//  @author vladcrim
//
//  Данный файл содержит объявление класса TelemetryServiceImpl,
//  который наследует сгенерированный gRPC-сервис telemetry::v1::TelemetryService.
//  Класс переопределяет методы GetParameter и SetParameter для обработки
//  входящих запросов.
// =============================================================================

#pragma once

#include <grpcpp/grpcpp.h>          // Основной заголовок gRPC для C++
#include "telemetry.grpc.pb.h"       // Сгенерированный код из .proto файла

/**
 * @brief Класс-наследник gRPC-сервиса TelemetryService.
 *
 * Реализует методы для получения и установки параметров системы.
 * Наследует сгенерированный класс telemetry::v1::TelemetryService::Service
 * и переопределяет его виртуальные методы.
 *
 * @note Класс объявлен как final — от него нельзя наследоваться.
 * @note Все методы возвращают grpc::Status и работают с контекстом ServerContext.
 */
class TelemetryServiceImpl final : public telemetry::v1::TelemetryService::Service {
public:
    /**
     * @brief Обрабатывает запрос на получение значения параметра.
     *
     * Метод должен:
     * 1. Проверить, что имя параметра не пустое.
     * 2. Найти параметр в кэше.
     * 3. Если найден — вернуть значение с временной меткой.
     * 4. Если не найден — вернуть статус NOT_FOUND.
     *
     * @param context Контекст gRPC-сервера (передаётся автоматически).
     * @param request Запрос, содержащий имя параметра.
     * @param response Ответ, содержащий имя, значение, время и статус.
     * @return grpc::Status Статус выполнения операции.
     */
    grpc::Status GetParameter(
        grpc::ServerContext* context,
        const telemetry::v1::GetParameterRequest* request,
        telemetry::v1::GetParameterResponse* response) override;

    /**
     * @brief Обрабатывает запрос на установку значения параметра.
     *
     * Метод должен:
     * 1. Проверить, что имя, значение и request_id не пустые.
     * 2. Проверить уникальность request_id (защита от дублей).
     * 3. Сохранить параметр в кэш.
     * 4. Отметить request_id как использованный.
     * 5. Вернуть статус OK или ошибку.
     *
     * @param context Контекст gRPC-сервера (передаётся автоматически).
     * @param request Запрос, содержащий имя, значение и request_id.
     * @param response Ответ, содержащий статус операции.
     * @return grpc::Status Статус выполнения операции.
     */
    grpc::Status SetParameter(
        grpc::ServerContext* context,
        const telemetry::v1::SetParameterRequest* request,
        telemetry::v1::SetParameterResponse* response) override;
};