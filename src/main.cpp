#include <iostream>

#include <net_client.h>

enum class MsgType : uint32_t
{
    Fire,
    Move,
};

class MyClient : public net::iclient<MsgType>
{
    public:
    bool Fire(float x, float y)
    {
        net::message<MsgType> msg;
        msg.header.id = MsgType::Fire;
        msg << x << y;
        Send(msg);
        return true;
    }
};

int main()
{
    net::message<MsgType> msg;
    msg.header.id = MsgType::Fire;

    int a = 1;
    bool b = false;
    double c = 1.638;
    return 0;
}