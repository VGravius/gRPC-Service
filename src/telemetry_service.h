#pragma once

#include <grpcpp/grpcpp.h>
#include "telemetry.grpc.pb.h"

class TelemetryServiceImpl final : public telemetry::v1::TelemetryService::Service {
public:
    grpc::Status GetParameter(
        grpc::ServerContext* context,
        const telemetry::v1::GetParameterRequest* request,
        telemetry::v1::GetParameterResponse* response) override;

    grpc::Status SetParameter(
        grpc::ServerContext* context,
        const telemetry::v1::SetParameterRequest* request,
        telemetry::v1::SetParameterResponse* response) override;
};