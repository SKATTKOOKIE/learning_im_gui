#pragma once
#include <boost/beast/websocket.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <string>
#include <thread>
#include <atomic>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <functional>

namespace websocket = boost::beast::websocket;
namespace net = boost::asio;
using tcp = net::ip::tcp;

class WebSocketClient
{
public:
    using MessageCallback = std::function<void(const std::string&)>;

    WebSocketClient(const std::string& host, const std::string& port);
    ~WebSocketClient();

    void Start();
    void Stop();

    void Send(const std::string& msg);
    bool PollMessage(std::string& outMsg);

    bool IsConnected() const { return connected_; }
    void SetMessageCallback(MessageCallback cb) { messageCallback_ = cb; }

private:
    void ConnectAndRun();
    void SendLoop();
    void ReceiveLoop();

    std::string host_;
    std::string port_;

    std::thread connectThread_;
    std::thread sendThread_;
    std::thread recvThread_;
    
    std::atomic<bool> running_{false};
    std::atomic<bool> connected_{false};

    // Shared WebSocket stream (protected by mutex for thread safety)
    std::unique_ptr<websocket::stream<tcp::socket>> ws_;
    std::mutex wsMutex_;

    std::queue<std::string> sendQueue_;
    std::mutex sendMutex_;
    std::condition_variable sendCv_;

    std::queue<std::string> recvQueue_;
    std::mutex recvMutex_;

    const bool websocketDebug_ = false;

    MessageCallback messageCallback_;
};