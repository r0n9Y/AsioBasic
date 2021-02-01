#pragma once

#include "net_common.h"
#include "net_message.h"
#include "net_tsqueue.h"
#include <memory>

constexpr std::string_view FMT_CONNECTION = "[Connection] %s";

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
	    connection(owner ownerType, asio::io_context& asioContext, tsqueue<owned_message<T>>& qIn, asio::ip::tcp::socket socket)
		    : m_ownerTYpe(ownerType), m_asioContext(asioContext), m_socket(std::move(socket)), m_qMessagesIn(qIn)
		{}

        virtual ~connection()
        {}

        public:
        bool Connect();

        void ConnectToClient(uint32_t uid = 0)
        {
            if(owner::server == m_ownerTYpe)
            {}
            else
            {
                spdlog::error(FMT_CONNECTION, "clint type is not able to invoke the function ConnectToClient()");
            }
        }

        void ConnectToServer()
        {}
        bool Disconnect();
        bool IsConnected() const;
        bool Send(const message<T>& msg);
        void ReadHeader()
        {}

        protected:
        asio::ip::tcp::socket m_socket;
        asio::io_context& m_asioContext;
        tsqueue<message<T>> m_qOut;
        tsqueue<owned_message<T>> m_qMessagesIn;
        owner m_ownerTYpe;
        uint32_t m_id = 0;
    };
} // namespace net