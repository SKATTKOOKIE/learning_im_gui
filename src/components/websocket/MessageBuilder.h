#pragma once

#include <string>
#include <sstream>
#include <vector>
#include <variant>
#include <map>
#include <memory>
#include <iostream>
#include <cstdio>

// Base interface for message builders
class IMessageBuilder
{
public:
    virtual ~IMessageBuilder() = default;
    virtual std::string Build() const = 0;
};

// JSON message builder with fluent interface
class JsonMessageBuilder : public IMessageBuilder
{
public:
    using ValueType = std::variant<int, double, std::string, bool>;

    JsonMessageBuilder& AddField(const std::string& key, int value)
    {
        fields_.push_back({key, value});
        return *this;
    }

    JsonMessageBuilder& AddField(const std::string& key, double value)
    {
        fields_.push_back({key, value});
        return *this;
    }

    JsonMessageBuilder& AddField(const std::string& key, const std::string& value)
    {
        fields_.push_back({key, value});
        return *this;
    }

    JsonMessageBuilder& AddField(const std::string& key, bool value)
    {
        fields_.push_back({key, value});
        return *this;
    }

    // Add a nested object
    JsonMessageBuilder& AddObject(const std::string& key, const JsonMessageBuilder& nestedBuilder)
    {
        nestedObjects_.push_back({key, nestedBuilder.Build()});
        return *this;
    }

    std::string Build() const override
    {
        std::ostringstream oss;
        oss << "{";
        bool needsComma = false;

        // Add regular fields
        for (const auto& [key, value] : fields_)
        {
            if (needsComma) oss << ",";
            needsComma = true;
            oss << "\"" << key << "\":";
            
            std::visit([&oss](auto&& arg) {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, std::string>)
                    oss << "\"" << arg << "\"";
                else if constexpr (std::is_same_v<T, bool>)
                    oss << (arg ? "true" : "false");
                else
                    oss << arg;
            }, value);
        }

        // Add nested objects
        for (const auto& [key, jsonStr] : nestedObjects_)
        {
            if (needsComma) oss << ",";
            needsComma = true;
            oss << "\"" << key << "\":" << jsonStr;
        }

        oss << "}";
        return oss.str();
    }

    void Clear()
    {
        fields_.clear();
        nestedObjects_.clear();
    }

private:
    std::vector<std::pair<std::string, ValueType>> fields_;
    std::vector<std::pair<std::string, std::string>> nestedObjects_;
};

// Interface for data payload structures
class IDataPayload
{
public:
    virtual ~IDataPayload() = default;
    virtual void PopulateBuilder(JsonMessageBuilder& builder) const = 0;
};

// Interface for envelope-level payloads (command, response, telemetry)
class IEnvelopePayload
{
public:
    virtual ~IEnvelopePayload() = default;
    virtual std::string Build() const = 0;
};

// Helper class to build command messages
class CommandMessageBuilder
{
public:
    static std::string BuildConnectCommand(int commandValue) // 0=query, 1=connect, 2=disconnect
    {
        JsonMessageBuilder dataBuilder;
        dataBuilder.AddField("command", commandValue);
        
        JsonMessageBuilder commandBuilder;
        commandBuilder.AddField("type", 1) // COMMAND_TYPE_CONNECT_COMMAND
                      .AddObject("data", dataBuilder);
        
        JsonMessageBuilder rootBuilder;
        rootBuilder.AddObject("command", commandBuilder);
        
        return rootBuilder.Build();
    }

    static std::string BuildControlModeCommand(int commandValue) // 0=query, 1=connect, 2=disconnect
    {
        JsonMessageBuilder dataBuilder;
        dataBuilder.AddField("command", commandValue);
        
        JsonMessageBuilder commandBuilder;
        commandBuilder.AddField("type", 2) // COMMAND_TYPE_CONNECT_COMMAND
                      .AddObject("data", dataBuilder);
        
        JsonMessageBuilder rootBuilder;
        rootBuilder.AddObject("command", commandBuilder);
        
        return rootBuilder.Build();
    }
};

// Helper class to parse response messages
class ResponseMessageParser
{
public:
    enum class ResponseCode
    {
        None = 0,
        Accepted = 1,
        Rejected = 2
    };

