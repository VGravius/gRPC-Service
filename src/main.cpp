#include <grpcpp/grpcpp.h>
#include "telemetry_service.h"
#include <iostream>
#include <memory>
#include <grpcpp/ext/proto_server_reflection_plugin.h>
#include <clocale> 

using grpc::Server;
using grpc::ServerBuilder;

void RunServer() {
    // Адрес сервера
    std::string server_address("0.0.0.0:50051");
    // Создаём экземпляр сервиса
    TelemetryServiceImpl service;
    // Строитель сервера
    ServerBuilder builder;
    grpc::reflection::InitProtoReflectionServerBuilderPlugin();
    builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
    builder.RegisterService(&service);
    // Запуск сервера
    std::unique_ptr<Server> server(builder.BuildAndStart());
    if (!server) {
        std::cerr << "Ошибка: не удалось запустить сервер!" << std::endl;
        return;
    }
    std::cout << "Сервер запущен на " << server_address << std::endl;
    // Ожидание запросов
    server->Wait();
}

int main() {
    setlocale(LC_ALL, "Rus");
    std::cout << "Telemetry gRPC Service v1.0" << std::endl;
    RunServer();
    return 0;
}