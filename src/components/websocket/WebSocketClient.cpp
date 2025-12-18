#include "WebSocketClient.h"
#include <boost/beast/core.hpp>
#include <iostream>

namespace beast = boost::beast;

WebSocketClient::WebSocketClient(const std::string& host, const std::string& port)
    : host_(host), port_(port)
{
}

WebSocketClient::~WebSocketClient()
{
    Stop();
}

void WebSocketClient::Start()
{
    if (running_)
        return;

    running_ = true;
    
    // Start connection thread which will spawn sender/receiver
    connectThread_ = std::thread(&WebSocketClient::ConnectAndRun, this);
}

void WebSocketClient::Stop()
{
    std::cout << "Stop() called\n";
    running_ = false;
    sendCv_.notify_all();

    // Close the WebSocket to unblock any pending reads
    {
        std::lock_guard<std::mutex> lock(wsMutex_);
        if (ws_)
        {
            beast::error_code ec;
            ws_->close(websocket::close_code::normal, ec);
        }
    }

    if (sendThread_.joinable())
        sendThread_.join();
    if (recvThread_.joinable())
        recvThread_.join();
    if (connectThread_.joinable())
        connectThread_.join();
}

void WebSocketClient::Send(const std::string& msg)
{
    std::cout << "Send() called with: " << msg << " (connected=" << connected_ << ")\n";
    {
        std::lock_guard<std::mutex> lock(sendMutex_);
        sendQueue_.push(msg);
        std::cout << "Message queued. Queue size: " << sendQueue_.size() << "\n";
    }
    sendCv_.notify_one();
}

bool WebSocketClient::PollMessage(std::string& outMsg)
{
    std::lock_guard<std::mutex> lock(recvMutex_);
    if (recvQueue_.empty())
        return false;

    outMsg = std::move(recvQueue_.front());
    recvQueue_.pop();
    return true;
}

void WebSocketClient::ConnectAndRun()
{
    try
    {
        std::cout << "Connecting to " << host_ << ":" << port_ << "\n";
        
        net::io_context ioc;
        tcp::resolver resolver{ioc};
        
        // Create WebSocket stream
        auto wsTemp = std::make_unique<websocket::stream<tcp::socket>>(ioc);

        auto results = resolver.resolve(host_, port_);
        std::cout << "Resolved host\n";
        
        net::connect(wsTemp->next_layer(), results);
        std::cout << "TCP connected\n";
        
        wsTemp->handshake(host_, "/");
        std::cout << "WebSocket handshake complete\n";

        wsTemp->set_option(
            websocket::stream_base::timeout::suggested(beast::role_type::client)
        );

        // Store the WebSocket stream
        {
            std::lock_guard<std::mutex> lock(wsMutex_);
            ws_ = std::move(wsTemp);
        }

        connected_ = true;
        std::cout << "WebSocket connected and ready\n";

        // Start sender and receiver threads
        sendThread_ = std::thread(&WebSocketClient::SendLoop, this);
        recvThread_ = std::thread(&WebSocketClient::ReceiveLoop, this);

        // Wait for threads to finish
        if (sendThread_.joinable())
            sendThread_.join();
        if (recvThread_.joinable())
            recvThread_.join();

        connected_ = false;
        std::cout << "Both threads finished\n";
    }
    catch (const std::exception& e)
    {
        connected_ = false;
        std::cerr << "WebSocket connection error: " << e.what() << "\n";
    }

    std::cout << "WebSocket connection thread exited\n";
}

void WebSocketClient::SendLoop()
{
    std::cout << "Send thread started (thread_id=" << std::this_thread::get_id() << ")\n";
    
    try
    {
        while (running_)
        {
            std::unique_lock<std::mutex> lock(sendMutex_);
            
            // Wait for messages to send or stop signal
            sendCv_.wait(lock, [this] { 
                return !sendQueue_.empty() || !running_; 
            });

            if (!running_)
                break;

            if (!sendQueue_.empty())
            {
                std::string msg = std::move(sendQueue_.front());
                sendQueue_.pop();
                lock.unlock();

                std::cout << "Attempting to send: '" << msg << "'\n";

                // Send the message - NO MUTEX! Boost.Beast allows concurrent read/write
                // as long as there's only one read and one write happening at a time
                if (ws_ && connected_)
                {
                    try
                    {
                        ws_->write(net::buffer(msg));
                        std::cout << "✓ Successfully sent: " << msg << "\n";
                    }
                    catch (const std::exception& e)
                    {
                        std::cerr << "✗ Write failed: " << e.what() << "\n";
                        connected_ = false;
                        break;
                    }
                }
                else
                {
                    std::cout << "✗ Cannot send - ws_=" << (ws_ ? "valid" : "null") 
                              << " connected_=" << connected_ << "\n";
                }
            }
        }
    }
    catch (const std::exception& e)
    {
        std::cerr << "Send thread error: " << e.what() << "\n";
        connected_ = false;
    }

    std::cout << "Send thread exited\n";
}

void WebSocketClient::ReceiveLoop()
{
    std::cout << "Receive thread started (thread_id=" << std::this_thread::get_id() << ")\n";
    
    try
    {
        while (running_ && connected_)
        {
            beast::flat_buffer buffer;
            beast::error_code ec;

            std::cout << "Waiting for message...\n";

            // Read from WebSocket (blocking) - NO MUTEX!
            // Boost.Beast allows one read and one write concurrently
            if (ws_)
            {
                ws_->read(buffer, ec);
            }
            else
            {
                std::cout << "WebSocket is null in receive thread\n";
                break;
            }

            if (!ec)
            {
                std::string msg = beast::buffers_to_string(buffer.data());
                std::cout << "✓ Received: " << msg << "\n";

                if (messageCallback_)
                    messageCallback_(msg);

                {
                    std::lock_guard<std::mutex> lock(recvMutex_);
                    recvQueue_.push(msg);
                }
            }
            else if (ec == websocket::error::closed)
            {
                std::cout << "Server closed connection\n";
                break;
            }
            else if (ec != net::error::would_block)
            {
                std::cerr << "Read error: " << ec.message() << "\n";
                throw beast::system_error{ec};
            }
        }

        connected_ = false;
    }
    catch (const std::exception& e)
    {
        connected_ = false;
        std::cerr << "Receive thread error: " << e.what() << "\n";
    }

    std::cout << "Receive thread exited\n";
}