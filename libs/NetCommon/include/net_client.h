#pragma once
#include "net_common.h"
#include "net_message.h"
#include "net_tsqueue.h"
#include "net_connection.h"

namespace net
{
    template <typename T>
    class iclient
    {
    public:
        iclient() : m_socket(m_ctx)
        {
        }

        virtual ~iclient()
        {
            Disconnect();
        }

    public:
        bool Connect(const std::string &host, const std::string port)
        {
            try
            {
                m_connection = std::make_unique<connection<T>>();
                asio::ip::tcp::resolver resolver(m_ctx);
                // Failed to resolve an address and port results throwing exception
                m_endpoint = resolver.resolve(host, port);

                m_connection->Connect(m_endpoint);
                m_zrdCtx = std::thread([this](){
                    m_ctx.run();
                });

            }
            catch(const std::exception& e)
            {
                std::cerr << e.what() << NL;
            }
            
            return false;
        }

        void Disconnect()
        {
            if(IsConnected())
            {
                m_connection->Disconnect();
            }
            m_ctx.stop();
            if(m_zrdCtx.joinable()){
                m_zrdCtx.join();
            }
            m_connection->reset();
        }

        bool IsConnected()
        {
            if (m_connection)
            {
                return m_connection.IsConnectd();
            }
            return false;
        }

        const tsqueue<owned_message<T>> &Incoming() const
        {
            return m_qIn;
        }

        void Send(message<T> msg){

        }
    protected:
        asio::io_context m_ctx;
        asio::ip::tcp::socket m_socket;
        asio::ip::tcp::endpoint m_endpoint;

        std::unique_ptr<connection<T>> m_connection;
        std::thread m_zrdCtx;

    private:
        tsqueue<owned_message<T>> m_qIn;
    };
} // namespace net
