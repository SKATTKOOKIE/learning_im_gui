#pragma once

namespace control_mode
{
    enum class Command
    {
        NONE,
        SET_STANDBY,
        SET_ACTIVE,
    };

    enum class State
    {
        UNKNOWN,
        STANDBY,
        ACTIVE,
        SAFE,
        ESTOP
    };
}