    static bool TryParse(const std::string& jsonStr, ResponseCode& code, std::string& description)
    {
        // Simple string-based parsing for response envelope
        // Expected format: {"response": {"type": 1, "data": {"code": <int>, "description": "<str>"}}}
        
        if (jsonStr.find("\"response\"") == std::string::npos)
        {
            std::cout << "ResponseMessageParser: No 'response' field found\n";
            return false;
        }
        
        // Extract code
        const std::string codeSearch = "\"code\":";
        size_t codePos = jsonStr.find(codeSearch);
        if (codePos == std::string::npos)
        {
            std::cout << "ResponseMessageParser: No 'code' field found\n";
            return false;
        }
        
        codePos += codeSearch.size();
        int codeValue = 0;
        if (sscanf(jsonStr.c_str() + codePos, "%d", &codeValue) != 1)
        {
            std::cout << "ResponseMessageParser: Failed to parse code value\n";
            return false;
        }
        
        code = static_cast<ResponseCode>(codeValue);
        
        // Extract description
        const std::string descSearch = "\"description\":\"";
        size_t descPos = jsonStr.find(descSearch);
        if (descPos != std::string::npos)
        {
            descPos += descSearch.size();
            size_t descEnd = jsonStr.find("\"", descPos);
            if (descEnd != std::string::npos)
            {
                description = jsonStr.substr(descPos, descEnd - descPos);
                std::cout << "ResponseMessageParser: Successfully extracted description: '" << description << "'\n";
            }
            else
            {
                std::cout << "ResponseMessageParser: Could not find closing quote for description\n";
            }
        }
        else
        {
            std::cout << "ResponseMessageParser: No 'description' field found\n";
        }
        
        return true;
    }
};

// Helper class to parse telemetry messages
class TelemetryMessageParser
{
public:
    enum class TelemetryType
    {
        None = 0,
        Connection = 1,
        ControlMode = 2,
        Jaw = 3
    };

    enum class ConnectionState
    {
        Unknown = 0,
        Connected = 1,
        Disconnected = 2,
    };

    enum class ControlModeState
    {
        Unknown = 0,
        Standby = 1,
        Active = 2,
    };

    static bool TryParseConnectionState(const std::string& jsonStr, ConnectionState& state)
    {
        if (jsonStr.find("\"telemetry\"") == std::string::npos)
            return false;

        // --- Extract type ---
        const std::string typeSearch = "\"type\":";
        size_t typePos = jsonStr.find(typeSearch);
        if (typePos == std::string::npos)
            return false;

        typePos += typeSearch.size();

        int typeValue = 0;
        if (sscanf(jsonStr.c_str() + typePos, "%d", &typeValue) != 1)
            return false;

        if (typeValue != 1)
            return false;

        // --- Extract state ---
        const std::string stateSearch = "\"state\":";
        size_t statePos = jsonStr.find(stateSearch);
        if (statePos == std::string::npos)
            return false;

        statePos += stateSearch.size();

        int stateValue = 0;
        if (sscanf(jsonStr.c_str() + statePos, "%d", &stateValue) != 1)
            return false;

        switch (stateValue)
        {
            case 0: state = ConnectionState::Unknown; break;
            case 1: state = ConnectionState::Connected; break;
            case 2: state = ConnectionState::Disconnected; break;
            default: return false;
        }

        return true;
    }


    static bool TryParseControlModeState(const std::string& jsonStr, ControlModeState& state)
    {
        if (jsonStr.find("\"telemetry\"") == std::string::npos)
            return false;
            
        // --- Extract type ---
        const std::string typeSearch = "\"type\":";
        size_t typePos = jsonStr.find(typeSearch);
        if (typePos == std::string::npos)
            return false;
            
        typePos += typeSearch.size();
        int typeValue = 0;
        // Fixed: Compare sscanf return value with 1 (successful parse), not the parsed value
        if (sscanf(jsonStr.c_str() + typePos, "%d", &typeValue) != 1)
        {
            return false;
        }
            
        if (typeValue != static_cast<int>(TelemetryType::ControlMode))
        {
            return false;
        }
            
        // --- Extract state ---
        const std::string stateSearch = "\"state\":";
        size_t statePos = jsonStr.find(stateSearch);
        if (statePos == std::string::npos)
            return false;
            
        statePos += stateSearch.size();
        int stateValue = 0;
        // Fixed: Compare sscanf return value with 1 (successful parse), not the parsed value
        if (sscanf(jsonStr.c_str() + statePos, "%d", &stateValue) != 1)
            return false;
            
        switch (stateValue)
        {
            case 0: state = ControlModeState::Unknown; break;
            case 1: state = ControlModeState::Standby; break;
            case 2: state = ControlModeState::Active; break;
            default: return false;
        }
        
        return true;
    }

};