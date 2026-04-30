#pragma once

namespace message_types
{
    enum class Command
    {
        NONE,
        SET_CONNECTION,
        SET_CONTROL_MODE,
        SET_POSITION_CONTROL,
        SET_JAW_CONTROL
    };

    enum class Telemetry
    {
        NONE,
        CONNECTION,
        CONTROL_MODE,
        POSITION_VELOCITY_TORQUE,
        JOINT_STATUS
    };

    enum class Response
    {
        NONE,
        ACCEPTED,
        REJECTED
    };
}