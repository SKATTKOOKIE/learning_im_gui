#pragma once
#include "core/UIComponent.h"
#include "components/websocket/MessageBuilder.h"
#include <array>

class JointStatusTable : public UIComponent
{
public:
    static constexpr int NUM_JOINTS = 7;

    JointStatusTable();
    void Render() override;
    void HandleTelemetry(const std::string& message);

private:
    std::array<JointStatusParser::JointStatus, NUM_JOINTS + 1> jointStatuses_;
};