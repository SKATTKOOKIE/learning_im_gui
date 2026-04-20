#pragma once

#include "components/websocket/MessageBuilder.h"

struct CommandData : public IDataPayload
{
    int command = 0; // 0=query, 1=connect, 2=disconnect
    
    void PopulateBuilder(JsonMessageBuilder& builder) const override
    {
        builder.AddField("command", command);
    }
};

struct ConnectionCommand : public IEnvelopePayload
{
    int type = 1; // COMMAND_TYPE_CONNECT_COMMAND
    CommandData data;
    
    std::string Build() const override
    {
        JsonMessageBuilder dataBuilder;
        data.PopulateBuilder(dataBuilder);
        
        JsonMessageBuilder commandBuilder;
        commandBuilder.AddField("type", type)
                      .AddObject("data", dataBuilder);
        
        JsonMessageBuilder rootBuilder;
        rootBuilder.AddObject("command", commandBuilder);
        
        return rootBuilder.Build();
    }
};