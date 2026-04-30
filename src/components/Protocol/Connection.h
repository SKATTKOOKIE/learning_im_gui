#pragma once

namespace connection
{
    enum class Command
    {
        NONE,
        CONNECT,
        DISCONNECT
    };

    enum class State
    {
        UNKNOWN,
        CONNECTED,
        DISCONNECTED
    };
}