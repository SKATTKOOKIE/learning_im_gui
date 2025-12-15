#pragma once
#include "components/websocket/MessageBuilder.h"

// Joystick control data payload (goes inside "data" field)
class JoystickDataPayload : public IDataPayload
{
public:
    int x = 0;
    int y = 0;
    int z = 0;
    int r = 0;  // roll
    int p = 0;  // pitch
    int h = 0;  // heading/yaw
    
    void PopulateBuilder(JsonMessageBuilder& builder) const override
    {
        builder.AddField("x", x)
               .AddField("y", y)
               .AddField("z", z)
               .AddField("r", r)
               .AddField("p", p)
               .AddField("h", h);
    }
};

// Example: Telemetry data payload
class TelemetryDataPayload : public IDataPayload
{
public:
    double temperature = 0.0;
    double depth = 0.0;
    double pressure = 0.0;
    
    void PopulateBuilder(JsonMessageBuilder& builder) const override
    {
        builder.AddField("temperature", temperature)
               .AddField("depth", depth)
               .AddField("pressure", pressure);
    }
};
