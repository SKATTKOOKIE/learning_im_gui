#pragma once

#include <string>
#include <sstream>
#include <vector>
#include <variant>
#include <map>
#include <memory>
#include <iostream>
#include <cstdio>
#include "components/Protocol/ControlMode.h"

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

    using ControlModeState = control_mode::State;

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
            
        switch (static_cast<control_mode::State>(stateValue))
        {
            case control_mode::State::UNKNOWN:  
                state = control_mode::State::UNKNOWN;  
                break;
            case control_mode::State::STANDBY:  
                state = control_mode::State::STANDBY;  
                break;
            case control_mode::State::ACTIVE:   
                state = control_mode::State::ACTIVE;   
                break;
            default: 
                return false;
        }
        
        return true;
    }

};


class JointStatusParser
{
public:
    struct JointStatus
    {
        int   joint              = 0;
        int   operational_mode   = 0;
        int   control_mode       = 0;
        int   regeneration_mode  = 0;
        bool  brake_override     = false;
        bool  movement_in_progress = false;
        bool  use_halls          = false;
        bool  temperature_warning = false;
        bool  realtime_control_mode = false;
        bool  valid              = false;
    };

    static const char* JointName(int joint)
    {
        switch (joint)
        {
            case 1: return "Shoulder Az";
            case 2: return "Shoulder El";
            case 3: return "Elbow El";
            case 4: return "Wrist El";
            case 5: return "Wrist Az";
            case 6: return "Wrist Rot";
            case 7: return "Jaw";
            default: return "Unknown";
        }
    }

    static const char* OperationalModeName(int mode)
    {
        switch (mode)
        {
            case 0: return "Safe";
            case 1: return "Standby";
            case 2: return "Active";
            case 3: return "Identify";
            case 4: return "Periphery";
            case 5: return "Bring Up";
            case 6: return "Shutdown";
            default: return "Unknown";
        }
    }

    static const char* ControlModeName(int mode)
    {
        switch (mode)
        {
            case 0: return "Speed";
            case 1: return "Position";
            case 2: return "Torque";
            case 3: return "FOC ID";
            case 4: return "Speed ID";
            case 5: return "Hall ID";
            case 6: return "Power Mgmt";
            default: return "Unknown";
        }
    }

    static bool TryParse(const std::string& jsonStr, JointStatus& status)
    {
        if (jsonStr.find("\"telemetry\"") == std::string::npos)
            return false;

        const std::string typeSearch = "\"type\":";
        size_t typePos = jsonStr.find(typeSearch);
        if (typePos == std::string::npos) return false;
        typePos += typeSearch.size();
        int typeValue = 0;
        if (sscanf(jsonStr.c_str() + typePos, "%d", &typeValue) != 1) return false;
        if (typeValue != 4) return false;

        auto parseInt = [&](const std::string& key, int& out) -> bool {
            size_t pos = jsonStr.find(key);
            if (pos == std::string::npos) return false;
            pos += key.size();
            return sscanf(jsonStr.c_str() + pos, "%d", &out) == 1;
        };

        auto parseBool = [&](const std::string& key, bool& out) -> bool {
            size_t pos = jsonStr.find(key);
            if (pos == std::string::npos) return false;
            pos += key.size();
            while (pos < jsonStr.size() && jsonStr[pos] == ' ') pos++;
            if (jsonStr.substr(pos, 4) == "true")  { out = true;  return true; }
            if (jsonStr.substr(pos, 5) == "false") { out = false; return true; }
            return false;
        };

        parseInt("\"joint\":",               status.joint);
        parseInt("\"operational_mode\":",    status.operational_mode);
        parseInt("\"control_mode\":",        status.control_mode);
        parseInt("\"regeneration_mode\":",   status.regeneration_mode);
        parseBool("\"brake_override\":",     status.brake_override);
        parseBool("\"movement_in_progress\":", status.movement_in_progress);
        parseBool("\"use_halls\":",          status.use_halls);
        parseBool("\"temperature_warning\":", status.temperature_warning);
        parseBool("\"realtime_control_mode\":", status.realtime_control_mode);

        status.valid = true;
        return true;
    }
};