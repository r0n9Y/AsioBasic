#pragma once

#include "net_common.h"
#include "net_tsqueue.hpp"
#include "net_message.hpp"
#include "net_connection.hpp"
#include "spdlog/sinks/stdout_color_sinks.h"
namespace net
{
    template<typename T>
    class iserver
    {
    public:
        using Client = std::shared_ptr<connection<T>>;

        iserver(uint16_t port)
        :m_asioAcceptor(m_asioContext, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), port))
        {
            m_logger = spdlog::stdout_color_mt("Server");
            m_logger->info("Server constructor");
        }

        virtual ~iserver()
        {
            Stop();
        }

        bool Start()
        {
            try
            {
                WaitForClientConnection();
                m_taskAsioCtx = std::thread([this]() { m_asioContext.run(); });   
            }
            catch(const std::exception& e)
            {
                // The server is not able to listening
                m_logger->error( "[Server] Exception {}", e.what());
                return false;
            }
            m_logger->info("[Server] started");
            return true;
        }

        void Stop()
        {
            m_logger->info("[Server] Stopping asio context ...");
            m_asioContext.stop();
            m_logger->info("[Server] Done. Stopping asio context thread ...");
            if(m_taskAsioCtx.joinable()) {
                m_taskAsioCtx.join();
            }
            m_logger->info("[Server] Done. Server is terminated");
        }

        // Asio wait for connections
        // asio_async
        void WaitForClientConnection()
        {
            m_asioAcceptor.async_accept([this](std::error_code ec, asio::ip::tcp::socket socket)
            {
                if(ec)
                { 
                    m_logger->error("[Server] error {}", ec.message());
                }
                else
                {
                    m_logger->info("[Server] new connection {}", socket.remote_endpoint().address().to_string());
                    auto client = std::make_shared<connection<T>>(connection<T>::owner::server, m_asioContext, std::move(socket), m_qMessageIn);

                    if(OnConnected(client))
                    {
                        m_dqConnections.push_back(std::move(client));
                        m_dqConnections.back()->ConnectToClient(m_clientId++);
                        m_logger->info("Connection [{}]  approved", m_dqConnections.back()->Id());
                    }
                }

                // Prime the asio context with more work.
                WaitForClientConnection();
            });
        }

        // Send a message to a client
        void Message(Client client, const message<T>& msg)
        {
            if(client && client->IsConnected())
            {
                client->Send(msg);
            }
            else
            {
                if(client) {
                    OnDisconnected(client);
                    client.reset();
                }
                m_dqConnections.erase(std::remove(m_dqConnections.begin(), m_dqConnections.end(), client), m_dqConnections.end());
            }
        }

        // Mssages all clients
        void Broadcast(const message<T>& msg, Client pIgnoredClient)
        {
            bool hasDisconnectedClients = false;
            for(auto& client : m_dqConnections)
            {
                // [Todo] move the ignored clients to a separted contain
                if(client && client->IsConnected() && client != pIgnoredClient)
                {
                    client->Send(msg);
                }
                else
                {
                    OnDisconnected(client);
                    client->reset();
                    hasDisconnectedClients = true;
                }
            }

            if(hasDisconnectedClients)
            {
                m_dqConnections.erase(std::remove(m_dqConnections.begin(), m_dqConnections.end(), nullptr), m_dqConnections.end());
            }
        }

        void Update(std::size_t nMaxMessageCount = -1)
        {
            std::size_t nMsgCount = 0;
            while(nMsgCount < nMaxMessageCount && !m_qMessageIn.empty())
            {
                auto msg = m_qMessageIn.pop_front();
                OnMessageReceived(msg.remote, msg.content);
                nMsgCount++;
            }
        }

    protected:
        // Invoked when a client is trying to connect.
        // The srever is able to veto the connection by returning false
        virtual bool OnConnected(Client client) = 0;

        // Invoked when a client is disconnected
        virtual void OnDisconnected(Client client)
        {}

        // Invoked when a message is arrived
        virtual void OnMessageReceived(Client client, message<T>& msg)
        {}

    protected:
        // Thread queue for incoming message packages
        tsqueue<owned_message<T>> m_qMessageIn;

        // asio
        asio::io_context m_asioContext;
        std::thread m_taskAsioCtx;

        // asio way of getting connected sockets
        asio::ip::tcp::acceptor m_asioAcceptor;

        // activated connections
        std::deque<Client> m_dqConnections;

        uint32_t MAX_ID = 1000;
        uint32_t m_clientId = 0;

        std::shared_ptr<spdlog::logger> m_logger;
    };
}