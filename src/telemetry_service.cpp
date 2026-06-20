#include "telemetry_service.h"

grpc::Status TelemetryServiceImpl::GetParameter(
    grpc::ServerContext* context,
    const telemetry::v1::GetParameterRequest* request,
    telemetry::v1::GetParameterResponse* response) {
    // Временная заглушка
    response->set_name(request->name());
    response->set_value("23.5");
    response->mutable_status()->set_code(0);
    return grpc::Status::OK;
}

grpc::Status TelemetryServiceImpl::SetParameter(
    grpc::ServerContext* context,
    const telemetry::v1::SetParameterRequest* request,
    telemetry::v1::SetParameterResponse* response) {
    response->mutable_status()->set_code(0);
    return grpc::Status::OK;
}