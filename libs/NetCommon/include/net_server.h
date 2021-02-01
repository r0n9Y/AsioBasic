#pragma once

#include "net_common.h"
#include "net_tsqueue.h"
#include "net_message.h"
#include "net_connection.h"

namespace net
{
    template<typename T>
    class iserver
    {
        using Client = std::shared_ptr<connection<T>>;
        namespace azoiptcp = asio::ip::tcp;
    public:
        iserver(uint16_t port)
        :m_asioAcceptor(m_asioContext, azoiptcp::endpoint(asio::ip::tcp::v4(), port))
        {
            m_logger = spdlog::stdout_color_mt("Server");
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
            }
            catch(const std::exception& e)
            {
                // The server is not able to listening
                m_logger.error( "[Server] Exception {}", e.what());
                return false;
            }
            m_logger.info("[Server] started");
            return true;
        }

        void Stop()
        {
            m_logger.info("[Server] Stopping asio context ...");
            m_asioContext.stop();
            m_logger.info("[Server] Done. Stopping asio context thread ...");
            if(m_threadContext.joinable()) {
                m_threadContext.join();
            }
            m_logger.info("[Server] Done. Server is terminated");
        }

        // Asio wait for connections
        [[asio_async]]
        void WaitForClientConnection()
        {
            m_asioAcceptor.async_accept([this](std::error_code ec, asio::ip::tcp::socket socket)
            {
                if(ec)
                { 
                    m_logger.error("[Server] error {}", ec.message());
                }
                else
                {
                    m_logger.info("[Server] new connection {}", socket.remote_endpoint());
                    auto client = std::make_shared<connection<T>>(m_asioContext, m_qMessageIn, socket);

                    if(OnConnected(client))
                    {
                        m_dqConnections.push_back(std::move(client));   
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
                m_deqConnections.erase(std::remove(m_deqConnections.begin(), m_deqConnections.end(), client), m_deqConnections.end());
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
                m_deqConnections.erase(std::remove(m_deqConnections.begin(), m_deqConnections.end(), nullptr), m_deqConnections.end());
            }
        }

        void Update(std::size_t nMaxMessageCount = -1)
        {
            std::size_t nMsgCount = 0;
            while(nMsgCount < nMaxMessageCount)
            {
                nMsgCount++;
            }
        }

    protected:
        // Invoked when a client is trying to connect.
        // The srever is able to veto the connection by returning false
        virtual bool OnConnected(Client client)
        {
            return false;
        }

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
        std::thread m_threadContext;

        // asio way of getting connected sockets
        asio::ip::tcp::acceptor m_asioAcceptor;

        // activated connections
        std::deque<Client> m_dqConnections;

        uint32_t MAX_ID = 1000;

        std::shared_ptr<spdlog::logger> m_logger;
    };
}