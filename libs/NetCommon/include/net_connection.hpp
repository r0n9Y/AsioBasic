#pragma once

#include "net_common.h"
#include "net_message.hpp"
#include "net_tsqueue.hpp"
#include "spdlog/sinks/stdout_color_sinks.h"
#include <memory>

constexpr std::string_view FMT_CONNECTION = "[Connection] {}";

namespace net
{
    template<typename T>
    class connection: std::enable_shared_from_this<connection<T>>
    {
        public:
        enum class owner
        {
            server,
            client,
        };
        public:
        // Constructor: Specify Owner, connect to context, transfer the socket
		//				Provide reference to incoming message queue
	    connection(owner ownerType, asio::io_context& asioContext, asio::ip::tcp::socket socket, tsqueue<owned_message<T>>& qIn)
		    : m_ownerType(ownerType)
            , m_asioContext(asioContext)
            , m_qMessagesIn(qIn)
            , m_socket(std::move(socket))
            
		{
            m_logger = spdlog::stdout_color_mt("Connection");
        }

        virtual ~connection()
        {}

        public:
        bool Connect();

        void ConnectToClient(uint32_t uid = 0)
        {
            if(owner::server == m_ownerType)
            {
                if(m_socket.is_open())
                {
                    m_id = uid;
                    m_logger->info(FMT_CONNECTION, uid);
                }
            }
            else
            {
                spdlog::error(FMT_CONNECTION, "clint type is not able to invoke the function ConnectToClient()");
            }
        }

        void ConnectToServer()
        {}
        bool Disconnect();
        bool IsConnected() const {
            return m_socket.is_open();
        }
        bool Send(const message<T>& msg);
        void ReadHeader()
        {
            asio::async_read(m_socket, asio::buffer(&m_msgIn.header, sizeof(m_msgIn.header)), 
            [this](std::error_code ec, std::size_t length)
            {
                if(ec)
                {
                    Error(fmt::format(" read header from socket [{}] failed, closing socket ...", m_id));

                } 
                else 
                {
                    if(m_msgIn.header.size > 0)
                    {
                        m_msgIn.body.resize(m_msgIn.header.size);
                        ReadBody();
                    } 
                    else 
                    {
                        AddToIncomingMessageQueue(m_msgIn);
                    }
                }
            });
        }
        void WriteHeader()
        {
            
        }

        void ReadBody()
        {
            asio::async_read(m_socket, asio::buffer(m_msgIn.body.data(), m_msgIn.header.size,
            [this](std::error_code ec, std::size_t length){
                if(ec)
                {
                    Error(std::string("{} read body failed, closing socket ...", m_id));
                }
                else
                {
                    AddToIncomingMessageQueue(m_msgIn);
                }
            }));
        }
        

        void WriteBody()
        {}
        uint32_t Id() const { return m_id; }

        void Error(std::string msg)
        {
            m_logger->error(msg);
            m_socket.close();
        }

        void AddToIncomingMessageQueue(tsqueue<owned_message<T>>&) 
        {
            return;
        }
        protected:
        asio::ip::tcp::socket m_socket;
        asio::io_context& m_asioContext;
        tsqueue<owned_message<T>>& m_qMessagesIn;
        tsqueue<message<T>> m_qOut;
        message<T> m_msgIn;
        message<T> m_msgOut;
        owner m_ownerType = owner::server;
        uint32_t m_id = 0;
        // logger
        std::shared_ptr<spdlog::logger> m_logger;
    };
} // namespace net