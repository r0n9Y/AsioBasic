#pragma once

#include <net_server.hpp>

enum class ShutenDoujiMsgType : uint32_t
{
    ServerAccept,
    ServerDeny,
    ServerPing,
    Broadcast,
    Message
};

class ShutenDoujiServer : public net::iserver<ShutenDoujiMsgType> 
{
public:
    ShutenDoujiServer(uint16_t nPort) : iserver<ShutenDoujiMsgType>(nPort)
    {
    }

protected:
    virtual bool OnConnected(Client client)
    {
        return true;
    }

    // Invoked when a client is disconnected
    virtual void OnDisconnected(Client client)
    {
    }

    // Invoked when a message is arrived
    virtual void OnMessageReceived(Client client, net::message<ShutenDoujiMsgType> &msg)
    {
    }
};