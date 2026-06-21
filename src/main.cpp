/**
 * @file main.cpp
 * @brief Точка входа в gRPC-сервис управления параметрами.
 *
 * Этот файл содержит функцию main(), которая запускает gRPC-сервер,
 * и функцию RunServer(), которая создаёт и настраивает сервер.
 * Сервер слушает порт 50051 и обрабатывает запросы GetParameter и SetParameter.
 */

#include <grpcpp/grpcpp.h>                                // Основной заголовок gRPC для C++
#include "telemetry_service.h"                            // Заголовок с реализацией gRPC-сервиса
#include <iostream>                                       // Для ввода/вывода в консоль
#include <memory>                                         // Для std::unique_ptr
#include <grpcpp/ext/proto_server_reflection_plugin.h>    // Для рефлексии (grpcurl)

using grpc::Server;
using grpc::ServerBuilder;

/**
 * @brief Запускает gRPC-сервер и ожидает входящие запросы.
 *
 * Функция выполняет следующие шаги:
 * 1. Устанавливает адрес сервера (0.0.0.0:50051) — слушает все интерфейсы.
 * 2. Включает рефлексию, чтобы утилита grpcurl могла автоматически определять методы сервиса.
 * 3. Создаёт экземпляр класса TelemetryServiceImpl, который содержит логику обработки запросов.
 * 4. Настраивает ServerBuilder: добавляет порт и регистрирует сервис.
 * 5. Запускает сервер с помощью BuildAndStart().
 * 6. Если сервер не запустился, выводит сообщение об ошибке и завершает работу.
 * 7. При успешном запуске выводит сообщение в консоль и переходит в режим ожидания запросов (Wait()).
 *
 * @note Сервер работает в бесконечном цикле, пока не будет прерван (Ctrl+C).
 */

void RunServer() {
    // 1. Адрес, на котором сервер будет слушать
    std::string server_address("0.0.0.0:50051");

    // 2. Включение рефлексии, чтобы grpcurl знал о существующих методах
    grpc::reflection::InitProtoReflectionServerBuilderPlugin();

    // 3. Создание экземпляра текущего сервиса
    TelemetryServiceImpl service;

    // 4. Строитель сервера = сборка сервера по частям
    ServerBuilder builder;
    builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
    builder.RegisterService(&service);   // Передача указателя на service

    // 5. Запуск сервера
    std::unique_ptr<Server> server(builder.BuildAndStart());
    if (!server) {
        std::cerr << "Ошибка: не удалось запустить сервер!" << std::endl;
        return;
    }

    std::cout << "Сервер запущен на " << server_address << std::endl;

    // 6. "Вечное" ожидание запросов
    server->Wait();
}

/**
 * @brief Точка входа в программу.
 *
 * Устанавливает локаль для корректного отображения русских символов,
 * выводит приветственное сообщение и запускает сервер.
 *
 * @return int Код возврата (0 — успешное завершение).
 */

int main() {
    
    // Установка локали для поддержки русского языка в консоли
    setlocale(LC_ALL, "Rus");
    std::cout << "Telemetry gRPC Service v1.0" << std::endl;
    
    // Запуск сервера
    RunServer();

    return 0;
}