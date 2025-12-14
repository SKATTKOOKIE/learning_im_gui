#pragma once
#include <string>
#include <sstream>
#include <vector>
#include <variant>
#include <map>
#include <memory>

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

// Common message envelope with header fields
struct MessageEnvelope
{
    std::string messageType = "rov";  // e.g., "rov", "telemetry", "status"
    int messageId = 0;
    int deviceId = 0;
    std::string version = "0.9.0";
    int sessionId = 0;
    
    std::string BuildWithPayload(const IDataPayload& payload) const
    {
        // Build the data payload
        JsonMessageBuilder dataBuilder;
        payload.PopulateBuilder(dataBuilder);
        
        // Build the outer envelope
        JsonMessageBuilder envelopeBuilder;
        envelopeBuilder.AddField("message_id", messageId)
                       .AddField("device_id", deviceId)
                       .AddField("version", version)
                       .AddField("session_id", sessionId)
                       .AddObject("data", dataBuilder);
        
        // Wrap in message type
        JsonMessageBuilder rootBuilder;
        rootBuilder.AddObject(messageType, envelopeBuilder);
        
        return rootBuilder.Build();
    }
};